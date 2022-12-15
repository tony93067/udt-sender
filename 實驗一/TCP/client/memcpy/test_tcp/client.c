#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <netinet/tcp.h>
#include <pthread.h>

#define DIE(x) perror(x),exit(1)
#define PORT 12000
#define BUFFER_SIZE 10000

clock_t old_time,new_time;//use for count executing time
struct tms time_start, time_end;//use for count executing time
double ticks;

char cc_method[15] = {0};
int Background_TCP_Number = 0;
double total_recv_size = 0;
int t = 0;
void* timer(void* arg)
{
    int ex = open("TCP_Receiver.csv", O_CREAT|O_RDWR|O_APPEND, S_IRWXU);
	printf("timer create\n");
    while(1)
    {
        t++;
		sleep(1);
        if(t == 300)
        {
            if((new_time = times(&time_end)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
            ticks = sysconf(_SC_CLK_TCK);
            double execute_time = (new_time - old_time) / ticks;
            printf("Execute time : %2.2f\n", execute_time);
            printf("Total Recv Size : %lf", total_recv_size);

            char str[100] = {0};
            sprintf(str, "%s\n", "TCP");
            write(ex, str, sizeof(str));

            memset(str, '\0', sizeof(str));
            sprintf(str, "%s\t%d\n", "Background_TCP_Number", Background_TCP_Number);
            write(ex, str, sizeof(str));

            memset(str, '\0', sizeof(str));
            sprintf(str, "%s\t%s\n", "Congestion Control", cc_method);
            write(ex, str, sizeof(str));

            memset(str, '\0', sizeof(str));
            sprintf(str, "%s\t%lf\n\n", "Throughput(Mb/s)", (total_recv_size*8/1000000)/execute_time);
            write(ex, str, sizeof(str));
            pthread_exit(0);
        }
            
    }
}
int main(int argc, char **argv)
{
    int sd;
    static struct sockaddr_in server;
    char buf[256];
    socklen_t len;

    struct stat sb;
   
    pthread_t t1;
    char recv_buf[BUFFER_SIZE] = {0};

    long long int recv_size = 0;

    if(argc != 4)
    {
        printf("Usage: %s <server_ip> <Background TCP Number> <congestion control>\n",argv[0]);
        exit(1);
    }
    
    strcpy(cc_method, argv[3]);
    Background_TCP_Number = atoi(argv[2]);
    /* Set up destination address. */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(PORT);
    
    printf("Client Socket Open:\n");
    sd = socket(AF_INET,SOCK_STREAM,0);
    
    // get current congestion control
    len = sizeof(buf);
    if (getsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }

    printf("Current: %s\n", buf);
    
    // set current congestion control to bbr
    
    if(strcmp(argv[3], "bbr") == 0)
    {
        strcpy(buf, "bbr");

        len = strlen(buf);

        if (setsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
        {
            perror("setsockopt");
            return -1;
        }
    }
    len = sizeof(buf);
    if (getsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }

    printf("Change to: %s\n", buf);
    if(sd < 0)
    {
        DIE("socket");
    }

    /* Connect to the server. */
    if(connect(sd,(struct sockaddr*)&server,sizeof(server)) == -1)
    {
        DIE("connect");
    }

    printf("Start Receiving!\n");

    /*receive packet*/

    int fd;
    int j = 0;
    // open file 
    fd = open("file.txt", O_CREAT|O_RDWR|O_TRUNC, S_IRWXU);
    if (fd == -1) {
        perror("open\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        if((recv_size = recv(sd, (char *)recv_buf, BUFFER_SIZE, 0)) < 0)
        {
            DIE("send");
        }
        if(j == 0)
        {
            if(pthread_create(&t1, NULL, timer, NULL) != 0)
            {
                printf("pthread create error\n");
                exit(1);
            }
            if((old_time = times(&time_start)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
            j++;
        }
        if(write(fd, recv_buf, recv_size) < 0)
        {
            DIE("write");
        }
        total_recv_size += recv_size;
        if(t == 300)
            break;
    }
    if(pthread_join(t1, NULL) != 0)
    {
        printf("pthread_join error\n");
        exit(1);
    }
    
    close(fd);
    
    //close connection
    close(sd);

}


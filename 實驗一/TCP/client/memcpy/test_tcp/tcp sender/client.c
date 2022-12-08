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

int Background_TCP_Number = 0;
int t = 0;
void* timer(void* arg)
{
	printf("timer create\n");
    while(1)
    {
        t++;
		sleep(1);
        if(t == 301)
            exit(1);
    }
}
int main(int argc, char **argv)
{
    static struct sockaddr_in server;
    clock_t old,new;//use for count executing time
    struct tms time_start,time_end;//use for count executing time
    double ticks;
    struct stat sb;
    int sd;
    pthread_t t1;
    char buf[256];
    socklen_t len;

    
    int send_size = 0;
    // used to get mmap return virtual address
    char* file_addr;

    if(argc != 4)
    {
        printf("Usage: %s <server_ip> <Background TCP Number> <congestion control>\n",argv[0]);
        exit(1);
    }
    
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

    printf("Start Sending!\n");

    /*receive packet*/
    //start time
    if((old = times(&time_start)) == -1)
    {
        DIE("times");
    }

    int fd;
    FILE *ex = NULL;
    ex = fopen("TCP_sender.csv", "a");
    // open file 
    fd = open("/home/tony/實驗code/論文code/file.txt", O_RDONLY, S_IRWXU);
    if (fd == -1) {
        perror("open\n");
        exit(EXIT_FAILURE);
    }
    // get file info
    if(fstat(fd, &sb) == -1)
    {
        printf("fstat error\n");
        exit(1);
    }
    
    // map file to memory
    file_addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(file_addr == MAP_FAILED)
    {
        printf("mmap error\n");
        exit(1);
    }
    
    if(pthread_create(&t1, NULL, timer, NULL) != 0)
    {
        printf("pthread create error\n");
        exit(1);
    }
    if((send_size = send(sd, (char *)file_addr, sb.st_size, 0)) < 0)
    {
        DIE("send");
    }
    //finish time
    if((new = times(&time_end)) == -1)
    {
        DIE("times error\n");
    }
    if(munmap(file_addr, sb.st_size) == -1)
    {
        printf("munmap error\n");
        exit(1);
    }
    close(fd);
    
    /********************************/
    
    /*executing time*/
    ticks=sysconf(_SC_CLK_TCK);
    printf("Send Time: %2.2f\n",(double)(new-old)/ticks);
    printf("send Size: %d\n", send_size);
    fprintf(ex, "%s\n", "TCP");
    fprintf(ex, "%s\t%d\n", "Background_TCP_Number", Background_TCP_Number);
    fprintf(ex, "%s\t%f\n", "Send Time", (double)(new-old)/ticks);
    fprintf(ex, "\n");
    fclose(ex);
    //close connection
    close(sd);

}


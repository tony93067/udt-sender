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

#define DIE(x) perror(x),exit(1)
#define PORT 8888

int main(int argc, char **argv)
{
    static struct sockaddr_in server;
    clock_t old,new;//use for count executing time
    struct tms time_start,time_end;//use for count executing time
    double ticks;
    struct stat sb;
    int sd;
    
    int send_size = 0;
    // used to get mmap return virtual address
    char* file_addr;

    if(argc != 2)
    {
        printf("Usage: %s <server_ip>\n",argv[0]);
        exit(1);
    }
    
    /* Set up destination address. */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(PORT);
    
    printf("Client Socket Open:\n");
    sd = socket(AF_INET,SOCK_STREAM,0);
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

    int total_recv_size = 0;
    int fd;
    int len;
    FILE *ex = NULL;
    ex = fopen("test.csv", "a");
    // open file 
    fd = open("../../../../../file.txt", O_RDONLY, S_IRWXU);
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
    
    
    if((send_size = send(sd, (char *)file_addr, sb.st_size, 0)) < 0)
    {
        DIE("send");
    }
    
    if(munmap(file_addr, sb.st_size) == -1)
    {
        printf("munmap error\n");
        exit(1);
    }
    close(fd);
    //finish time
    if((new = times(&time_end)) == -1)
    {
        DIE("times error\n");
    }
    /********************************/
    
    /*executing time*/
    ticks=sysconf(_SC_CLK_TCK);
    printf("Send Time: %2.2f\n",(double)(new-old)/ticks);
    printf("send Size: %d\n", send_size);
    fprintf(ex, "%s\n", "TCP");
    fprintf(ex, "%s\t%f\n", "Send Time", (double)(new-old)/ticks);
    fclose(ex);
    //close connection
    close(sd);

}


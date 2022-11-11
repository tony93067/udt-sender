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
#include <netinet/tcp.h>


#define DIE(x) perror(x),exit(1)
#define BUFFER_SIZE 10000

int main(int argc, char **argv)
{
    // use to get socket info
    char buf[256];
    socklen_t len;

    struct timeval timeout = {0, 0};
    static struct sockaddr_in server;
    int sd;
    struct hostent *host;
    char buffer[BUFFER_SIZE];
    int port = atoi(argv[3]);
    

    printf("sizeof(buffer): %ld\n",sizeof(buffer));

    if(argc != 5)
    {
        printf("Usage: %s <server_ip> <client num> <port num> <CC Name>\n",argv[0]);
        exit(1);
    }

    /* Set up destination address. */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);    
    printf("Client Socket Open:\n");
    sd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    //int ret = setsockopt(sd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    if(sd < 0)
    {
        DIE("socket");
    }

    if (getsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }

    printf("Current: %s\n", buf);
    if(strcmp(argv[4], "bbr") == 0)
    {
        strcpy(buf, "bbr");

        len = strlen(buf);

        if (setsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
        {
            perror("setsockopt");
            return -1;
        }
    }
    /* Connect to the server. */
    if(connect(sd,(struct sockaddr*)&server,sizeof(server)) == -1)
    {
        DIE("connect");
    }

    printf("Client %s Start Sending!\n", argv[2]);
    // open file 
 
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        memset(buffer, '1', BUFFER_SIZE);
        if(send(sd, buffer,sizeof(buffer),0) < 0)
        {
            printf("%s\n", strerror(errno));
        }

    }
    /********************************/
    
    /*executing time*/
    //close connection
    close(sd);

}


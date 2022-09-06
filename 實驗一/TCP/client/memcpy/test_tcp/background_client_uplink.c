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

#define DIE(x) perror(x),exit(1)
#define BUFFER_SIZE 10000

int main(int argc, char **argv)
{
    struct timeval timeout = {0, 0};
    static struct sockaddr_in server;
    int sd;
    struct hostent *host;
    char buffer[BUFFER_SIZE];
    int port = atoi(argv[3]);
    

    printf("sizeof(buffer): %ld\n",sizeof(buffer));

    if(argc != 4)
    {
        printf("Usage: %s <server_ip> <client num> <port num>\n",argv[0]);
        exit(1);
    }

    /* Set up destination address. */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);    
    printf("Client Socket Open:\n");
    sd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    int ret = setsockopt(sd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    if(sd < 0)
    {
        DIE("socket");
    }

    /* Connect to the server. */
    if(connect(sd,(struct sockaddr*)&server,sizeof(server)) == -1)
    {
        DIE("connect");
    }

    printf("Client %s Start Sending!\n", argv[2]);
    int len;
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


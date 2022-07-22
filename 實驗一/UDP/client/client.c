#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1500
#define SERVER_PORT 8888
#define SERVER_IP "140.117.171.182"

void udp_send(int fd, struct sockaddr* server)
{
    int recv_size, send_size;
    socklen_t len = sizeof(*server);
    char buffer[BUFFER_SIZE];
    memcpy(buffer, "udp test\n", sizeof("udp test\n"));
    if((send_size = sendto(fd, buffer, BUFFER_SIZE, 0, server, len)) == -1)
    {
            printf("sendto error\n");
            exit(1);
    }
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        if((recv_size = recvfrom(fd, buffer, BUFFER_SIZE, 0, server, &len)) == -1)
        {
            printf("recvfrom error\n");
            exit(1);
        }else
        {
            if(recv_size == 0)
                break;
            printf("recv message : %s\n", buffer);
        }
    }
}

int main(int argc, char** argv)
{
    int socket_fd;
    struct sockaddr_in src;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    src.sin_family = AF_INET;
    src.sin_addr.s_addr = inet_addr(SERVER_IP);
    src.sin_port = htons(SERVER_PORT);

    udp_send(socket_fd, (struct sockaddr*)&src);

    close(socket_fd);

    return 0;
    
}
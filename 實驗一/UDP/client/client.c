#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 10000
#define SERVER_PORT 8888
#define SERVER_IP "140.117.171.182"

int main(int argc, char** argv)
{
    int socket_fd;
    struct sockaddr_in server_addr;
    int server_length = sizeof(server_addr);

    char buffer[BUFFER_SIZE];
    int recv_size, send_size;

    // set socket info
    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if(socket_fd < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    memset(buffer, '\0', BUFFER_SIZE);
    // Send the message to server:
    if((send_size = sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, server_length)) < 0){
        printf("Unable to send message\n");
        return -1;
    }
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        // Receive the server's response:
        if((recv_size = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &server_length) < 0)){
            printf("Error while receiving server's msg\n");
            return -1;
        }
        printf("Server's response: %s\n", buffer);
    }
    
    // Close the socket:
    close(socket_fd);
    
    return 0;
}
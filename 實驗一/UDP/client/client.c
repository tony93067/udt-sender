#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 10000
#define SERVER_PORT 8888
#define SERVER_IP "140.117.171.182"

int main(int argc, char** argv)
{
    int socket_fd;
    struct sockaddr_in server_addr;
    socklen_t server_length = sizeof(server_addr);

    clock_t old,new;//use for count executing time
    struct tms time_start,time_end;//use for count executing time
    // UDP recvt timeout
    struct timeval timeout={10,0};

    char buffer[BUFFER_SIZE];
    int recv_size, send_size;
    int start = 0;
    int total_recv_size = 0;
    double execute_time = 0;
    int result_fd;

    // set socket info
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fd < 0){
        printf("Error while creating socket\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    
    printf("Socket created successfully\n");
    memset(buffer, '\0', BUFFER_SIZE);
    strcpy(buffer, "Client Data");
    if(setsockopt(socket_fd, 0, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
    {
        printf("set socket Error");
        exit(1);
    }
    // Send the message to server:
    if((send_size = sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, server_length)) < 0){
        printf("Unable to send message\n");
        return -1;
    }
    printf("send size %d\n", send_size);
    memset(buffer, '\0', BUFFER_SIZE);
    recv_size = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &server_length);
    printf("Msg from server %s\n", buffer);
    printf("recv size %d\n", recv_size);
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        if(start == 0)
        {
            if((old = times(&time_start)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
        }
        // Receive the server's response:
        printf("hang\n");
        if((recv_size = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &server_length)) < 0){
            printf("Error while receiving server's msg\n");
            return -1;
        }
        printf("hangend\n");
        printf("Msg from Server %d\n", recv_size);
        //printf("strlen %ld \n", strlen(buffer));
        if(recv_size == 0)
            break;
        total_recv_size += recv_size;
        start++;
        printf("start %d\n", start);
        printf("-----------------------\n");
    }
    if((new = times(&time_end)) == -1)
    {
        printf("time error\n");
        exit(1);
    }
    execute_time = (double)(new - old)/sysconf(_SC_CLK_TCK) - 3;
    char str[100];
    // record result
    result_fd = open("result.txt", O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
    memset(str, '\0', 100);

    printf("Execute Time : %f\n", execute_time);
    printf("Data Loss : %f\n", total_recv_size/1000000000.);
    sprintf(str, "Execute Time : %f, Data Loss : %f\n\n", execute_time, total_recv_size/1000000000.);
    if(write(result_fd, str, strlen(str)) < 0)
    {
        printf("write error \n");
        exit(1);
    }
    // Close the file and socket
    close(result_fd);
    close(socket_fd);
    
    return 0;
}
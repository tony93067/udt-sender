#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/times.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


#define BUFFER_SIZE 10000
#define SERVER_PORT 8888
#define SERVER_IP "140.117.171.182"

clock_t old,new;//use for count executing time
struct tms time_start,time_end;//use for count executing time
int t = 0;

void* timer(void* arg)
{
    while(1)
    {
        t++;
        sleep(1);
        if(t == 301)
        {
            if((new = times(&time_end)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
        }
    }
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    int socket_fd;
    struct sockaddr_in server_addr;
    socklen_t server_length = sizeof(server_addr);

    char buffer[BUFFER_SIZE];
    struct stat sb;            // get file info
    int recv_size, send_size;
    int start = 0;
    double execute_time = 0;
    int result_fd;
    char* file_addr;
    pthread_t t1;

    int total_send_size = 0;
    
    if(argc != 3)
    {
    	printf("usage : ./client <server IP> <BK Number>\n");
    	exit(1);
    }

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
    
    // use to get server info
    memset(buffer, '\0', BUFFER_SIZE);
    strcpy(buffer, "Client Data");
    /*if(setsockopt(socket_fd, 0, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
    {
        printf("set socket Error");
        exit(1);
    }*/

    // Send the message to server:
    if((send_size = sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, server_length)) < 0){
        printf("Unable to send message\n");
        return -1;
    }
    printf("send size: %d\n", send_size);
    printf("send msg: %s\n", buffer);


    int fd = open("/home/tony/實驗code/論文code/file.txt", O_RDONLY, S_IRWXU);
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

    // get server info
    memset(buffer, '\0', BUFFER_SIZE);
    recv_size = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &server_length);
    printf("Msg from server: %s\n", buffer);
    printf("recv size %d\n", recv_size);
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        memcpy(buffer, file_addr + total_send_size, BUFFER_SIZE);
        if(start == 0)
        {
            if((old = times(&time_start)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
            if(pthread_create(&t1, NULL, timer, NULL) != 0)
            {
                printf("can't create t1 thread\n");
                exit(1);
            }
        }
        // send packet to server
        if((send_size = sendto(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, server_length)) < 0){
            printf("Error while sending msg to server\n");
            return -1;
        }
        start++;

        /*

        bandwidth : 100 Mb/s

        100000000 / 8 = 12500000 bytes

        12500000 / packet size (10000 Bytes) = 1250 Packets

        1000000(us) / 1250 = 800 us

        每 800 us 發送一個 Packet

        */
        usleep(800);
        if(t == 301)
            break;
        total_send_size += send_size;
        if(total_send_size == sb.st_size)
        	break;
    }

    // 已送出封包數量
    /*
    memset(buffer, '\0', BUFFER_SIZE);
    sprintf(buffer, "%d", start);
    if((send_size = sendto(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, server_length)) < 0){
        printf("Error while sending msg to server\n");
        return -1;
    }*/

    
    if(munmap(file_addr, sb.st_size) == -1)
    {
        printf("munmap error\n");
        exit(1);
    }
    // Close the file and socket
    close(fd);
    close(socket_fd);
    
    return 0;
}

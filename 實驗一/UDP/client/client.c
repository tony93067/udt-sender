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
double total_recv_packets = 0;
int Background_TCP_Number = 0;
int t = 0;
double ticks = 0;

void* timer(void* arg)
{
	int ex = open("UDP_Receiver.csv", O_CREAT|O_RDWR|O_APPEND, S_IRWXU);
    while(1)
    {
        t++;
        sleep(1);
        if(t == 300)
        {
            if((new = times(&time_end)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
            ticks = sysconf(_SC_CLK_TCK);
            double execute_time = (new - old)/ticks;
            printf("Execute Time: %2.2f\n", execute_time);
            printf("total recv size %f Bytes\n", total_recv_packets*BUFFER_SIZE);
            
            char str[100] = {0};
            sprintf(str, "%s\n", "UDP");
            write(ex, str, sizeof(str));
            
            memset(str, '\0', sizeof(str));
            sprintf(str, "%s\t%d\n", "Background_TCP_Number", Background_TCP_Number);
            write(ex, str, sizeof(str));
            
            double throughput = (total_recv_packets*BUFFER_SIZE*8/1000000);
            memset(str, '\0', sizeof(str));
            sprintf(str, "%s\t%lf\n", "Throughput(Mb/s)", throughput/execute_time);
            write(ex, str, sizeof(str));
            
            memset(str, '\0', sizeof(str));
            sprintf(str, "%s\t%lf\n\n", "Loss Rate: ", total_recv_packets/2500000);
            write(ex, str, sizeof(str));
            close(ex);
            break;
            
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
    int recv_size, send_size;
    int start = 0;
    double execute_time = 0;
    int result_fd;
    pthread_t t1;

    int total_send_size = 0;
    
    if(argc != 3)
    {
    	printf("usage : ./client <server IP> <BK Number>\n");
    	exit(1);
    }
	Background_TCP_Number = atoi(argv[2]);
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

    // get server info
    memset(buffer, '\0', BUFFER_SIZE);
    recv_size = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &server_length);
    printf("Msg from server: %s\n", buffer);
    printf("recv size %d\n", recv_size);
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        if((recv_size = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &server_length)) < 0){
            printf("Error while sending msg to server\n");
            return -1;
        }else
        {
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
        }
        start++;
        total_recv_packets++;
        // send packet to server
        

       
        if(t == 300)
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

    // Close the file and socket
    close(socket_fd);
    
    return 0;
}

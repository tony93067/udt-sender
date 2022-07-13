int n = 65535; // 1M
int nSendBuf=10240000;//设置为1M
int rsize = 0;
// send_buf (Cindy)
int sendbuf = 0;
int optlen;
optlen = sizeof(sendbuf);
UDT::getsockopt(client_data, 0, UDT_SNDBUF, &sendbuf, &optlen);
printf("UDT before set, send buffer size = %d\n", sendbuf);
setsockopt(client_data, 0, UDT_SNDBUF, new int(500), sizeof(int));
sendbuf = 0;
UDT::getsockopt(client_data, 0, UDT_SNDBUF, &sendbuf, &optlen);
printf("UDT after set, send buffer size = %d\n", sendbuf);

// recv_buf (Cindy)
int recvbuf = 0;
int optlen_rcv;
optlen_rcv = sizeof(recvbuf);
if(!(UDT::ERROR != UDT::getsockopt(client_data, 0, UDT_RCVBUF, &recvbuf, &optlen_rcv)))
{
printf("getsockopt\n");
}

printf("UDT before set, recv buf size = %d\n", recvbuf);

//setsockopt(client_data, 0, UDT_RCVBUF, &n, sizeof(n));
optlen = sizeof(recvbuf);
UDT::getsockopt(client_data, 0, UDT_RCVBUF, &recvbuf, &optlen);
printf("UDT after set, recv buf size = %d\n", recvbuf);

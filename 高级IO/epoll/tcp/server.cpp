#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct epoll_event epoll_event;

///////////////////////////////////////////////
//  初始化
///////////////////////////////////////////////

int ServerInit(const char* ip, short port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return -1;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    int ret = bind(fd, (sockaddr*)&addr, sizeof(addr));
    if (ret < 0)
    {
        perror("bind");
        return -1;
    }
    ret = listen(fd, 5);
    if (ret < 0)
    {
        perror("listen");
        return -1;
    }
    return fd;
}
void ProcessListenSock(int epoll_fd, int listen_sock)
{
    sockaddr_in peer;
    socklen_t len = sizeof(sockaddr_in);
    int new_sock = accept(listen_sock, (sockaddr*)&peer, &len);
    if (new_sock < 0)
    {
        perror("accept");
        return;
    }
    // 把 new_sock 加入 epoll 对象
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = new_sock;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_sock, &ev);
    if (ret < 0)
    {
        perror("epoll_ctl: add");
        return;
    }
    printf("[client %d] connected!\n", new_sock);
}

void ProcessNewSock(int epoll_fd, int new_sock)
{
    char buf[1024] = {0};
    ssize_t read_size = read(new_sock, buf, sizeof(buf)-1);
    if (read_size < 0)
    {
        perror("read");
        return;
    }
    if (read_size == 0)
    {
        close(new_sock);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, new_sock, NULL);
        printf("[client %d] disconnected!\n", new_sock);
        return;
    }
    buf[read_size] = '\0';
    printf("[client %d] says: %s\n", new_sock, buf);
    write(new_sock, buf, strlen(buf));
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage:%s [ip] [port]\n", argv[0]);
        return 1;
    }

    int listen_sock = ServerInit(argv[1], atoi(argv[2]));
    if (listen_sock < 0)
    {
        printf("ServerInit failed!\n");
        return 1;
    }
    printf("ServerInit ok!\n");
    int epoll_fd = epoll_create(2);
    if (epoll_fd < 0)
    {
        perror("epoll_create");
        return 1;
    }
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev);
    if (ret < 0)
    {
        perror("epoll_ctl: add");
        return 1;
    }
    while (1)
    {
        epoll_event output[100];
        int nfds = epoll_wait(epoll_fd, output, 100, -1);
        if (nfds < 0)
        {
            perror("epoll_wait");
            continue;
        }
        for (int i = 0; i < nfds; i++)
        {
            if (output[i].data.fd == listen_sock)
            {
                ProcessListenSock(epoll_fd, listen_sock);
            }
            else 
            {
                ProcessNewSock(epoll_fd, output[i].data.fd);
            }
        }
    }
    return 0;
}

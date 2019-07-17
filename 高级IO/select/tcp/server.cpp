#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

///////////////////////////////////////////////
//  封装fd_set
///////////////////////////////////////////////

typedef struct FdSet 
{
    fd_set m_fds;
    int m_max_fd;
}FdSet;

void FdSetInit(FdSet* set)
{
    if (NULL == set)
    {
        return;
    }
    set->m_max_fd = -1;
    FD_ZERO(&set->m_fds);
}

void FdSetAdd(FdSet* set, int fd)
{
    FD_SET(fd, &set->m_fds);
    if (fd > set->m_max_fd)
    {
        set->m_max_fd = fd;
    }
}

void FdSetDel(FdSet* set, int fd)
{
    FD_CLR(fd, &set->m_fds);
    int max_fd = -1;
    for (int i = 0; i <= set->m_max_fd; i++)
    {
        if (!FD_ISSET(i, &set->m_fds))
        {
            continue;
        }
        if (i > max_fd)
        {
            max_fd = i;
        }
    }
    set->m_max_fd = max_fd;
}

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

int ProcessResquest(int new_sock)
{
    char buf[1024] = {0};
    ssize_t read_size = read(new_sock, buf, sizeof(buf)-1);
    if (read_size < 0)
    {
        perror("read");
        return -1;
    }
    if (read_size == 0)
    {
        printf("[client %d] disconnect!\n", new_sock);
        return 0;
    }
    buf[read_size] = '\0';
    printf("[client %d] say: %s\n", new_sock, buf);
    write(new_sock, buf, strlen(buf));
    return read_size;
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
    // 进入事件循环
    FdSet read_fds;
    FdSetInit(&read_fds);
    FdSetAdd(&read_fds, listen_sock);
    while (1)
    {
        // 使用 select 完成等待
        FdSet output_fds = read_fds;
        int ret = select(output_fds.m_max_fd+1, &output_fds.m_fds, NULL, NULL, NULL);
        if (ret < 0)
        {
            perror("select");
            continue;
        }
        // select 返回之后进行处理
        //  1> listen_sock 就绪, 调用 accept
        //  2> new_sock 就绪, 调用 read
        if (FD_ISSET(listen_sock, &output_fds.m_fds))
        {
            sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int new_sock = accept(listen_sock, (sockaddr*)&peer, &len);
            if (new_sock < 0)
            {
                perror("accept");
                continue;
            }
            FdSetAdd(&read_fds, new_sock);
            printf("[client %d] connected!\n", new_sock);
        }
        else 
        {
            for (int i = 0; i < output_fds.m_max_fd+1; i++)
            {
                if (!FD_ISSET(i, &output_fds.m_fds))
                {
                    continue;
                }
                int ret = ProcessResquest(i);
                if (ret == 0)
                {
                    close(i);
                    FdSetDel(&read_fds, i);
                }
            }
        }
    }
    return 0;
}

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "Log.hpp"

class TcpSocket
{

public:
    TcpSocket(int sock = -1)
        : m_sock(sock)
    {}

    ~TcpSocket()
    {
        Close();
    }
public:
    int GetSock()
    {
        return m_sock;
    }

    bool Socket()
    {
        m_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (m_sock < 0)
        {
            return false;
        }
        return true;
    }

    void SocketOpt()
    {
        int opt = 1;
        setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    bool Bind(const std::string& ip, uint16_t port)
    {
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = inet_addr(ip.c_str());

        if (bind(m_sock, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            return false;
        }
        return true;
    }

    bool Listen()
    {
        if (listen(m_sock, 5) < 0)
        {
            return false;
        }
        return true;
    }

    bool Accept(int &sock, struct sockaddr* client, socklen_t *len)
    {
        sock = accept(m_sock, client, len);
        if (sock < 0)
        {
            return false;
        }
        return true;
    }

    bool Connect(const std::string& ip, uint16_t port)
    {
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr(ip.c_str());
        if (connect(m_sock, (struct sockaddr*)&server, sizeof(server)) < 0)
        {
            return false;
        }
        return true;
    }

    void Write(char buf[], int size)
    {
        write(m_sock, buf, size);
    }

    ssize_t Read(int sock, char buf[], int len)
    {
        ssize_t s = read(sock, buf, len);
        return s;
    }

    void ProcessConnect(int m_sock)
    {
        while (1)
        {
            char buf[1024] = {0};
            ssize_t read_size = read(m_sock, buf, sizeof(buf)-1);
            if (read_size < 0)
            {
                continue;
            }
            if (read_size == 0)
            {
                LOG(INFO, "客户端关闭连接");
                return;
            }
            buf[read_size] = '\0';
            std::cout << "Client:" << m_sock << buf << std::endl;
            write(m_sock, buf, strlen(buf));
        }
    }

    bool Close()
    {
        close(m_sock);
        LOG(INFO, "Close!");
        return true;
    }
private:
    int m_sock;
};

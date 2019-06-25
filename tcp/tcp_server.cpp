#include "common.hpp"

class Server
{
public:
    Server(std::string ip, uint16_t port)
        : m_ip(ip)
        , m_port(port)
    {}

    void Start()
    {
        if (!m_server_sock.Socket())
        {
            LOG(ERROR, "Create socket error!");
            return;
        }
        LOG(INFO, "Create socket ok!");
        m_server_sock.SocketOpt();
        if (!m_server_sock.Bind(m_ip, m_port))
        {
            LOG(ERROR, "Bind socket error!");
            return;
        }
        LOG(INFO, "Bind socket ok!");
        if (m_server_sock.Listen())
        {
            LOG(ERROR, "Listen socket error!");
        }
        LOG(INFO, "Listen socket ok!");
        for (; ; )
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int sock;
            m_server_sock.Accept(sock, (struct sockaddr*)&client, &len);
            if (sock < 0)
            {
                LOG(ERROR, "Accept error!");
                continue;
            }
            printf("get new link [%s:%d]...!\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            pthread_t id;
            int* sockp = (int *)malloc(sizeof(int));
            *sockp = sock;
            pthread_create(&id, NULL, service, (void*)sockp);
            pthread_detach(id);
        }
    }

    ~Server()
    {
    }
private:
    static void serviceIO(int sock)
    {
        char buf[1024];
        while (1)
        {
            ssize_t s = read(sock, buf, sizeof(buf)-1);
            if (s > 0)
            {
                buf[s] = 0;
                printf("client# %s\n", buf);
            }
            else if (0 == s)
            {
                printf("client quit!!!\n");
                break;
            }
            else 
            {
                perror("read error");
                break;
            }
        }
        close(sock);
    }

    static void *service(void *arg)
    {
        int sock = *(int*)arg;
        free(arg);
        serviceIO(sock);
    }
private:
    TcpSocket m_server_sock;
    std::string m_ip;
    uint64_t m_port;
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage:%s [ip] [port]\n", argv[0]);
        return 1;
    }

//    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
//    if (listen_sock < 0)
//    {
//        perror("socket error");
//        return 2;
//    }
//
//    struct sockaddr_in local;
//    local.sin_family = AF_INET;
//    local.sin_port = htons(atoi(argv[2]));
//    local.sin_addr.s_addr = inet_addr(argv[1]);
//
//    if (bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0)
//    {
//        perror("bind error");
//        return 3;
//    }
//    if (listen(listen_sock, 5) < 0)
//    {
//        perror("listen erroe");
//        return 4;
//    }
//
    Server s(argv[1], atoi(argv[2]));
    s.Start();
    return 0;
}

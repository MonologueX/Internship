#include "common.hpp"

class Client
{
public:
    Client(std::string ip, uint16_t port)
        : m_ip(ip)
        , m_port(port)
    {}

    void Start()
    {
        if (!m_client_sock.Socket())
        {
            LOG(ERROR, "create socket error!");
            return;
        }
        LOG(INFO, "create socket ok!");

        if (!m_client_sock.Connect(m_ip, m_port))
        {
            LOG(ERROR, "Connect error!");
            return;
        }
        char buf[1024];
        while (1)
        {
            printf("Please enter:");
            scanf("%s", buf);
            m_client_sock.Write(buf, strlen(buf));
        }
    }

    ~Client()
    {
    }
private:
private:
    TcpSocket m_client_sock;
    std::string m_ip;
    uint64_t m_port;
};


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage:%s ip port\n", argv[0]);
        return 1;
    }
    Client c(argv[1], atoi(argv[2]));
    c.Start();
    //int sock = socket(AF_INET, SOCK_STREAM, 0);
    //if (sock < 0)
    //{
    //    perror("socket error");
    //    return 2;
    //}
    //struct sockaddr_in server;
    //server.sin_family = AF_INET;
    //server.sin_port = htons(atoi(argv[2]));
    //server.sin_addr.s_addr = inet_addr(argv[1]);
    //if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    //{
    //    perror("connect error");
    //    return 3;
    //}
    //char buf[1024];
    //while (1)
    //{
    //    printf("Please enter:");
    //    scanf("%s", buf);
    //    write(sock, buf, strlen(buf));
    //}
    return 0;
}

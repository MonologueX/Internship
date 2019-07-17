#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int SetNoBlocck(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    if (flag <0)
    {
        printf("fcntl error");
        return -1;
    }
    int ret = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    if (ret < 0)
    {
        printf("fcntl error");
        return -1;
    }
    return 0;
}

int main()
{
    SetNoBlocck(0);
    while (1)
    {
        usleep(100000);
        printf(">");
        fflush(stdout);
        char buf[1024] = {0};
        ssize_t read_size = read(0, buf, sizeof(buf)-1);
        if (read_size < 0)
        {
            perror("read");
            continue;
        }
        if (read_size == 0)
        {
            printf("read done!\n");
            return 0;
        }
        buf[read_size] = '\0';
        printf("buf=%s", buf);
    }
    return 0;
}

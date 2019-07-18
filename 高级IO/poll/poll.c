#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

int main()
{
    while (1)
    {
        struct pollfd fds;
        fds.fd = 0;
        fds.events = POLLIN;
        int ret = poll(&fds, 1, -1);
        if (ret < 0)
        {
            perror("poll");
            return -1;
        }
        char buf[1024] = {0};
        ssize_t read_size = read(0, buf, sizeof(buf)-1);
        if (read_size < 0)
        {
            perror("read");
            return 1;
        }
        if (read_size == 0)
        {
            printf("read done!\n");
            return 0;
        }
        buf[read_size] = '\0';
        printf("user input: %s\n", buf);
    }
    return 0;
}

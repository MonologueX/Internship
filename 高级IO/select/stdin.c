#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

int main()
{
    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        int ret = select(1, &readfds, NULL, NULL, NULL);
        if (ret < 0)
        {
            perror("select");
            return 1;
        }
        if (!FD_ISSET(0, &readfds))
        {
            printf("readfds error!\n");
            return 1;
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
        printf("buf = %s\n", buf);
    }
    return 0;
}

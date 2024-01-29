
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define LOGGER_RX_FIFO      "/tmp/log_rx.fifo"      //logger rx fifo for all device.
#define WRITE_STRING        "write string test!\n"
static char pstr[] = WRITE_STRING;

int main(int argc, char *argv[])
{
    int writefd = open(LOGGER_RX_FIFO, O_WRONLY, 0);

    while (1)
    {
        if (writefd > 0)
        {
            write(writefd, pstr, strlen(pstr));
        }
        sleep(2);
    }

    close(writefd);
    return 0;
}
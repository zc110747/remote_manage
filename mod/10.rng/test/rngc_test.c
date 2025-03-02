/*
 * File      : rngc_test.c
 * test for key driver.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define RNGC_DEVICE "/dev/hwrng"

int main(int argc, const char *argv[])
{
    char buffer[64];
    int fd;
    int size = 0;

    fd = open(RNGC_DEVICE, O_RDONLY);
    if (fd == -1) {
        printf("open %s error\n", RNGC_DEVICE);
        return -1;
    }

    size = read(fd, buffer, sizeof(buffer));
    if (size >= 0) {
        printf("read size:%d\n", size);
        for (int i = 0; i < size; i++) {
            printf("%02x ", buffer[i]);
        }
    } else {
        printf("read failed:%d\n", size);
    }

    close(fd); 

    return 0;
}

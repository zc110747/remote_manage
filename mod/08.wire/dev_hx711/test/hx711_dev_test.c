/*
 * File      : hx711_test.c
 * test for key driver.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
 */
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>

#define HX711_DEVICE_NAME "/dev/hx711"

int main(int argc, const char *argv[])
{
    unsigned int val;
    int fd;
    int size = 0;

    fd = open(HX711_DEVICE_NAME, O_RDWR | O_NDELAY);
    if (fd == -1) {
        printf("open %s error", HX711_DEVICE_NAME);
        return -1;
    }

    while (1) {
        size = read(fd, (char *)&val, sizeof(val));
        if (size >= 0) {
            printf("read size:%d\n", size);
            printf("value:%d, 0x%x\n", val, val);
        }
    }

    close(fd); 

    return 0;
}
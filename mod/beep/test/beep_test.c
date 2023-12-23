/*
 * File      : beep_test.c
 * test for beep driver.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
 */
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

/**
 * 测试LED工作
 * 
 * @param NULL
 *
 * @return NULL
 */
int main(int argc, const char *argv[])
{
    unsigned char val = 1;
    int fd;

    fd = open("/dev/beep", O_RDWR | O_NDELAY);
    if (fd == -1)
    {
        printf("/dev/beep open error");
        return -1;
    }

    if (argc > 1){   
        val = atoi(argv[1]);
    }

    write(fd, &val, 1);

    close(fd); 

    exit(0);  
}

////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      key_async_test.cpp
//          GPIO1_18
//
//  Purpose:
//     测试按键输入
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      4/3/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static int fd;

#define KEY_DEV_NAME "/dev/key"

static void sigio_signal_func(int signum)
{
    unsigned int key_val = 0;

    read(fd, &key_val, sizeof(unsigned int));
    if (0 == key_val)
        printf("Key Press\n");
    else if (1 == key_val)
        printf("Key Release\n");
}

int main(int argc, char *argv[])
{
    int flags = 0;

    fd = open(KEY_DEV_NAME, O_RDONLY | O_NONBLOCK);
    if (0 > fd) {
        printf("ERROR: %s file open failed!\n", KEY_DEV_NAME);
        return -1;
    }

    signal(SIGIO, sigio_signal_func);
    fcntl(fd, F_SETOWN, getpid());
    flags = fcntl(fd, F_GETFD);
    fcntl(fd, F_SETFL, flags | FASYNC);

    for (;;) {
        sleep(2);
    }

    close(fd);
    return 0;
}

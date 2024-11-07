////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      key_rwio_test.cpp
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
#include <poll.h>

#define KEY_DEV_NAME "/dev/key"

int blockio_process(void);
int noblock_io_process(void);

int main(int argc, char *argv[])
{
    int mod;

    if (2 != argc) {
        mod = 0;
    } else {
        mod = atoi(argv[1]);
    }

    if (mod == 0)
        blockio_process();
    else
        noblock_io_process();

    return 0;
}

int noblock_io_process(void)
{
    int fd;
    int key_val;

    fd = open(KEY_DEV_NAME, O_RDONLY | O_NONBLOCK);
    if(0 > fd) {
        printf("ERROR: %s file open failed!\n", KEY_DEV_NAME);
        return -1;
    }

    for (;;) {
        read(fd, &key_val, sizeof(int));
        if (0 == key_val) {
            printf("Key Press\n");
        }
        else if (1 == key_val) {
            printf("Key Release\n");
        }
    }

    close(fd);
}

int blockio_process(void)
{
    int fd, ret;
    int key_val;
    fd_set readfds;

    fd = open(KEY_DEV_NAME, O_RDONLY);
    if(0 > fd) {
        printf("ERROR: %s file open failed!\n", KEY_DEV_NAME);
        return -1;
    }

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    for (;;) {
        ret = select(fd + 1, &readfds, NULL, NULL, NULL);
        switch (ret) {
        case 0:
            break;

        case -1:
            break;

        default:
            if(FD_ISSET(fd, &readfds)) {
                read(fd, &key_val, sizeof(int));
                if (0 == key_val) {
                    printf("Key Press\n");
                }
                else if (1 == key_val) {
                    printf("Key Release\n");
                }
            }
            break;
        }
    }

    close(fd);    
}
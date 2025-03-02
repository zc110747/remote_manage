////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//     joystick_test.c
//
//  Purpose:
//     遥感输入检测
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      4/3/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/select.h>

#define JOYSTICK_TEST_PATH "/dev/input/event2"

static int check_joystick_value(int fd) {
    assert(fd >= 0);

    /* wait button being pressed or released. */
    fd_set input;
    FD_ZERO(&input);
    FD_SET(fd, &input);
    int ret = select(fd + 1, &input, NULL, NULL, NULL);
    if (ret < 0) {
        printf("%s", strerror(errno));
        return -1;
    }

    /* read event */
    struct input_event buf;
    if (read(fd, &buf, sizeof(struct input_event)) < 0) {
        printf("%s", strerror(errno));
        return -1;
    }

    switch (buf.type) {
        case EV_KEY:
            if (buf.code < BTN_MISC) {
                printf("key %d %s\n", buf.code, buf.value ? "press" : "release");
            } else {
                printf("button %d %s\n", buf.code, buf.value ? "press" : "release");
            }
            break;
        case EV_REL:
            break;
        case EV_ABS:
            printf("abs code%d value:%d ", buf.code, buf.value);
            if (buf.code == ABS_Y) {
                printf("\n");
            }
            break;
        case EV_MSC:
            break;
        case EV_SW:
            break;
    }
    return buf.code;
}

int main(int argc, const char *argv[])
{
    unsigned char val = 1;
    int fd;
    struct input_event inputevent;

    fd = open(JOYSTICK_TEST_PATH, O_RDWR);
    if (fd < 0) {
        printf("%s open failed!\n", JOYSTICK_TEST_PATH);
        return -1;
    } else {
        while (1) {
            int key_code = check_joystick_value(fd);
            if (key_code < 0)
                continue;
        }
    }

    close(fd);
    return 0;
}

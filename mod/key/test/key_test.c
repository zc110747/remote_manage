/*
 * File      : key_test.c
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

#define KEY_DEV_NAME "/dev/input/event1"

int main(int argc, const char *argv[])
{
    unsigned char val = 1;
    int flags;
	int fd;
    struct input_event inputevent;

    fd = open(KEY_DEV_NAME, O_RDWR);
    if (fd < 0)
    {
        printf("%s open failed!\n", KEY_DEV_NAME);
        return -1;
    }
    else
    {
        while (1) 
        {
            flags = read(fd, &inputevent, sizeof(inputevent));
            if (flags > 0) 
            {
                switch (inputevent.type) 
                {
                    case EV_KEY:
                        if (inputevent.code < BTN_MISC) 
                        {
                            printf("key %d %s\n", inputevent.code, inputevent.value ? "press" : "release");
                        } 
                        else 
                        {
                            printf("button %d %s\n", inputevent.code, inputevent.value ? "press" : "release");
                        }
                        break;
                    case EV_REL:
                        break;
                    case EV_ABS:
                        break;
                    case EV_MSC:
                        break;
                    case EV_SW:
                        break;
                }
            } 
            else 
            {
                printf("read data failed!\n");
                break;
            }
        }
    }

    close(fd);
    return 0;
}

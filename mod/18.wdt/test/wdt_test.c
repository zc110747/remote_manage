/*
 * File      : key_test.c
 * test for key driver.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/watchdog.h>

static unsigned int flag = 0;

#define WATCHDOG_DEVICE "/dev/watchdog1"

int main(int argc, char *argv[])
{
    int wdt_fd;
    int retval, flags;
    int timeout = 60;

    wdt_fd = open(WATCHDOG_DEVICE, O_RDWR | O_NONBLOCK);

    if (wdt_fd>=0)
    {
        ioctl(wdt_fd, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);
        ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);

        while (!flag)
        {
            ioctl(wdt_fd, WDIOC_KEEPALIVE, NULL);
            sleep(1);
        }

        ioctl(wdt_fd, WDIOC_SETOPTIONS, WDIOS_DISABLECARD);
        close(wdt_fd);
    }

    return 0;
}
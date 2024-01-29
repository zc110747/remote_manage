/*
 * File      : key_test.c
 * test for key driver.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
 */
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>

#define KEY_DEV_NAME "/dev/key"

int fd;

/*
 * SIGIO信号处理函数
 * @param - signum : 信号值
 * @return : 无
 */
static void sigio_signal_func(int signum)
{
    int err = 0;
    unsigned int keyvalue = 0;

    err = read(fd, &keyvalue, sizeof(keyvalue));
    if (err < 0) 
    {
        /* 读取错误 */
    } 
    else 
    {
        printf("sigio signal! key value=%d\r\n", keyvalue);
    }
}

int main(int argc, const char *argv[])
{
    unsigned char val = 1;
    int flags;

    fd = open(KEY_DEV_NAME, O_RDWR | O_NDELAY);
    if (fd < 0)
    {
        printf("%s open failed, error:%s", KEY_DEV_NAME, strerror(errno));
        return -1;
    }
    else
    {
        /* 设置信号SIGIO的处理函数 */
        signal(SIGIO, sigio_signal_func);

        fcntl(fd, F_SETOWN, getpid());      /* 设置当前进程接收SIGIO信号 */
        flags = fcntl(fd, F_GETFL);         /* 获取当前的进程状态 */
        fcntl(fd, F_SETFL, flags | FASYNC); /* 设置进程启用异步通知功能 */

        while (1){
            sleep(2);
        }
    }

    close(fd);
    return 0;
}

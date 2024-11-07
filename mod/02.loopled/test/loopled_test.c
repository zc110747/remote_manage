////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      loopled_test.c
//
//  Purpose:
//      LED红绿灯带应用层控制。
//      设备文件: /dev/loopled
//      
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022  Create New Version.
//      23/08/202   Format update.     
/////////////////////////////////////////////////////////////////////////////
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define LOOP_LED_DEVICE     "/dev/loopled"
#define LOOP_LED_NUM        3

/**
 * 测试LED工作
 * 
 * @param NULL
 *
 * @return NULL
 */
int main(int argc, const char *argv[])
{
    unsigned char val[2];
    unsigned char index, on_num = 0;
    int fd;
    int time_loop = 2;

    fd = open(LOOP_LED_DEVICE, O_RDWR | O_NDELAY);
    if (fd == -1) {
        printf("%s open failed!\n", LOOP_LED_DEVICE);
        return -1;
    }

    if (argc > 1) {
        time_loop = atoi(argv[1]);
    }

    while (1) {
        for (index=0; index<LOOP_LED_NUM; index++) {
            val[0] = index;
            if (index == on_num){
                val[1] = 1;
            } else {
                val[1] = 0;
            }
            write(fd, val, 2);
        }

        on_num++;
        if (on_num == LOOP_LED_NUM){
            on_num = 0;
        }
        sleep(time_loop);
    }

    close(fd); 
    exit(0);  
}

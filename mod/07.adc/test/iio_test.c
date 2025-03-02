////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      iio_test.c
//
//  Purpose:
//      读取iio设备目录下的文件，对应文件
//      /sys/bus/iio/devices/iio:device0/in_voltage%d_raw
//      GPIO_1_2 - CSI_RESET
//      GPIO_1_4 - CSI_PWDN 
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022  Create New Version.
//      14/02/2025   Format update.     
/////////////////////////////////////////////////////////////////////////////
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAILT_RAW_NUM     2
#define IIO_CHANNEL_FILE    "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw"

int file_data_read(char *filename, char *str, size_t size)
{
    int ret = 0;
    FILE *file;

    file = fopen(filename, "r"); /* 只读打开 */
    if (file == NULL) {
        printf("can't open file %s\r\n", filename);
        return -1;
    }

    ret = fread(str, 1, size, file);
    if (ret < 0) {
        return ret;
    }

    fseek(file, 0, SEEK_SET);
    fclose(file);

    return 0;
}

int main(int argc, const char *argv[])
{
    int ret;
    int value;
    char str[50], filepath[50];
    int val = DEFAILT_RAW_NUM;

    if (argc > 1) {   
        val = atoi(argv[1]);
    }

    sprintf(filepath, IIO_CHANNEL_FILE, val);

    while (1) {

        ret = file_data_read(filepath, str, sizeof(str));
        if (ret == 0) {
            value = atoi(str);
            printf("read_adc vol:%d, voltage:%3f\n", value, (float)value/4096*3.3);
        } else {
            printf("read failed, error flag:%d", ret);
        }

        sleep(1);
    }

    return 0;
}

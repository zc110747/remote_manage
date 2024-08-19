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

#define h711_path "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"

int file_data_read(char *filename, char *str)
{
    int ret = 0;
    FILE *data_stream;

    data_stream = fopen(filename, "r"); /* 只读打开 */
    if (data_stream == NULL) 
    {
        printf("can't open file %s\r\n", filename);
        return -1;
    }

    ret = fscanf(data_stream, "%s", str);
    if (!ret) 
    {
        printf("file read error!\r\n");
    }
    else if (ret == EOF) 
    {
        /* 读到文件末尾的话将文件指针重新调整到文件头 */
        fseek(data_stream, 0, SEEK_SET);
    }
    fclose(data_stream); /* 关闭文件 */
    return 0;
}

int main(int argc, const char *argv[])
{
    int ret;
    char str[50];

    ret = file_data_read(h711_path, str);
    if (ret == 0)
    {
        printf("read_gx711:、%s\n", str);
    }

    return 0;
}

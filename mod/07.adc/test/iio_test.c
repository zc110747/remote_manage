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
#include <stdlib.h>
#include <unistd.h>

#define h711_path "/sys/bus/iio/devices/iio:device0/in_voltage4_raw"

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
    char str[50];

    while (1) {
        ret = file_data_read(h711_path, str, sizeof(str));
        if (ret == 0) {
            value = atoi(str);
            printf("read_gx711:%d, voltage:%3f\n", value, (float)value/4096*3.3);
        } else {
            printf("read failed, error flag:%d", ret);
        }

        sleep(1);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_info.c
//
//  Purpose:
//      get the linux device information.
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/21/2023   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>

void data_test(void);

#define PATH "test_file"
int main(int argc, char **argv)
{
    FILE *file;
    int len;
    char buffer[32];

    data_test();

    file = fopen(PATH, "r");
    if (!file){
        return -1;
    }

    len = fread(buffer, 1, sizeof(buffer), file);
    if (len > 0)
    {
        buffer[len] = '\0';
        printf("read file:%s, size:%d\n", buffer, len);
    }

    fclose(file);
    return 0;
}

typedef union 
{
    int i_val;

    float f_val;

    uint8_t buffer[4];
}UNION_VAL;

void data_test(void)
{
    UNION_VAL v1[4], v2[4];
    char buf[40];
    size_t size = 0;

    v1[0].f_val = 2.5;
    v1[1].f_val = 1.4;
    v1[2].f_val = 1.8;
    v1[3].f_val = 4.6;
    
    for (int i=0; i<4; i++)
    {
        memcpy(&buf[size], v1[i].buffer, 4);
        size += 4;
    }

    size = 0;
    for (int i=0; i<4; i++)
    {
        memcpy(v2[i].buffer, &buf[size], 4);
        size += 4;    
    }

    printf("v2:%f,%f\n", v2[0].f_val, v2[1].f_val);
}
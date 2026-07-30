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
#include <sys/types.h>
#include <linux/rtc.h>
#include <time.h>

#define MEM_B       0
#define MEM_KB      1
#define MEM_MB      2
#define MEM_GB      3
#define MEM_TB      4

typedef struct
{
    uint8_t unit;
    double value;
}MEM_INFO;

static int memory_manage(void);
static int kernal_info_get(void);
static int diskinfo_manage(void);
int main(int argc, char *argv[])
{
    memory_manage();

    kernal_info_get();

    diskinfo_manage();
    return 0;
}

void memory_unit(MEM_INFO *pInfo)
{
    while (pInfo->value > 1024)
    {
        pInfo->value /= 1024;
        pInfo->unit++;
    }
}

char *get_memory_info(uint8_t unit)
{
    switch (unit)
    {
        case MEM_B:
            return "B";
        break;
        case MEM_KB:
            return "KB";
        break;
        case MEM_MB:
            return "MB";
        break;
        case MEM_GB:
            return "GB";
        break;
        case MEM_TB:
            return "TB";
        break;
    }

    return "NULL";
}

static int diskinfo_manage(void)
{
    struct statfs diskInfo;
    MEM_INFO total, available;
    if (statfs("/", &diskInfo) < 0)
    {
        perror("statfs");
        return -1;
    }

    total.value = diskInfo.f_blocks*diskInfo.f_bsize;
    total.unit = MEM_B;
    memory_unit(&total);
    available.value = diskInfo.f_bavail*diskInfo.f_bsize;
    available.unit = MEM_B;
    memory_unit(&available);

    printf("disk total:%lf%s, available:%lf%s\n", total.value, get_memory_info(total.unit),
        available.value, get_memory_info(available.unit));

    return 0;
}

static int memory_manage(void)
{
    struct sysinfo info;

    if (sysinfo(&info) == -1)
    {
        perror("sysinfo");
        return -1;
    }

    MEM_INFO total, free, used;

    total.value = info.totalram;
    total.unit = MEM_B;
    memory_unit(&total);
    free.value = info.freeram;
    free.unit = MEM_B;
    memory_unit(&free);
    used.value = info.totalram - info.freeram;
    used.unit = MEM_B;
    memory_unit(&used);

    printf("memory total: %lf%s, free: %lf%s, used: %lf%s\n", total.value, get_memory_info(total.unit),
        free.value, get_memory_info(free.unit),
        used.value, get_memory_info(used.unit));

    return 0;
}

static int kernal_info_get(void)
{
    struct utsname uname_info;
    if (uname(&uname_info) == -1)
    {
        perror("uname");
        return -1; 
    }
    printf("Kernel version: %s\n", uname_info.release);
    return 0;
}

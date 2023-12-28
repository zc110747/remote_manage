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
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>  
#include <ifaddrs.h>  
#include <sys/sysinfo.h>
#include <sys/statfs.h>  
#include <unistd.h>  
#include <netinet/in.h>  
#include <string.h>  
#include <arpa/inet.h>
#include <netdb.h>

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

static int diskinfo_manage(void);
static int memory_manage(void);
static int ip_list(void);

int main() 
{  
    diskinfo_manage();
    
    memory_manage();

    ip_list();
    
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

    printf("total disk:%lf%s\n", total.value, get_memory_info(total.unit));
    printf("available disk:%lf%s\n", available.value, get_memory_info(available.unit));   
}

static int memory_manage(void)
{
    struct sysinfo info;  
    if (sysinfo(&info) == -1) {  
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
  
    printf("toatl memory: %lf%s\n", total.value, get_memory_info(total.unit));   
    printf("free memory: %lf%s\n", free.value, get_memory_info(free.unit));               
    printf("used memory: %lf%s\n", used.value, get_memory_info(used.unit));
}

static int ip_list(void)
{
    struct ifaddrs *ifaddr, *ifa;  
    char host[NI_MAXHOST];  
  
    if (getifaddrs(&ifaddr) == -1) {  
        perror("getifaddrs");  
        exit(EXIT_FAILURE);  
    }  
  
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {  
        if (ifa->ifa_addr == NULL) {  
            continue;  
        }  
        int family = ifa->ifa_addr->sa_family;  
        if (family == AF_INET) { // IPv4 or IPv6  
            if (getnameinfo(ifa->ifa_addr, family==AF_INET?sizeof(struct sockaddr_in):sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {  
                printf("%s IP地址:%s\n", ifa->ifa_name, host);  
            }  
        }  
    }  
    freeifaddrs(ifaddr);
}
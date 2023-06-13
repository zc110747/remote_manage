/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "disk_interface.h"
#include "diskio.h"
#include "driver.hpp"
#include <string.h>

//RAM disk
int RAM_disk_status(void)
{
    //for ram, status already ok.
    return 0;
}

int RAM_disk_initialize(void)
{
    //for ram, not need initialize
    return 0;
}

int RAM_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    uint32_t address = RAM_START_ADDRESS + sector*RAM_SECTOR_SIZE;
    uint32_t size = count * RAM_SECTOR_SIZE;
    
    memcpy(buff, (uint8_t *)address, size);
    return 0;
}

int RAM_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    uint32_t address = RAM_START_ADDRESS + sector*RAM_SECTOR_SIZE;
    uint32_t size = count * RAM_SECTOR_SIZE;
    
    memcpy((uint8_t *)address, buff, size);
    return 0;
}

int RAM_disk_ioctl(BYTE cmd, void *buff)
{
    switch(cmd)
    {
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = RAM_SECTOR_SIZE;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = RAM_SECTOR_COUNT;
            break;
        case CTRL_SYNC:
            break;
    }
    return 0;
}

//MMC disk
int MMC_disk_status(void)
{
    return 0;
}

int MMC_disk_initialize(void)
{
    //
    return 0;
}

int MMC_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    if(read_disk(buff, sector, count) == HAL_OK)
    {
        return 0;
    }
    return -1;
}

int MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    if(write_disk(buff, sector, count) == HAL_OK)
    {
        return 0;
    }
    return -1;
}

int MMC_disk_ioctl(BYTE cmd, void *buff)
{
    switch(cmd)
    {
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = MMC_SECTOR_SIZE;
            break;
        case GET_SECTOR_SIZE:
            *(DWORD *)buff = MMC_SECTOR_SIZE;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = MMC_SECTOR_COUNT;
            break;
        case CTRL_SYNC:
            break;
    }
    return 0;
}

//USB disk
int USB_disk_status(void)
{
    return 0;
}

int USB_disk_initialize(void)
{
    return 0;
}

int USB_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    return 0;
}

int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    return 0;
}

DWORD get_fattime(void)
{
    return 0;
}
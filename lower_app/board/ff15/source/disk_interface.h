
#ifndef _DISK_INTERFACE_H
#define _DISK_INTERFACE_H

#include "ff.h"

#define RAM_START_ADDRESS   0xC0000000
#define RAM_SECTOR_SIZE     512
#define RAM_SECTOR_COUNT    4096

int RAM_disk_status(void);
int RAM_disk_initialize(void);
int RAM_disk_read(BYTE *buff, LBA_t sector, UINT count);
int RAM_disk_write(const BYTE *buff, LBA_t sector, UINT count);
int RAM_disk_ioctl(BYTE cmd, void *buff);
 
#define MMC_SECTOR_SIZE     512
#define MMC_SECTOR_COUNT    62500000

int MMC_disk_status(void);
int MMC_disk_initialize(void);
int MMC_disk_read(BYTE *buff, LBA_t sector, UINT count);
int MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count);
int MMC_disk_ioctl(BYTE cmd, void *buff);

int USB_disk_status(void);
int USB_disk_initialize(void);
int USB_disk_read(BYTE *buff, LBA_t sector, UINT count);
int USB_disk_write(const BYTE *buff, LBA_t sector, UINT count);

DWORD get_fattime(void);
#endif

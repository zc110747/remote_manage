/*
 * File      :
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 * 2020-5-20     zc           Code standardization
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/
#define UART_BUFFER_SIZE 1024

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static uint8_t nCacheBuffer[UART_BUFFER_SIZE];
static uint8_t nComFd;
struct serial_opt
{
    int baud;
    int databits;
    int stopbits;
    const char* parity;
};

struct serial_opt opt = {115200, 8, 1, "n"};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/
#define RS232_DRIVER_NAME   "ttymxc2"

/**************************************************************************
* Local Function Declaration
***************************************************************************/
static int set_opt(int, struct serial_opt*);

/**************************************************************************
* Function
***************************************************************************/
int main(int argc, char *argv[])
{
    int fd;
    ssize_t nLen;

    /*开启串口模块*/
    fd = open(RS232_DRIVER_NAME, O_RDWR|O_NOCTTY|O_NDELAY);
    if (fd < 0)
    {
        printf("%s Device Open Failed\n");
        return -1;
    }
    else
    {
        if (set_opt(fd, &opt) != 0)
        {
            do
            {
                /*从串口缓冲区中读出数据，并写入到发送缓冲区*/
                nLen = read(fd, nCacheBuffer, UART_BUFFER_SIZE);
                if (nLen > 0)
                {
                    write(fd, nCacheBuffer, nLen);
                }
            } while (1);
        }
    }
    close(fd);
    return 0;
}

/**
 * 配置Uart的通讯参数
 * 
 * @param nFd  设备描述符
 * @param opt_info串口的配置信息
 *  
 * @return NULL
 */
static int set_opt(int nFd,  struct serial_opt *opt_info)
{
    struct termios newtio;
    struct termios oldtio;

    if (tcgetattr(nFd, &oldtio)  !=  0)
    {
        printf("Get Serial Attribute Failed\n");
        return -1;
    }
    memset((char *)&newtio, 0, sizeof(newtio));
    newtio.c_cflag |= (CLOCAL|CREAD);
    newtio.c_cflag &= ~CSIZE;

    switch (opt_info->databits)
    {
        case 7:
            newtio.c_cflag |= CS7;
        break;
        case 8:
            newtio.c_cflag |= CS8;
        break;
        default:
        break;
    }

    switch (opt_info->parity[0])
    {
        case 'O':
        case 'o':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
        break;
        case 'E':
        case 'e':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
        break;
        case 'N':
        case 'n':
            newtio.c_cflag &= ~PARENB;
        break;
    }

    switch (opt_info->baud)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
        break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
        break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
        break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
        break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
        break;
        case 921600:
            cfsetispeed(&newtio, B921600);
            cfsetospeed(&newtio, B921600);
        break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
        break;
    }

    /*设置停止位*/
    if (opt_info->stopbits == 1)
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if (opt_info->stopbits == 2)
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;

    /*将配置信息实际设置到串口中*/
    tcflush(nFd, TCIFLUSH);
    if ((tcsetattr(nFd, TCSANOW, &newtio))!=0)
    {
        printf("Serial Config Error\n");
        return -1;
    }

    printf("Serial Config Success!\n\r");
    return 0;
}

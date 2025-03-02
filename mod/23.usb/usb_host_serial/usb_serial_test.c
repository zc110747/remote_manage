/*
 * File      : usb_serial_test.c
 * test for usb host serial driver.
 * COPYRIGHT (C) 2024, zc
 */
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
static uint8_t buffer[UART_BUFFER_SIZE];
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
#define RS232_DRIVER_NAME   "/dev/ttyUSB0"

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
    ssize_t length;

    fd = open(RS232_DRIVER_NAME, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("%s Device Open Failed\n");
        return -1;
    } else {
        length = strlen("hello world!");
        memcpy(buffer, "hello world!", length);
        buffer[length] = '\0';

        if (set_opt(fd, &opt) == 0) {
            printf("[usb serial] write length:%d!\n", length);
            write(fd, buffer, length);
            tcdrain(fd);
            length = read(fd, buffer, UART_BUFFER_SIZE);
            if (length > 0) {
                printf("[usb serial] rx:%d, data:%s\n", length, buffer);
            } else {
                printf("rx length:%d!\n", length);
            }
        }
    }

    printf("[usb serial] end\n");
    close(fd);
    return 0;
}

// 配置串口操作
static int set_opt(int nFd,  struct serial_opt *opt_info)
{
    struct termios term;

    if (tcgetattr(nFd, &term)  !=  0) {
        printf("Get Serial Attribute Failed\n");
        return -1;
    }

    term.c_cflag |= CLOCAL | CREAD;
    term.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    term.c_oflag &= ~OPOST;
    term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    
    switch (opt_info->databits) {
        case 7:
            term.c_cflag |= CS7;
        break;
        case 8:
            term.c_cflag |= CS8;
        break;
        default:
        break;
    }

    switch (opt_info->parity[0]) {
        case 'O':
        case 'o':
            term.c_cflag |= PARENB;
            term.c_cflag |= PARODD;
            term.c_iflag |= (INPCK | ISTRIP);
        break;
        case 'E':
        case 'e':
            term.c_iflag |= (INPCK | ISTRIP);
            term.c_cflag |= PARENB;
            term.c_cflag &= ~PARODD;
        break;
        case 'N':
        case 'n':
            term.c_cflag &= ~PARENB;
        break;
    }

    // 设置波特率
    switch (opt_info->baud) {
        case 2400:
            cfsetispeed(&term, B2400);
            cfsetospeed(&term, B2400);
        break;
        case 4800:
            cfsetispeed(&term, B4800);
            cfsetospeed(&term, B4800);
        break;
        case 9600:
            cfsetispeed(&term, B9600);
            cfsetospeed(&term, B9600);
        break;
        case 115200:
            cfsetispeed(&term, B115200);
            cfsetospeed(&term, B115200);
        break;
        default:
            cfsetispeed(&term, B9600);
            cfsetospeed(&term, B9600);
        break;
    }

    // 设置停止位
    if (opt_info->stopbits == 1) {
        term.c_cflag &=  ~CSTOPB;
    } else if (opt_info->stopbits == 2) {
        term.c_cflag |=  CSTOPB;
    }

    term.c_cc[VTIME] = 1;     
    term.c_cc[VMIN] = 1;

    // 配置写入到串口中
    tcflush(nFd, TCIFLUSH);
    if ((tcsetattr(nFd, TCSANOW, &term))!=0) {
        printf("Serial Config Error\n");
        return -1;
    }

    return 0;
}

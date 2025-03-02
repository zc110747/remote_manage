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
#include <sys/select.h>

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
static int set_nonblock(int fd);

/**************************************************************************
* Global Variable Declaration
***************************************************************************/
#define RS232_DRIVER_NAME   "/dev/ttymxc2"

/**************************************************************************
* Local Function Declaration
***************************************************************************/
static int set_opt(int, struct serial_opt*);
static void signal_read_write(int fd);
static int select_read_write(int fd);

/**************************************************************************
* Function
***************************************************************************/
int main(int argc, char *argv[])
{
    int fd;

    fd = open(RS232_DRIVER_NAME, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("%s Device Open Failed\n", RS232_DRIVER_NAME);
        return -1;
    } else {
        if (set_opt(fd, &opt) == 0) {
            select_read_write(fd);
        }
    }

    printf("[rs232_test] end!\n\r");
    close(fd);
    return 0;
}

static int select_read_write(int fd)
{
    fd_set readfds;
    ssize_t length;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    int ret = select(fd + 1, &readfds, NULL, NULL, NULL);
    if (ret < 0) {
        printf("%s", strerror(errno));
        return -1;
    }

    length = read(fd, buffer, UART_BUFFER_SIZE);
    if (length > 0) {
        printf("[rs232_test] rx:%d, data:%s\n", length, buffer);
        write(fd, buffer, length);
        tcdrain(fd);
    } else {
        printf("rs232 receive:%d\n", length);
        return -1;
    }
    return 0;
}

static void signal_read_write(int fd)
{
    ssize_t length;

    length = read(fd, buffer, UART_BUFFER_SIZE);
    if (length > 0) {
        printf("[rs232_test] rx:%d, data:%s\n", length, buffer);
        write(fd, buffer, length);
        tcdrain(fd);
    } else {
        printf("rs232 receive:%d\n", length);
    }
}

static int set_opt(int fd,  struct serial_opt *opt_info)
{
    struct termios term;

    if (tcgetattr(fd, &term) !=  0)
    {
        printf("Get Serial Attribute Failed\n");
        return -1;
    }

    //Linux串口接收二进制字符0x11，0x13, 0x0d等特殊字符串丢弃和转换(如下设置避免处理)
    term.c_cflag |= CLOCAL | CREAD;
    term.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    term.c_oflag &= ~OPOST;
    term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    
    switch (opt_info->databits)
    {
        case 7:
            term.c_cflag |= CS7;
        break;
        case 8:
            term.c_cflag |= CS8;
        break;
        default:
        break;
    }

    switch (opt_info->parity[0])
    {
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

    switch (opt_info->baud)
    {
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
        case 460800:
            cfsetispeed(&term, B460800);
            cfsetospeed(&term, B460800);
        break;
        case 921600:
            cfsetispeed(&term, B921600);
            cfsetospeed(&term, B921600);
        break;
        default:
            cfsetispeed(&term, B9600);
            cfsetospeed(&term, B9600);
        break;
    }

    /*设置停止位*/
    if (opt_info->stopbits == 1)
    {
        term.c_cflag &=  ~CSTOPB;
    }
    else if (opt_info->stopbits == 2)
    {
        term.c_cflag |=  CSTOPB;
    }

    //VTIME定义读取操作的超时时间(单位0.1s)
    //大于0时，超时未获取VMIN指定值则返回已经读取的数据
    //0值时，如果VMIN为0，read函数立即返回
    //0值时，如果VMIN大于0，至少接受cc[VMIN]长度值才返回
    term.c_cc[VTIME] = 10;     
    term.c_cc[VMIN] = 1;

    // 配置写入到串口节点中
    tcflush(fd, TCIFLUSH);
    if ((tcsetattr(fd, TCSANOW, &term))!=0)
    {
        printf("Serial Config Error\n");
        return -1;
    }
    return 0;
}

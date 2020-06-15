#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <QDebug>
#include <QString>
#include <QSemaphore>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define UDP_DEFAULT_PORT        8100

#define RT_PROTOCOL_OK          0
#define RT_PROTOCOL_TIMOUT      1
#define RT_PROTOCOL_IDERR       2
#define RT_PROTOCOL_PACKETERR   3
#define RT_PROTOCOL_CHECKERR    4

//协议相关的指令
#define PROTOCOL_SEND_HEAD          0x5A
#define PROTOCOL_RECV_HEAD          0x5B
#define PROTOCOL_RECV_HEAD_SIZE     3
#define PROTOCOL_CRC_SIZE           2
#define PROTOCOL_TIMEOUT            3000

//缓存的大小
#define BUFF_CACHE_SIZE             1200

//接收数据的头部长度
#define RECV_DATA_HEAD          7

#define RT_OK               0
#define RT_EMPTY            -1
#define RT_TIMEOUT          -2
#define RT_CRC_ERROR        -3
#define RT_FAIL             -4

//队列相关的信息
#define MAX_QUEUE            20
#define QUEUE_INFO_OK        0
#define QUEUE_INFO_FULL     -1
#define QUEUE_INFO_INVALID  -2
#define QUEUE_INFO_EMPTY    -3

//文件大小
#define FILE_BLOCK_SIZE     1000

#define TEST_DEBUG          0

#define DEFAULT_CONFIG_FILE "config.json"
//当前的协议类型
enum PROTOCOL_STATUS
{
    PROTOCOL_NULL = 0,
    PROTOCOL_UART,
    PROTOCOL_TCP,
    PROTOCOL_UDP
};

#define  SYSTEM_VERSION         "v0.04"
#endif // TYPEDEF_H

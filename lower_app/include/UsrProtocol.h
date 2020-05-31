/*
 * File      : protocol.h
 * protocol layer interface
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
#ifndef _INCLUDE_PROTOCOL_H
#define _INCLUDE_PROTOCOL_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "UsrTypeDef.h"
#include "ApplicationThread.h"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
/*协议数据长度*/
#define FRAME_HEAD_SIZE			6   //协议头数据的宽度
#define CRC_SIZE				2   //CRC数据的长度 

/*协议数据格式*/
#define PROTOCOL_REQ_HEAD  		0x5A	/*协议数据头*/
#define PROTOCOL_DEVICE_ID      0x01	/*设备ID*/
#define PROTOCOL_ACK_HEAD		0x5B	/*应答数据头*/

/*设备操作指令*/
#define CMD_REG_READ 			0x01    /*读寄存器*/
#define CMD_REG_WRITE			0x02	/*写寄存器*/
#define CMD_UPLOAD_CMD			0x03	/*上传指令*/
#define CMD_UPLOAD_DATA			0x04	/*上传数据*/

/*设备应答指令*/
#define ACK_OK					0x00
#define ACK_INVALID_CMD			0x01
#define ACK_OTHER_ERR			0xff

#define BIG_ENDING         		0
#if BIG_ENDING	
#define LENGTH_CONVERT(val)	(val)
#else
#define LENGTH_CONVERT(val) (((val)<<8) |((val)>>8))
#endif

/**************************************************************************
* Global Type Definition
***************************************************************************/
/*协议文件头格式*/
#pragma pack(push, 1)
struct req_frame
{
	uint8_t head;
	uint8_t id;
	uint16_t packet_id;
	uint16_t length;
};
#pragma pack(pop)

class CProtocolInfo
{
public:  
    CProtocolInfo(uint8_t *pRxCachebuf, uint8_t *pTxCacheBuf, uint8_t *pRxData, uint16_t nMaxSize);
        ~CProtocolInfo(void){};

    int ExecuteCommand(int nFd);                                			//数据处理和执行
    int CheckRxBuffer(int nFd);                             				//接收数据分析
	int SendTxBuffer(int nFd);												//发送数据
    int CreateTxBuffer(uint8_t nAck, uint16_t nDataSize, uint8_t *pData);	//创建发送数据包
	uint16_t CrcCalculate(uint8_t *pDataStart, uint16_t nDataSize);					//数组生成CRC校验数据


	/*设备读写函数，因为不同设备的实现可能不同，用纯虚函数*/
	virtual int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize) = 0;   
	virtual int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize) = 0;
private:
    uint8_t *m_RxCachePtr;       	//接收数据首指针
	uint8_t *m_TxCachePtr;	   		//发送数据首指针
	uint8_t *m_RxCacheDataPtr;  	//接收数据数据段首指针
	uint16_t m_RxBufSize;	   		//接收数据长度
	uint16_t m_TxBufSize;      		//发送数据长度
	uint16_t m_RxDataSize; 			//接收数据数据段长度
    uint16_t m_MaxCacheBufSize;  	//最大的数据长度
	uint16_t m_PacketNum;	  		//数据包的编号,用于数据校验同步
	uint32_t m_RxTimeout; 			//超时时间
};
/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
#endif

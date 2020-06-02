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


template<class T>
class CProtocolInfo
{
public:  
	/**
	 * 内部数据构造函数
	 * 
	 * @param pRxCachebuf  接收数据缓存首地址指针
	 * @param pTxCacheBuf  发送数据缓存首地址指针
	 * @param pRxData      接收到有效数据的首地址
	 * @param nMaxSize     最大缓存数据长度
	 *  
	 * @return NULL
	 */
	CProtocolInfo(uint8_t *pRxCachebuf, uint8_t *pTxCacheBuf, uint8_t *pRxData, uint16_t nMaxSize){
		m_RxCachePtr = pRxCachebuf;
		m_TxCachePtr = pTxCacheBuf;
		m_RxCacheDataPtr = pRxData;
		m_RxBufSize = 0;
		m_TxBufSize = 0;
		m_PacketNum = 0;
		m_RxDataSize = 0;
		m_MaxCacheBufSize = nMaxSize;
		m_PacketNum = 0;
		m_RxTimeout = 0;
	};
		~CProtocolInfo(void){};

	/**
	 * 执行具体的指令
	 * 
	 * @param fd 执行的设备ID号
	 *  
	 * @return 执行执行的结果
	 */
	int ExecuteCommand(int nFd)                               	
	{
		uint8_t nCommand;
		uint16_t nRegIndex, nRxDataSize;
		uint8_t *pCacheDataBuf;
		CApplicationReg  *pApplicationReg;

		nCommand = m_RxCacheDataPtr[0];
		nRegIndex = m_RxCacheDataPtr[1]<<8 | m_RxCacheDataPtr[2];
		nRxDataSize = m_RxCacheDataPtr[3]<<8 | m_RxCacheDataPtr[4];
		pCacheDataBuf = (uint8_t *)malloc(m_MaxCacheBufSize);
		m_TxBufSize = 0;
		pApplicationReg = GetApplicationReg();
		
		switch (nCommand)
		{
			case CMD_REG_READ:
				pApplicationReg->GetMultipleReg(nRegIndex, nRxDataSize, pCacheDataBuf);
				m_TxBufSize = CreateTxBuffer(ACK_OK, nRxDataSize, pCacheDataBuf);
				break;
			case CMD_REG_WRITE:	
				memcpy(pCacheDataBuf, &m_RxCacheDataPtr[5], nRxDataSize);
				pApplicationReg->SetMultipleReg(nRegIndex, nRxDataSize, pCacheDataBuf);	
				m_TxBufSize = CreateTxBuffer(ACK_OK, 0, NULL);
				break;
			case CMD_UPLOAD_CMD:		
				break;
			case CMD_UPLOAD_DATA:
				break;
			default:
				break;
		}
		free(pCacheDataBuf);    

		/*发送数据，并清空接收数据*/
		m_RxBufSize = 0;
		return RT_OK;
	}

	/**
	 * 接收数据以及校验
	 * 
	 * @param nFd 访问设备的ID
	 *  
	 * @return 接收数据和校验的结果
	 */
	int CheckRxBuffer(int nFd, T ExtraInfo){
		int nread;
		int CrcRecv, CrcCacl;
		struct req_frame *frame_ptr; 
		frame_ptr = (struct req_frame *)m_RxCachePtr;

		//数据包头的处理
		if(m_RxBufSize == 0)
		{
			nread = DeviceRead(nFd, &m_RxCachePtr[m_RxBufSize], 1, ExtraInfo);
			if(nread != 0 &&frame_ptr->head == PROTOCOL_REQ_HEAD)
			{
				m_RxBufSize++;
				m_RxTimeout = 0;
			}
			else
			{
				usleep(1);
				m_RxBufSize = 0;
				return RT_FAIL;
			}
		}

		//数据包的解析
		if(m_RxBufSize > 0)
		{
			/*从设备中读取数据*/
			nread = DeviceRead(nFd, &m_RxCachePtr[m_RxBufSize], (m_MaxCacheBufSize-m_RxBufSize), ExtraInfo);
			if(nread > 0)
			{        
				m_RxTimeout = 0;
				m_RxBufSize += nread;

				/*已经接收到长度数据*/
				if(m_RxBufSize > 5)
				{
					int nLen;

					/*设备ID检测*/
					if(frame_ptr->id != PROTOCOL_DEVICE_ID)
					{
						m_RxBufSize = 0;
						USR_DEBUG("Devoce ID Invalid\n");
						return RT_INVALID;
					}

					/*获取接收数据的总长度*/
					m_RxDataSize = LENGTH_CONVERT(frame_ptr->length);

					/*crc冗余校验*/
					nLen = m_RxDataSize+FRAME_HEAD_SIZE+CRC_SIZE;
					if(m_RxBufSize >= nLen)
					{
						/*计算head后到CRC尾之前的所有数据的CRC值*/
						CrcRecv = (m_RxCachePtr[nLen-2]<<8) + m_RxCachePtr[nLen-1];
						CrcCacl = CrcCalculate(&m_RxCachePtr[1], nLen-CRC_SIZE-1);
						if(CrcRecv == CrcCacl){
							m_PacketNum = LENGTH_CONVERT(frame_ptr->packet_id);
							return RT_OK;
						}
						else{
							m_RxBufSize = 0;
							USR_DEBUG("CRC Check ERROR!. rx_data:%d, r:%d, c:%d\n", m_RxDataSize, CrcRecv, CrcCacl);
							return RT_INVALID;
						}
					}  
				}           
			}
			else
			{
				usleep(1);
				m_RxTimeout++;
				if(m_RxTimeout > 4000)
				{
					m_RxBufSize = 0;
					m_RxTimeout = 0;
					USR_DEBUG("Recv RxTimeout\n");
					return RT_TIMEOUT;
				}
			}		
		}
		return RT_EMPTY;
	};                             				//接收数据分析
	
	/**
	 * 提交数据到上位机
	 * 
	 * @param fd 执行的设备ID号
	 *  
	 * @return 执行执行的结果
	 */
	int SendTxBuffer(int nFd, T ExtraInfo)
	{
		SystemLogArray(m_TxCachePtr, m_TxBufSize);
		DeviceWrite(nFd, m_TxCachePtr, m_TxBufSize, ExtraInfo);
	}
	
	/**
	 * 生成发送的数据包格式
	 * 
	 * @param nAck      应答数据的状态
	 * @param nDataSize 应答有效数据的长度
	 * @param pData     应答有效数据的首指针
	 *  
	 * @return 执行执行的结果
	 */
	int CreateTxBuffer(uint8_t nAck, uint16_t nDataSize, uint8_t *pData)
	{
		uint8_t nOutSize, nIndex;
		uint16_t nCrcCalc;

		nOutSize = 0;
		m_TxCachePtr[nOutSize++] = PROTOCOL_ACK_HEAD;
		m_TxCachePtr[nOutSize++] = PROTOCOL_DEVICE_ID;
		m_TxCachePtr[nOutSize++] = (uint8_t)(m_PacketNum>>8);
		m_TxCachePtr[nOutSize++] = (uint8_t)(m_PacketNum&0xff);
		m_TxCachePtr[nOutSize++] = nAck;
		m_TxCachePtr[nOutSize++] = (uint8_t)(nDataSize>>8);
		m_TxCachePtr[nOutSize++] = (uint8_t)(nDataSize&0xff);	

		if(nDataSize != 0 && pData != NULL)
		{
			for(nIndex=0; nIndex<nDataSize; nIndex++)
			{
				m_TxCachePtr[nOutSize++] = *(pData+nIndex);
			}
		}

		nCrcCalc = CrcCalculate(&m_TxCachePtr[1], nOutSize-1);
		m_TxCachePtr[nOutSize++] = (uint8_t)(nCrcCalc>>8);
		m_TxCachePtr[nOutSize++] = (uint8_t)(nCrcCalc&0xff);	

		return nOutSize;
	}

	/**
	 * CRC16计算实现
	 * 
	 * @param pDataStart 计算CRC数据的首地址
	 * @param nDataSize  计算CRC数据的长度
	 *  
	 * @return NULL
	 */
	uint16_t CrcCalculate(uint8_t *pDataStart, uint16_t nDataSize)
	{
		return 0xFFFF;
	}			

	/*设备读写函数，因为不同设备的实现可能不同，用纯虚函数*/
	virtual int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)=0;  
	virtual int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)=0;

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

//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      UartThrad.cpp
//
//  Purpose:
//      Uart Thread process workflow.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "UartThread.hpp"

static void *UartLoopThread(void *arg)
{
	int nFlag;
	int size;
	auto pThreadInfo = static_cast<UartThreadManage *>(arg);
	auto *pProtocolInfo = pThreadInfo->getProtocolInfo();

	PRINT_LOG(LOG_INFO, xGetCurrentTime(), "UART Thread start!");
	for(;;)
	{	   
		nFlag = pProtocolInfo->CheckRxBuffer(pThreadInfo->getComfd(), false);
		if(nFlag == RT_OK)
		{
			pProtocolInfo->ExecuteCommand(pThreadInfo->getComfd());
			pProtocolInfo->SendTxBuffer(pThreadInfo->getComfd());
		}
		else
		{
			usleep(50); //通讯结束让出线程
		}
	}

	pthread_detach(pthread_self()); 
    pthread_exit((void *)0);
}

UartThreadManage* UartThreadManage::pInstance = nullptr;
UartThreadManage* UartThreadManage::getInstance()
{
	if(pInstance == nullptr)
	{
		pInstance = new(std::nothrow) UartThreadManage();
		if(pInstance == nullptr)
		{
			PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "UartThreadManage new failed!");
		}
	}
	return pInstance;
}

bool UartThreadManage::init()
{
#if UART_MODULE_ON == 1
	const SerialSysConfig* pSerialConfig = SystemConfig::getInstance()->getserial();
	if((nComFd = open(pSerialConfig->dev.c_str(), O_RDWR|O_NOCTTY|O_NDELAY))<0)
	{	
		PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Open Device %s failed!", pSerialConfig->dev.c_str());
		return false;
	}
	else
	{
		if(set_opt(pSerialConfig->baud, pSerialConfig->dataBits, pSerialConfig->parity, pSerialConfig->stopBits) != 0)
		{
			PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Serial Option failed!");
			return false;
		}
	}

	//创建UART协议管理对象
	pProtocolInfo = new CUartProtocolInfo(nRxCacheBuffer, nTxCacheBuffer, UART_MAX_BUFFER_SIZE);
	if(pthread_create(&tid, NULL, UartLoopThread, this) != 0)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Uart Thread Create failed!");
		return false;
	}
#endif
	return true;
}

int UartThreadManage::set_opt(int nBaud, int nDataBits, std::string cParity, int nStopBits)
{
	struct termios newtio;
	struct termios oldtio;

	if(tcgetattr(nComFd, &oldtio)  !=  0) 
	{ 
		PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Get serial attribute failed!");
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag |= (CLOCAL|CREAD);
	newtio.c_cflag &= ~CSIZE;

	switch(nDataBits)
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

	switch(cParity[0])
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

	switch(nBaud)
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
	
	if(nStopBits == 1)
	{
		newtio.c_cflag &=  ~CSTOPB;
	}
	else if (nStopBits == 2)
	{
		newtio.c_cflag |=  CSTOPB;
	}
	newtio.c_cc[VTIME]  = 0;
	newtio.c_cc[VMIN] = 0;

	tcflush(nComFd, TCIFLUSH);
	if((tcsetattr(nComFd, TCSANOW, &newtio))!=0)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Serial Config Error!");
		return -1;
	}

	PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Serial Config Done Success!");
	return 0;
}

void UartThreadManage::release()
{
	if(pProtocolInfo != nullptr)
	{
		delete pProtocolInfo;
		pProtocolInfo = nullptr;
	}

	if(nComFd >= 0)
	{
		close(nComFd);
		nComFd = -1;
	}
}

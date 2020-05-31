/*
 * File      : uart_task.cpp
 * uart hardware process task
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

#include "../include/UartThread.h"
#include "../include/SystemConfig.h"

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static CUartProtocolInfo *pUartProtocolInfo; //uart模块管理信息指针

static uint8_t 	nRxCacheBuffer[UART_BUFFER_SIZE];
static uint8_t  nTxCacheBuffer[UART_BUFFER_SIZE];
static uint8_t 	nComFd;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function
***************************************************************************/
static void *UartLoopThread(void *arg);
static int set_opt(int, int, int, std::string, int);

/**************************************************************************
* Function
***************************************************************************/
/**
 * uart模块任务初始化
 * 
 * @param NULL
 *  
 * @return NULL
 */
void UartThreadInit(void)
{
	int nErr;
	pthread_t tid1;
	struct SSystemConfig *pSystemConfigInfo;
	
	pSystemConfigInfo = GetSSytemConfigInfo();

	if((nComFd = open(TTY_DEVICE, O_RDWR|O_NOCTTY|O_NDELAY))<0)
	{	
		USR_DEBUG("Open %s failed\n", TTY_DEVICE);
		return;
	}
	else
	{
		set_opt(nComFd, pSystemConfigInfo->m_baud, pSystemConfigInfo->m_data_bits, pSystemConfigInfo->m_parity, pSystemConfigInfo->m_stop_bits);
		USR_DEBUG("Open %s Success!\t\n", TTY_DEVICE);
	}

	//创建UART协议管理对象
	pUartProtocolInfo = new CUartProtocolInfo(nRxCacheBuffer, nTxCacheBuffer, 
						&nRxCacheBuffer[FRAME_HEAD_SIZE], UART_BUFFER_SIZE);

	nErr = pthread_create(&tid1, NULL, UartLoopThread, NULL);
	if(nErr != 0)
	{
		USR_DEBUG("Uart Task Thread Create Err=%d\n", nErr);
	}
}

/**
 * uart主任务执行流程
 * 
 * @param arg 线程传递的参数
 *  
 * @return NULL
 */
static void *UartLoopThread(void *arg)
{
	int nFlag;

	USR_DEBUG("Uart Main Task Start\n");
	write(nComFd, "Uart Start OK!\n", strlen("Uart Start OK!\n"));

	for(;;)
	{	   
		nFlag = pUartProtocolInfo->CheckRxBuffer(nComFd);
		if(nFlag == RT_OK)
		{
			pUartProtocolInfo->ExecuteCommand(nComFd);
			pUartProtocolInfo->SendTxBuffer(nComFd);
		}
		else
		{
			usleep(50); //通讯结束让出线程
		}
	}
	return (void *)0;
}

/**
 * 配置Uart硬件的功能
 * 
 * @param fd 	 设置的串口设备ID
 * @param nBaud 波特率
 * @param nDataBits  数据位
 * @param cParity 奇偶校验位
 * @param nStopBits  停止位
 *  
 * @return NULL
 */
static int set_opt(int nFd, int nBaud, int nDataBits, std::string cParity, int nStopBits)
{
	struct termios newtio;
	struct termios oldtio;

	if(tcgetattr(nFd, &oldtio)  !=  0) 
	{ 
		USR_DEBUG("SetupSerial 1");
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

	tcflush(nFd, TCIFLUSH);
	if((tcsetattr(nFd, TCSANOW,&newtio))!=0)
	{
		USR_DEBUG("serial config error\n");
		return -1;
	}

	USR_DEBUG("serial config done!\n\r");
	return 0;
}


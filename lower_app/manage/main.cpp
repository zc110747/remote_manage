/*
 * File      : main.cpp
 * main frame process
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
#include "source/UnityMain.hpp"
#include "driver/driver.hpp"

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static int pipe_fd[2];

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/
#if __SYSTEM_DEBUG == 1
static void SystemTest(void);
#endif

/**************************************************************************
* Function
***************************************************************************/
/**
 * 进程执行入口函数
 * 
 * @param argc 输入命令行的参数数目
 * @param argv 输入命令行的参数指针数组
 *  
 * @return 进程执行的返回状态(非异常状态下不返回)
 */
int main(int argc, char* argv[])
{
	int c;
    int result = 0;
	int nConfigDefault = 0;
	std::string sConfigFile(DEFAULT_CONFIG_FILE);

	//命令行输入说明
	while ((c = getopt(argc, argv, "v:d:f:h::")) != -1)
	{
		switch (c)
		{
			case 'd':
				nConfigDefault = 1;
				break;
			case 'f':
				if(optarg != nullptr)
				{
					sConfigFile = std::string(optarg);
				}
				break;
			case '?':
			case 'h':
				printf("Usage: app [options] [file]\n");
				printf("-v       显示版本信息\n");
				printf("-d       使用默认配置\n");
				printf("-h       显示帮组选项\n");
				printf("-f       指定选择的配置文件\n");
				exit(0);
				break;
			case 'v':
				printf("%s\n", DEVICE_VERSION);
				exit(0);	
			default:
				exit(1);
				break;
		}
	}

	//守护进程，用于进程后台执行
    result = daemon(1, 1);
	if(result < 0)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "daemon error!");
		return result;
	}
	
	//选中配置文件
	if(nConfigDefault == 0)
	{
		if(!SystemConfig::getInstance()->init(sConfigFile.c_str()))
		{
			PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Config Read Error!");
		}
		else
		{
			std::cout<<*(SystemConfig::getInstance())<<std::endl;
		}
	}
	else
	{
		PRINT_LOG(LOG_INFO, xGetCurrentTime(), "System Config use default!");
	}

	PRINT_LOG(LOG_INFO, xGetCurrentTime(), "System Start!");
#if __SYSTEM_DEBUG == 0
	/*硬件相关初始化*/
	hardware_driver_init();

	/*任务创建*/
	ApplicationThreadInit();
	UartThreadInit();
	SocketTcpThreadInit();
	SocketUdpThreadInit();

	for(;;)
	{
		static char MainMqFlag;
		int flag;
		CBaseMessageInfo *pBaseMessageInfo;
		#if __WORK_IN_WSL == 1
		pBaseMessageInfo = static_cast<CBaseMessageInfo *>(GetFifoMessageInfo());
		#else
		pBaseMessageInfo = static_cast<CBaseMessageInfo *>(GetMqMessageInfo());
		#endif
		flag = pBaseMessageInfo->WaitInformation(MAIN_BASE_MESSAGE, &MainMqFlag, sizeof(MainMqFlag));
		if(flag != -1)
		{
			pBaseMessageInfo->CloseInformation(MAIN_BASE_MESSAGE);
			break;
		}
		else
		{
			USR_DEBUG("Mq Wait Error,Error Code:%s\n", strerror(errno));
			sleep(100);
		}
	}
	hardware_driver_release();
#else
	SystemTest();
#endif	

	USR_DEBUG("Process app_demo stop, error:%s\n", strerror(errno));
	return result;
}

/**
 * 打印调试信息接口
 * 
 * @param pArrayBuffer 打印数组的首地址
 * @param nArraySize   打印数组的长度
 *  
 * @return NULL
 */
void SystemLogArray(uint8_t *pArrayBuffer, uint16_t nArraySize)
{
#if __DEBUG_PRINTF == 1
	uint16_t index;
	for(index=0; index<nArraySize; index++)
	{
		printf("0x%x ", (int)pArrayBuffer[index]);
	}
	printf("\n");
#endif
}

#if __SYSTEM_DEBUG == 1
#define TEST_BUFFER_SZIE	1200
static uint8_t 	rx_buffer[TEST_BUFFER_SZIE];
static uint8_t  tx_buffer[TEST_BUFFER_SZIE];
const uint8_t test_command[] = {
	0x5a, 0x00, 0x0B, 0x01, 0x32, 0x23, 0x02, 0x00, 
	0x00, 0x00, 0x03, 0x07, 0x00, 0x01, 0x54, 0x3f
};

template<class T>
class CTestProtocolInfo:public CProtocolInfo<T>
{
private: 
	uint16_t read_num{0};
	uint16_t total_num{sizeof(test_command)};

public:
	using CProtocolInfo<T>::CProtocolInfo;

	void clear(void){
		read_num = 0;
	}

	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)
	{
		uint16_t left_num, min_num;

		left_num = total_num - read_num;
		min_num = nDataSize>=left_num?left_num:nDataSize;
		memcpy(pDataStart, &test_command[read_num], min_num);
		read_num += min_num;

		*ExtraInfo = min_num;
		return *ExtraInfo;
	}
	
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)
	{
		printf("send array:");
		SystemLogArray(pDataStart, nDataSize);
		*ExtraInfo = nDataSize;
		return *ExtraInfo;
	}
};

/**
 * 模块的测试代码
 * 
 * @param NULL
 *  
 * @return NULL
 */
static void SystemTest(void)
{
	int nFd = 0, nFlag;
	int size;
	CApplicationReg *pApplicationReg;
	CTestProtocolInfo<int *> *pTesetProtocolInfo;

	ShowSSystemConfigInfo();
	pApplicationReg = new CApplicationReg();
	pTesetProtocolInfo = new CTestProtocolInfo<int *>(rx_buffer, tx_buffer, TEST_BUFFER_SZIE);
	/*更新设备处理指针*/
	SetApplicationReg(pApplicationReg);

	/*执行接收数据的处理指令*/
	nFlag = pTesetProtocolInfo->CheckRxBuffer(nFd, false, &size);
	if(nFlag == RT_OK){
		pTesetProtocolInfo->ExecuteCommand(nFd);
		pApplicationReg->RefreshAllDevice();
		pTesetProtocolInfo->SendTxBuffer(nFd, &size);
	}

	delete pApplicationReg;
	delete pTesetProtocolInfo;
	pApplicationReg = NULL;	
	pTesetProtocolInfo = NULL;
}
#endif
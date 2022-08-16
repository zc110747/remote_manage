//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//      main entry for the manage system.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "source/UnityMain.hpp"
#include "driver/driver.hpp"

#if __SYSTEM_DEBUG == 1
static void SystemTest(void);
#endif

static bool system_init(int is_default, const char* path);

int main(int argc, char* argv[])
{
	int c;
    int result = 0;
	int nConfigDefault = 0;
	std::string sConfigFile(DEFAULT_CONFIG_FILE);

	//命令行输入说明
	//if no parameter follow, no ":"
	//must one paramter follow, one ":"
	//can be no or one parameter, without space
	while ((c = getopt(argc, argv, "vdf:h::")) != -1)
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
					printf("set file:%s!\n", sConfigFile.c_str());
				}
				break;
			case '?':
			case 'h':
				printf("Usage: app [options] [file]\n");
				printf("-v       	显示版本信息\n");
				printf("-d       	使用默认配置\n");
				printf("-h       	显示帮组选项\n");
				printf("-f [file]	指定选择的配置文件\n");
				exit(0);
				break;
			case 'v':
			case 'V':
				printf("app_demo %s\n", DEVICE_VERSION);
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
	
	system_init(nConfigDefault, sConfigFile.c_str());

#if __SYSTEM_DEBUG == 0
	for(;;)
	{
		char buf[5];
		int ret;
		MessageBase *pSysMessageInfo = getMessageInfo(SYSTEM_MESS_INDEX);
		ret = pSysMessageInfo->read(buf, sizeof(buf)/sizeof(char));
		if(ret != -1)
		{
			pSysMessageInfo->Release();
			break;
		}
		else
		{
			USR_DEBUG("Mq Wait Error,Error Code:%s\n", strerror(errno));
			sleep(100);
		}
	}
#else
	SystemTest();
#endif	

	DriverManage::getInstance()->release();
	UartThreadManage::getInstance()->release();
	USR_DEBUG("Process app_demo stop, error:%s\n", strerror(errno));
	return result;
}

static bool system_init(int is_default, const char* path)
{
	bool ret = true;

	//选中配置文件
	if(is_default == 0)
	{
		if(!SystemConfig::getInstance()->init(path))
		{
			SystemConfig::getInstance()->default_init();
			PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Config Read Error!");
		}
	}
	else
	{
		SystemConfig::getInstance()->default_init();
		PRINT_LOG(LOG_INFO, xGetCurrentTime(), "System Config use default!");
	}
	std::cout<<*(SystemConfig::getInstance())<<std::endl;

	ret &= FIFOManage::getInstance()->init();
	ret &= cmdProcess::getInstance()->init();
	ret &= LoggerManage::getInstance()->init();
	ret &= DriverManage::getInstance()->init();
	ret &= ApplicationThread::getInstance()->init();
	ret &= UartThreadManage::getInstance()->init();
	ret &= TcpThreadManage::getInstance()->init();
	ret &= UdpThreadManage::getInstance()->init();

	return ret;
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
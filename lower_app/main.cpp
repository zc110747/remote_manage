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
#include "include/UartThread.h"
#include "include/ApplicationThread.h"
#include "include/SocketTcpThread.h"
#include "include/SocketUdpThread.h"
#include "include/SystemConfig.h"
#include "driver/Beep.h"
#include "driver/Led.h"

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/
#if __SYSTEM_DEBUG == 1
static void SystemTest(void);
#endif
static void HardwareDriveInit(void);

/**************************************************************************
* Function
***************************************************************************/
/**
 * 代码执行入口函数
 * 
 * @param argc 输入的字符串长度
 * @param argv 输入的字符串数组
 *  
 * @return the error code, 0 on initialization successfully.
 */
int main(int argc, char* argv[])
{
    int result = 0;
	std::string sConfigFile;

	//守护进程初始化
    result = daemon(1, 1);
	if(result < 0)
	{
		USR_DEBUG("daemon\n");
		return result;
	}
	
	//选中配置文件
	if(argc == 2)
	{
		sConfigFile = std::string(argv[1]);
	}
	else
	{
		sConfigFile = std::string("config.json");
	}
	USR_DEBUG("config file:%s\n", sConfigFile.c_str());

	if(system_config_init(sConfigFile) != EXIT_SUCCESS)
	{
		USR_DEBUG("system config read fail, use default\n");
	}

	//硬件模块初始化
	HardwareDriveInit();

	/*任务创建*/
#if __SYSTEM_DEBUG == 0
	UartThreadInit();
	ApplicationThreadInit();
	SocketTcpThreadInit();
	SocketUdpThreadInit();
	for(;;){
		sleep(10);
	}
#else
	SystemTest();
#endif	

	return result;
}

static void HardwareDriveInit(void)
{
	LedDriveInit();
	BeepDriveInit();	
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
	0x00, 0x00, 0x03, 0x07, 0x00, 0x01, 0x87, 0x27
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

	pApplicationReg = new CApplicationReg();
	pTesetProtocolInfo = new CTestProtocolInfo<int *>(rx_buffer, tx_buffer, TEST_BUFFER_SZIE);
	/*更新设备处理指针*/
	SetApplicationReg(pApplicationReg);

	/*执行接收数据的处理指令*/
	nFlag = pTesetProtocolInfo->CheckRxBuffer(nFd, &size);
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
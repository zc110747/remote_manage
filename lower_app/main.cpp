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
#include "include/SocketThread.h"
#include "include/SystemConfig.h"

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
		sConfigFile = std::string("config.js");
	}
	USR_DEBUG("config file:%s\n", sConfigFile.c_str());

	if(system_config_init(sConfigFile) != EXIT_SUCCESS)
	{
		USR_DEBUG("system config read fail, use default\n");
	}

	/*任务创建*/
#if __SYSTEM_DEBUG == 0
	UartThreadInit();
	ApplicationThreadInit();
	SocketThreadInit();
	for(;;){
		sleep(10);
	}
#else
	SystemTest();
#endif	

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
static uint8_t 	rx_buffer[BUFFER_SIZE];
static uint8_t  tx_buffer[BUFFER_SIZE];
const uint8_t test_command[] = {
	0x5a, 0x01, 0x32, 0x23, 0x00, 0x08, 0x02, 0x00, 
	0x00, 0x00, 0x03, 0x07, 0x00, 0x01, 0xFF, 0xFF
};
class CTestProtocolInfo:public CProtocolInfo
{
private: 
	uint16_t read_num;
	uint16_t total_num;

public:
	CTestProtocolInfo(uint8_t *p_rx, uint8_t *p_tx, uint8_t *p_rxd, uint16_t max_bs):
		CProtocolInfo(p_rx, p_tx, p_rxd, max_bs){
			total_num = sizeof(test_command);
			read_num = 0;
	}
	~CTestProtocolInfo(){}

	void clear(void){
		read_num = 0;
	}

	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		uint16_t left_num, min_num;

		left_num = total_num - read_num;
		min_num = nDataSize>=left_num?left_num:nDataSize;
		memcpy(pDataStart, &test_command[read_num], min_num);
		read_num += min_num;

		return min_num;
	}
	
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		printf("send array:");
		SystemLogArray(pDataStart, nDataSize);
		return nDataSize;
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
	CApplicationReg *pApplicationReg;
	CTestProtocolInfo *pTesetProtocolInfo;

	pApplicationReg = new CApplicationReg();
	pTesetProtocolInfo = new CTestProtocolInfo(rx_buffer, tx_buffer, 
								&rx_buffer[FRAME_HEAD_SIZE], BUFFER_SIZE);
	/*更新设备处理指针*/
	SetApplicationReg(pApplicationReg);

	/*执行接收数据的处理指令*/
	nFlag = pTesetProtocolInfo->CheckRxBuffer(nFd);
	if(nFlag == RT_OK){
		pTesetProtocolInfo->ExecuteCommand(nFd);
		pApplicationReg->RefreshAllDevice();
	}

	delete pApplicationReg;
	delete pTesetProtocolInfo;
	pApplicationReg = NULL;	
	pTesetProtocolInfo = NULL;
}
#endif
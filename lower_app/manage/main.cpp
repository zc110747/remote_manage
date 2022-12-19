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
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "UnityMain.hpp"
#include "Semaphore.hpp"
#include "driver.hpp"
#include "TimeManage.hpp"

#if __SYSTEM_DEBUG == 1
static void SystemTest(void);
#endif

int nConfigDefault = 0;
std::string sConfigFile(DEFAULT_CONFIG_FILE);
static EVENT::Semaphore global_exit_sem(0);

static bool system_init(int is_default, const char* path);
static void option_process(int argc, char *argv[])
{
	int c;

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
				//printf("app_demo %s\n", DEVICE_VERSION);
				exit(0);	
			default:
				exit(1);
				break;
		}
	}
}

int main(int argc, char* argv[])
{
	option_process(argc, argv);

	//守护进程，用于进程后台执行
    auto result = daemon(1, 1);
	if(result < 0)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "daemon error!");
		return result;
	}
	
	system_init(nConfigDefault, sConfigFile.c_str());

	for(;;)
	{
		if(global_exit_sem.wait())
		{
			break;
		}
	}
	
	DriverManage::getInstance()->release();
	UartThreadManage::getInstance()->release();
	PRINT_NOW("Process app_demo stop, error:%s\n", strerror(errno));
	return result;
}

static bool system_init(int is_default, const char* path)
{
	bool ret = true;

	//选中配置文件
	if(is_default == 0)
	{
		SystemConfig::getInstance()->init(path);
	}
	else
	{
		SystemConfig::getInstance()->default_init();
		PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "System Config use default!");
	}
	std::cout<<*(SystemConfig::getInstance())<<std::endl;

	ret &= cmdProcess::getInstance()->init();
	ret &= LoggerManage::getInstance()->init();
	ret &= DriverManage::getInstance()->init();
	ret &= NAMESPACE_DEVICE::DeviceManageThread::getInstance()->init();
	ret &= UartThreadManage::getInstance()->init();
	ret &= TcpThreadManage::getInstance()->init();
	ret &= UdpThreadManage::getInstance()->init();
	ret &= TimeManage::getInstance()->init();

	return ret;
}

void exit_main_app()
{
	global_exit_sem.signal();
}

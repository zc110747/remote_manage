//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//      本项目作为嵌入式管理平台的下位机，主要负责网络，串口通讯，本地设备管理，
//		远端设备轮询，详细设计参考document目录下说明
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
//		
/////////////////////////////////////////////////////////////////////////////
#include "modules.hpp"
#include "remote.hpp"
#include "semaphore.hpp"
#include "driver.hpp"
#include "time_manage.hpp"
#include "internal_process.hpp"
#include "center_manage.hpp"

//internal data
static int nConfigDefault = 0;
static std::string sConfigFile(DEFAULT_CONFIG_FILE);
static EVENT::semaphore global_exit_sem(0);

//internal function
static bool system_init(int is_default, const char* path);

//用于处理命令行输入的函数
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
				{
					const uint8_t *pVer = get_version();
					printf("version: %d.%d.%d.%d\n", 
						pVer[0], pVer[1], pVer[2], pVer[3]);
					exit(0);	
				}
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
	
	uart_thread_manage::get_instance()->release();
	PRINT_NOW("Process app_demo stop, error:%s\n", strerror(errno));
	return result;
}

//系统模块初始化
//主要包含调试接口，硬件驱动，设备管理
//网络协议通讯，串口协议通讯，以及时间周期触发的循环事件
static bool system_init(int is_default, const char* path)
{
	bool ret = true;

	//选中配置文件
	if(is_default == 0)
	{
		system_config::get_instance()->init(path);
	}
	else
	{
		system_config::get_instance()->default_init();
		PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "System Config use default!");
	}
	std::cout<<*(system_config::get_instance())<<std::endl;

	ret &= LoggerManage::get_instance()->init();
	ret &= driver_manage::get_instance()->init();
	ret &= NAMESPACE_DEVICE::device_manage::get_instance()->init();
	//ret &= uart_thread_manage::get_instance()->init();
	ret &= tcp_thread_manage::get_instance()->init();
	ret &= internal_process::get_instance()->init();
	ret &= time_manage::get_instance()->init();
	ret &= center_manage::get_instance()->init();

	return ret;
}

void exit_main_app()
{
	global_exit_sem.signal();
}

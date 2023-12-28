//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//     	支持将socket数据转换为
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
#include <getopt.h>
#include "common_unit.hpp"
#include "serial.hpp"
#include "asio_client.hpp"

//internal data
static int nConfigDefault = 0;
static std::string sConfigFile(DEFAULT_CONFIG_FILE);
static EVENT::semaphore global_exit_sem(0);
static option long_opts[] = {
	{"lt_ver", no_argument, NULL, 'v'},
	{"lt_default", no_argument, NULL, 'd'},
	{"lt_file", required_argument, NULL, 'f'},
	{"lt_help", required_argument, NULL, 'h'},
};

//internal function
static bool system_init(int is_default, const char* path);

//用于处理命令行输入的函数
static void option_process(int argc, char *argv[])
{
	int c;

	//命令行输入说明
	//if no parameter_ follow, no ":"
	//must one paramter follow, one ":"
	//can be no or one parameter_, without space
	while ((c = getopt_long(argc, argv, "vdf:h", long_opts, NULL)) != -1)
	{
		switch (c)
		{
			case 'd':
				nConfigDefault = 1;
				break;
			case 'f':
				if (optarg != nullptr)
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
					const auto& version = system_config::get_instance()->get_version();
					std::cout<<"Firmware Version:"<<version<<std::endl;
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
	if (result < 0)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "daemon error!");
		return result;
	}
	
	system_init(nConfigDefault, sConfigFile.c_str());

	for (;;)
	{
		if (global_exit_sem.wait())
		{
			break;
		}
	}
	
	log_manage::get_instance()->release();
	
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
	if (is_default == 0)
	{
		system_config::get_instance()->init(path);
		if (!ret)
		{
			return false;
		}
	}
	else
	{
		system_config::get_instance()->default_init();
		PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "System Config use default!");
	}
	std::cout<<*(system_config::get_instance())<<std::endl;

	ret &= log_manage::get_instance()->init();
	ret &= time_manage::get_instance()->init();
	ret &= serial_manage::get_instance()->init();
	ret &= asio_client::get_instance()->init();
	
	return ret;
}

void exit_main_app()
{
	global_exit_sem.signal();
}

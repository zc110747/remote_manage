//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      parameter_parser.cpp
//
//  Purpose:
//      解析输入参数
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      8/16/2024   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>

#include "json_config.hpp"
#include "parameter_parser.hpp"

const char * const VERSION = "1.0";

//if no parameter_ follow, no ":"
//must one paramter follow, one ":"
//can be no or one parameter_, without space
static std::string short_opts = {"df:hi:m:p:v"};

static const struct option long_opts [] = {
    { "default",        required_argument,  NULL, 'd' },
    { "file",           required_argument,  NULL, 'f' },
    { "help",           no_argument,        NULL, 'h'},
    { "ipaddr",         required_argument,  NULL, 'i'},
    { "mhost",          required_argument,  NULL, 'm'},
    { "mport",          required_argument,  NULL, 'p'},    
    { "version",        no_argument,        NULL, 0},
    { 0, 0, 0, 0 }
};

static void usage(char *prog_name)
{
    printf ("Usage: %s [options] \n"
            "-v | --version 显示版本信息\n"
            "-d | --default 使用默认配置\n"
            "-h | --help    显示帮组选项\n"
            "-i | --ipaddr [ip]     设置本地ip地址\n"
            "-m | --mhost  [ip]     设置mqtt服务器地址\n"
            "-p | --mport  [port]   设置mqtt服务器端口\n"
            "-f | --file   [file]   指定配置文件\n"
            "e.g. :\n"
            "   ./%s -f /home/sys/config/config.json \n"
            "   ./%s -f /home/sys/config/config.json -i [ipaddress] -m -i [ipaddress]\n",
            prog_name, prog_name, prog_name);
}

bool parse_parameter(struct _Params *params, int argc, char **argv)
{
    params->configfile = DEFAULT_CONFIG_FILE;
    params->is_default_config = 0;
    params->ipaddress = DEFAULT_IPADDRESS;
    params->mqtt_host = DEFAULT_IPADDRESS;
    params->mqtt_port = DEFAULT_MQTT_PORT;

    int c = 0;
    while ((c = getopt_long(argc, argv, short_opts.c_str(), long_opts, NULL))!= -1) 
    {
        switch (c) 
        {
        case 'd':
            params->is_default_config = 1;
            break;
        case 'f':
            params->configfile = std::string(optarg);
            break;
        case 'i':
            params->ipaddress = std::string(optarg);
            break;
        case 'm':
            params->mqtt_host = std::string(optarg);
            break;
        case 'p':
            params->mqtt_port = std::atoi(optarg);
            break;
        case 'v':
            printf("version : %s\n", VERSION);
            exit(0);
        case 'h':
            usage(basename(argv[0]));
            exit(0);
        default :
            printf("invalid commnad...\n");
            usage(basename(argv[0]));
            exit(1);
        }
    }

    printf("default_config:%d, config_file:%s, ip:%s, host:%s, port:%d\n",
        params->is_default_config,
        params->configfile.c_str(),
        params->ipaddress.c_str(),
        params->mqtt_host.c_str(),
        params->mqtt_port);

    return true;
}
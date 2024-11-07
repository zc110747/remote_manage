//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      parameter_parser.hpp
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
_Pragma("once")

#include "common.hpp"

struct _Params {
    std::string configfile;
    bool is_default_config;

    std::string ipaddress;
    std::string mqtt_host;
    uint16_t mqtt_port;
};

bool parse_parameter(struct _Params *params, int argc, char **argv);

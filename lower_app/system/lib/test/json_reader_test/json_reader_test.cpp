/*
 * File      : json_reader_test.cpp
 * json cpp读写测试
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-29      zc           the first version
 */
#include "json/json.h"
#include <fstream>
#include <iostream>

using namespace std;

int main(void)
{
    Json::Value root;
    std::ifstream ifs;
    ifs.open("config.json");

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
    std::cout << errs << std::endl;
    return EXIT_FAILURE;
    }
    std::cout << root << std::endl;
    std::cout << root["Uart"]["Baud"] << std::endl;
    return EXIT_SUCCESS;
}
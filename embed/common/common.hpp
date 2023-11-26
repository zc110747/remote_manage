//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      common.hpp
//
//  Purpose:
//      全局的用于支持项目编译包含的库
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

//c interface
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <stdarg.h>

//c++ interface
#include <new>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <atomic>
#include <map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <vector>
#include <functional>

#include "event.hpp"
#include "thread_queue.hpp"
#include "productConfig.hpp"

#if __cplusplus < 201703
#error "c++ compiler need newer than g++7, can use 'g++ -v' to see version."
#endif
//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      includes.hpp
//
//  Purpose:
//      system include.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef __INCLUDES_HPP
#define __INCLUDES_HPP

//c interface
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
//#include <sys/signal.h>

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

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#endif
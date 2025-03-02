//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      cmd_process.hpp
//
//  Purpose:
//      用于支持命令行处理的接口, 包含字符串处理和事件触发
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

#include "common_unit.hpp"

#define COMMAND_MAX_SIZE    4
typedef enum
{
    CmdGetOS,
    cmdGetHelp,
}cmd_format_t;

class cmd_process
{
public:
    /// \brief constructor
    cmd_process() = default;
    
    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief parse_data
    /// - This method is used to parse the receive data and save format.
    /// \param ptr -- start of the point received for parse.
    /// \param size -- size of the point received for parse.
    /// \return Wheather parse is success or failed.
    bool parse_data(char *ptr, int size);

    /// \brief process_data
    /// - This method is used to process the data already parse.
    /// \return Wheather process is success or failed.
    bool process_data();

    /// \brief get_format
    /// - This method is used to process the data already parse.
    /// \return Wheather process is success or failed.
    cmd_format_t get_format()   { return cmd_format_; }
    
private:
    /// \brief cmd_data_pointer_
    /// - memory point the start to data area for received.
    char *cmd_data_pointer_;

    /// \brief cmd_format_
    /// - command alread parse from the data receive.
    cmd_format_t cmd_format_;
};


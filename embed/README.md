# 嵌入式系统设计文档

## embed_framework

对于嵌入式端设计开发，主要包含如下功能模块，有相应文件目录实现。

- gui_manage    :gui界面管理，展示设备支持信息。
- local_device  :本地设备管理，管理本地硬件设备。
- logger_tool   :用于logger打印调试工具。支持调试信息打印和远端命令调试
- lower_device  :用于管理下位机设备数据的进程。
- main_process  :核心进程，管理数据交互提交到mqtt服务器，用于其它模块访问。
- node_server   :node web后台服务，启用node服务器应用。

## logger_tool

logger_tool是用于处理调试信息的工具，其它模块通过接口**PRINT_LOG(level, time, fmt, ...)**将logger信息发送服务器中

# feature

本文档主要说明关键功能的设计思路和方法的整理，具体如下。

## logger-module

logger模块主要为所有模块提供logger打印, 记录和命令远程调试功能. 对于打印，记录功能，流程如下。

1. PRINT_LOG应用接口(各应用中)。
2. logger_rx_fifo，通过logger_tool进程。
3. logger输出接口，支持网络，stdout, 以及本地文件。

在打印基础上，支持打印信息控制，按照等级分为:

- LOG_TRACE trace level，此时打印所有信息
- LOG_DEBUG debug level，在调试模式下打印的信息
- LOG_INFO  info level，此时打印执行中的常用信息
- LOG_WARN  warning level，仅打印警告以上的信息
- LOG_ERROR error level, 仅打印错误以上的信息
- LOG_FATAL fatal level, 仅打印致命以上的信息

关于系统的权限管理，分为两部分，主要是调试信息打印权限和记录权限，打印权限主要用于产品出问题时代码调试，记录权限则用于记录系统长期运行的状态，用于后期产品工作状态分析，对于系统权限的修改，主要包含功能实现为。

1. 支持logger权限的修改，支持桌面工具(开启调试模式密码)，UI界面和logger工具直接修改权限
2. 权限修改能够存储文件，在下次启动时保留。
3. 打印权限由各个模块独自管理，提高工作效率，记录权限由logger管理，确定何时记录logger信息。

上述时logger模块的核心述求。另外为了验证新功能，权限的配置文件用数据库文件存储(基于sqlite)。

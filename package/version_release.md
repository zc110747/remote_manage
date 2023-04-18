# Ver1.0.0.7

## manage

1. 基于asio实现tcp服务器，移除udp服务器
2. 协议模块重写，基于fifo实现解析处理，替换tcp和串口模块中实现
3. 代码注释维护，所有类更新完整的注释
4. 扩展node模块为Internal通讯管理模块，后续gui使用该接口通讯

## upper_app
1. 扩展调试工具，支持hex处理

# Ver1.0.0.6

## manage

1. 实现基于asio的logger服务器，支持命令处理
2. Device管理重构,功能模块化
3. 事件处理，信号量处理添加
4. 扩展支持与node通讯模块，用于网页访问控制硬件

## server

1. 基于node实现web服务器，支持静态页面获取和动态处理
2. 实现灯和蜂鸣器的远程控制
3. 实现与manage程序的基于localhost的tcp通讯  设备
4. 基于vue实现简单的页面

## upper_app

1. 基于C#实现的支持tcp客户端调试工具

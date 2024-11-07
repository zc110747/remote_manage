# 协议文档定义说明

对于项目来说，首先定义包含的模块。

![image](image/mainFrame.png)

- 应用端、界面组件
  - Brower: 前端网页访问
  - server: 基于node的服务器实现
  - PC Winform: 桌面端软件访问
  - Android Dev: 安卓桌面端软件
  - local gui(Qt): 本地GUI界面
- 嵌入式端组件
  - mqtt sever: 基于开源的mosquitto服务器
  - main_process: 主进程，用于和mosquitto服务器
  - local_device: 用于管理本地资源的进程
  - lower_device: 用于管理下位机资源的进程
  - logger_tools： 打印调试的进程
- 其它设备
  - lower_manage: 下位机设备，类似STM32等

## 网页前端和后端Node交互协议

定义前端和服务器之间的命令和状态(只有软件支持升级，界面不支持)，命令如下

[获取设备状态](#req_getstat)
[获取设备信息](#req_getinfo)
[设置设备状态](#req_setdev)

### req_getstat

```json
//获取配置信息
request:
{
    "command":"req_getstat",
}

replay:
{
    "replay": "rep_getstat",
    "status": "ok", "error-state",
    "data" : {
        //device information
    }
}
```

### req_getinfo

### req_setdev

```json
//设置设备信息
request:
{
    "command":"req_setdev",
    "data" : {
        "dev": "example",
        "value": "data set" 
    }
}

replay:
{
    "replay": "rep_setdev",
    "status": "ok", "error-state",
    "data" : {
        //device information
    }
}
```

### req_setnet

```json
//设置设备信息
request:
{
    "command":"req_setnet",
    "data" : {
        "dev": "example",
        "value": "data set" 
    }
}

replay:
{
    "replay": "rep_getnet",
    "status": "ok", "error-state",
    "data" : {
        //device information
    }
}
```

## 其它模块和MainProcess的交互

其它模块包含: Node后端服务器，GUI屏幕，桌面客户端

模块之间通过MQTT服务进行交互， 格式如下


### json数据格式

A0: 表示数据为json格式(A0 + json数据)，后续数据按照json解析

示例: A0+{"command":"get_dev_info"}

满足此格式的主要为控制命令，如获取设备信息，设置设备信息，获取配置信息, 格式:

#### req_getstat

```json
A0{"command":"req_getstat","source":"SRC_DESKTOP"}

//获取设备状态
//request
{
    "command":"req_getstat",
    "source": ["SRC_NODE"/"SRC_GUI"/"SRC_DESKTOP"],
}

//replay
{
    "replay": "rep_getstat",
    "status": "ok", "error-state",
    "data": {
        "key_area": {
            "led_status" : 0,
            "beep_status" : 0,
        },
        "pwm_area": {
            "pwm_duty" : 0,
        },
        "time_area": {
            "rtc_time" : "2014-8-5 17:10",
        },
        "status_area": {
            "angle" : 0,
            "temperature": 0,
            "voltage": 3.3,
            "als": 0,
        }
    }
}
```

#### req_getinfo

```json
//sample
A0{"command":"req_getinfo","source":"SRC_DESKTOP"}

//获取设备信息
//request
{
    "command":"req_getinfo",
    "source": ["SRC_NODE"/"SRC_GUI"/"SRC_DESKTOP"],   
}
//replay
{
    "replay": "rep_getinfo",
    "status": "ok", "error-state",
    "data": {
        //...
    }
}
```

#### req_setdev

```json
//设置设备信息
{
    "command":"req_setdev",
    "source": ["SRC_NODE"/"SRC_GUI"/"SRC_DESKTOP"],
    "data" : {
        "device": ["led"/"beep"/"pwm"],
        "value": ["on"/"off"/"data"]
    }
}
//replay
{
    "replay": "rep_setdev",
    "status": "ok", "error-state",
    "data": {
        //...
    }
}
```

#### req_setnet

```json
//设置设备信息
{
    "command":"req_setnet",
    "source": ["SRC_NODE"/"SRC_GUI"/"SRC_DESKTOP"],
    "data" : {
        "value": ["on"/"off"/"data"]
    }
}
//replay
{
    "replay": "rep_setnet",
    "status": "ok", "error-state",
    "data": {
        //...
    }
}
```

### 二进制格式

A1: 表示数据为二进制(A1 + 二进制格式数据)，后续按照二进制解析, 格式

格式如下:

| HEAD | 数据长度(2字节) | 数据(cmd+data) | 异或校验(1字节，数据长度+数据) | 
| --- | --- | --- | --- |
| 0x41 0x31 | 长度(2字节) | 数据 | 校验(1字节) |

主要用于特殊命令，如升级，查询设备版本信息

### A0_JSON

### A1_HEX_DATA
# remote_manage
上位机使用QT实现的界面<br/>
支持opencv的方法说明:<br/>
1.解压support\opencv_install.7z, 并复制到和remote_manage同目录<br/>
2.将xxx\opencv_install\x64\mingw\bin路径添加到系统环境Path中<br/>
3.在QT中选择项目->Buid&Run, 将编译选项切换为MinGW 64-bit(选择其它选项会出现链接错误)<br/>
4.编译即可，如果编译后出现程序执行异常，可能原因为动态库路径没有正确链接，可以检查Path，或者将bin下的dll文件
添加到系统路径中。<br/>

开发日志记录<br/>
## version 0.0.0.1
1.实现了界面框架，后续功能添加<br/>

## version 0.0.0.2
1.添加了串口驱动的的配置实现<br/>
2.增加了协议的封装处理<br/> 
3.增加环形队列和多线程的实现,用于多线程同步处理<br/>
4.增加了信号槽，用于子线程和主线程通讯<br/>

## version 0.0.0.3
1.增加TCP通讯相关的实现<br/>

## version 0.0.0.4
1.增加对读取数据的处理，添加应用回调函数<br/>

## version 0.0.0.5
1.增加对于UDP的通讯实现<br/>
2.对应用的线程进行整理，由一个线程管理所有通讯<br/>
3.添加对界面的table处理<br/>
4.添加对opencv的支持，并提供编译完成的opencv库，不支持opencv的可以使用宏管理<br/> 


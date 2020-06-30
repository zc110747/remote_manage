# remote_manage

功能: 
添加项目需要的外部静态库  
jsoncpp原始地址:https://github.com/open-source-parsers/jsoncpp
openssl原始地址:https://github.com/openssl/openssl  

项目路径:  
jsoncpp/ json解析的库编译方法  
test/   静态库的测试代码，默认为PC平台测试, arm平台需要修改Makefile重新编译 

库编译方法:  
进入指定库，执行make指令
如  
cd jsoncpp/  
make
# 编译规则说明
&emsp;&emsp;基于linux下Makefile语法实现的支持c,cpp编译的脚本，支持编译可执行文件，静态库和动态库，脚本执行依赖部分env环境，使用前需要将env/*中的信息加载到系统环境中，具体加载系统环境可参考外部目录下README.md说明，具体实现脚本的规则详细见下面分类。<br />
编译后所有文件会放置在buildout目录下，支持arm(arm32), aarch64(arm64), i386(x86)三个平台。

## 编译可执行文件Makefile例程
```shell
#编译输出可执行文件
BUILD?=e

#添加CPP flags
CCFLAGS:=-O3 -std=c++17 -fno-strict-aliasing -lrt

#添加头文件目录
INCLUDES=-I include/

#编译输出
buildout=test

#编译的cpp文件
cpp_objects=test_cpp.o

include $(ENV_PATH_ROOT)/build/buildrules.mk
```

## 编译静态库Makefile例程
```shell
#编译输出可执行文件
BUILD?=s

#添加C flags
CFLAGS :=

#添加头文件目录
INCLUDES=-I include/

#编译输出格式为libtest.a
buildout=libtest

#编译的c文件
c_objects=test_c.o

include $(ENV_PATH_ROOT)/build/buildrules.mk
```

## 编译动态库Makefile例程
```shell
#编译输出可执行文件
BUILD?=d

#添加C/CPP flags
CFLAGS :=
CCFLAGS :=

#添加头文件目录
INCLUDES=-I include/

#编译输出格式为libtest.so
buildout=libtest

#编译的c/cpp文件
c_objects=test_c.o
cpp_obects=test_cpp.o

include $(ENV_PATH_ROOT)/build/buildrules.mk
```
# **1.目录说明**

内核驱动模块和测试代码的实现，作为项目开发中的驱动部分.<br/>
整个驱动模块分为两大类:<br/>
1. 根据设备驱动的需要，添加修改的设备树文件<br/>
2. 设备驱动的具体实现<br/>
beep/       蜂鸣器外设的驱动<br/>
dts/        用于支持驱动配置的设备树文件<br/>
i2C_ap/     i2c光照传感器的驱动<br/> 
key/        按键操作的应用<br/>
led/        LED的驱动<br/>
rs232/      串口说明和rs232上层的应用(十六进制访问)<br/>
spi_icm/    SPI六轴传感器驱动<br/>
测试代码则为加载模块后，可检测驱动功能的简单应用实现，后续项目中使用相应的扩展功能。<br/>

# **2.嵌入式Linux驱动开发说明**
&emsp;&emsp;对于嵌入式Linux驱动开发，主要包含两部分，一部分是系统硬件相关的初始化，另一部分将驱动注册在内核中的接口。<br/>
## **2.1 硬件初始化**
&emsp;&emsp;硬件初始化的接口，包含寄存器直接访问，设备树访问等多种方式。<br/>
### **2.1.1 寄存器访问**
寄存器访问方式通过直接定义寄存器物理地址，然后直接通过内核提供的接口writel和readl修改和读取即可，下面则举例说明.<br/>
```c
/* 寄存器物理地址 */
#define CCM_CCGR1_BASE				(0X020C406C)	
#define SW_MUX_GPIO1_IO03_BASE		(0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE		(0X020E02F4)
#define GPIO1_DR_BASE				(0X0209C000)
#define GPIO1_GDIR_BASE				(0X0209C004)

//将寄存器映射到虚拟空间访问的地址
static void __iomem *IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
static void __iomem *SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
static void __iomem *SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
static void __iomem *GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
static void __iomem *GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

//使能GPIO1时钟
val = readl(IMX6U_CCM_CCGR1);
val &= ~(3 << 26);	
val |= (3 << 26);
writel(val, IMX6U_CCM_CCGR1);

//设置IO复用和连接
writel(5, SW_MUX_GPIO1_IO03);
writel(0x10B0, SW_PAD_GPIO1_IO03);

//......

//取消寄存器映射
iounmap(IMX6U_CCM_CCGR1);
iounmap(SW_MUX_GPIO1_IO03);
iounmap(SW_PAD_GPIO1_IO03);
iounmap(GPIO1_DR);
iounmap(GPIO1_GDIR);

//访问接口
//读数据接口，分别对应8bit，16bit和32bit
u8 readb(const volatile void __iomem *addr)
u16 readw(const volatile void __iomem *addr)
u32 readl(const volatile void __iomem *addr)

//写数据接口，分别对应8bit，16bit和32bit
void writeb(u8 value, volatile void __iomem *addr)
void writew(u16 value, volatile void __iomem *addr)
void writel(u32 value, volatile void __iomem *addr)
```
&emsp;&emsp;这种方式访问类似于单片机的使用方法，不过这种方法对于不同的芯片，外设和产品都需要涉及不同的寄存器访问实现，这对于支持多个平台的嵌入式Linux系统，会带来大量的数据冗余，所以Linux内核提供了设备树语法，通过dts文件，来支持不同芯片，设备，在框架上则可以更加灵活。<br/>
### **2.1.2 设备树语法**
&emsp;&emsp;设备树将平台设备进行完全的抽象，是用节点描述系统中设备的树状结构。<br/>

//完善中...<br/>

## **2.2 将驱动加载到系统中**
### **2.2.1 驱动加载命令**
&emsp;&emsp;Linux驱动会以内核模块的形式出现，对于Linux驱动，支持将所有需要的功能都编译到Linux内核中，也可以将驱动编译到成模块，通过命令加载到Linux系统中，对于模块的加载，支持如下命令<br />
```bash
insmod [mod]    #加载指定的模块路径，不支持自动添加依赖模块，会报错
modprobe [mod]  #通过模块名加载，且会自动加载依赖的模块
#modprobe加载内核的文件必须位于/lib/modules/(shell uname -r)/下
depmod          #生成modprobe依赖的modules.dep
rmmod [mod]     #移除现有的module
```
### **2.2.2 驱动加载入口**
&emsp;&emsp;对于标准的Linux驱动，需要声明作者，license, 描述，以及最重要的用于加载和移除调用的接口，具体如下.<br />
```c
#include <linux/init.h>
#include <linux/module.h>

static int __init test_init(void)
{
    return 0;
}
static int __exit test_exit(void)
{
    return 0;
}

module_init(test_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZC");
```
此外也有些扩展宏，允许驱动包含更多信息，如:<br />
&emsp;&emsp;MODULE_DESCRIPTION("string") 模块的功能描述.<br />
&emsp;&emsp;MODULE_ALIAS("name") 模块的别名等.<br />

### **2.2.3 驱动注册实现**
&emsp;&emsp;对于驱动注册，
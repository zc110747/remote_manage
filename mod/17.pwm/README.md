# pwm

## pwm_interface

kernel中pwm的目录: "/drivers/pwm/pwm-imx27.c"

pwm寄存器说明目录： Chapter 40: Pluse Width Modulation

- PWMCR寄存器

```c
/*
* PWMCR: pwm控制寄存器
* 地址: 0x020f8000
* 位功能
* bit[27:26] 设置FIFO置空标志位置位的条件
* bit[25] 停止模式下PWM是否工作，1表示工作
* bit[24] 休眠模式下PWM是否工作，1表示工作
* bit[23] 等待模式下PWM是否工作，1表示工作
* bit[22] 调试模式下PWM是否工作，1表示工作
* bit[21] 从FIFO到寄存器写入时，字节是否交换，0表示不交换(16-bit数据时)
* bit[20] 从FIFO到寄存器写入时，半字是否交换，0表示不交换(32-bit数据时)
* bit[19:18] PWM输出极性，00:重载计数时高电平，到达计数后低电平 01：重载计数时低电平，到达计数后高电平
* bit[17:16] PWM时钟源，00:无时钟 01:ipg时钟 02:ipg_highfreq 03:ipg_32k， 01 ipg=66M
* bit[15:4] 时钟分频，clock/(data+1) prescaler
* bit[3] 软件复位，设置1复位，复位完成后自动清零
* bit[2:1] FIFO中每个样本使用的次数, 00:1 times 01：2 times, 02: 4 times 03: 8 times
* bit[0] pwm使能位，未使能状态下，prescale和count在复位状态
*/
```

- PWMSR寄存器

```c
/*
* PWMSR: pwm状态寄存器
* 地址: 0x020f8004
* 位功能
* bit[6] FIFO写入错误状态标志位
* bit[5] 比较事件触发标志位
* bit[4] 翻转事件触发标志位
* bit[3] FIFO是否为空标志位
* bit[2:0] FIFO内数据状态，只读数据，写入无效
*/
```

- PWMIR寄存器

```c
/*
* PWMIR: pwm中断控制寄存器
* 地址: 0x020f8008
* 位功能
* bit[2] 比较中断使能
* bit[1] 翻转中断使能
* bit[0] FIFO空中断使能
*/
```

- PWMSAR寄存器

```c
/*
* PWMSAR: pwm sample fifo写入寄存器
* 地址: 0x020f800c
* 位功能
* bit[15:0] sample value
*/

state->dutycycle = (sample/(pwm_clk/prescaler))*NSEC_PER_SEC
      = (NSEC_PER_SEC*(u64)sample*prescaler)/pwm_clk
```

- PWMPR寄存器

```c
/*
* PWMPR: pwm周期寄存器
* 地址: 0x020f8010
* 位功能
* bit[15:0] peroid value
*/

PWMO (Hz) = PCLK(Hz) / (period +2)
state->period = (((u64)period + 2)/(pwm_clk/prescaler))*NSEC_PER_SEC
      = (NSEC_PER_SEC*((u64)period+2)*prescaler)/pwm_clk
```
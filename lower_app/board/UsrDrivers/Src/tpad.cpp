
#include "tpad.hpp"
#include "driver.hpp"
#include "includes.hpp"

#define TPAD_ARR_MAX_VAL  0XFFFFFFFF
void tpad_driver::init(void)
{
    uint16_t buf[10];
    uint16_t temp;
    uint8_t j,i;
    uint8_t psc = 2;
    
	hardware_init(TPAD_ARR_MAX_VAL,psc-1);//设置分频系数
	for(i=0;i<10;i++)//连续读取10次
	{				 
		buf[i]=get_value();
		HAL_Delay(10);	    
	}				    
	for(i=0;i<9;i++)//排序
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	for(i=2;i<8;i++)
        temp+=buf[i];//取中间的8个数据进行平均
	
    default_val=temp/6;
    printf("tpad default_val:%d\r\n",default_val);	
}

uint8_t tpad_driver::scan_key(uint8_t mode)
{
	static uint8_t keyen=0;	//0,可以开始检测;>0,还不能开始检测	 
	uint8_t  res=0;
	uint8_t  sample=3;	//默认采样次数为3次	 
	uint16_t rval;
	if(mode)
	{
		sample=6;	//支持连按的时候，设置采样次数为6次
		keyen=0;	//支持连按	  
	}
	rval=get_max_value(sample); 
	if(rval>(default_val*4/3)&&rval<(10*default_val))//大于tpad_default_val+(1/3)*tpad_default_val,且小于10倍tpad_default_val,则有效
	{							 
		if(keyen==0)
            res=1;	//keyen==0,有效 
		//printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//至少要再过3次之后才能按键有效   
	} 
	if(keyen)
        keyen--;		
    
	return res;
}	

void tpad_driver::reset()
{
    GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin = GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull = GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);	//PA5输出0，放电
    
    HAL_Delay(5);
    __HAL_TIM_CLEAR_FLAG(&TIM2_Handler,TIM_FLAG_CC1|TIM_FLAG_UPDATE);   //清除标志位
    __HAL_TIM_SET_COUNTER(&TIM2_Handler,0); //计数器值归0
    
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //推挽复用
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    GPIO_Initure.Alternate=GPIO_AF1_TIM2;   //PA5复用为TIM2通道1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure); 
}

uint16_t tpad_driver::get_value(void)
{
    reset();
    
    while(__HAL_TIM_GET_FLAG(&TIM2_Handler,TIM_FLAG_CC1)==RESET) //等待捕获上升沿
    {
        if(__HAL_TIM_GET_COUNTER(&TIM2_Handler)>TPAD_ARR_MAX_VAL-500) 
            return __HAL_TIM_GET_COUNTER(&TIM2_Handler);//超时了，直接返回CNT的值
    };
    return HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);
}

uint16_t tpad_driver::get_max_value(uint8_t n)
{
    uint16_t temp=0; 
	uint16_t res=0; 
	uint8_t lcntnum=n*2/3;//至少2/3*n的有效个触摸,才算有效
	uint8_t okcnt=0;
	while(n--)
	{
		temp=get_value();//得到一次值
		if(temp>(default_val*5/4))
            okcnt++;//至少大于默认值的5/4才算有效
		if(temp>res)
            res=temp;
	}
	if(okcnt>=lcntnum)
        return res;//至少2/3的概率,要大于默认值的5/4才算有效
	else 
        return 0;
}

bool tpad_driver::test()
{
    return true;
}
    
void tpad_driver::hardware_init(uint32_t arr, uint16_t psc)
{
    TIM_IC_InitTypeDef TIM2_CH1Config;  

    __HAL_RCC_TIM2_CLK_ENABLE();
   
    TIM2_Handler.Instance=TIM2;                          //通用定时器3
    TIM2_Handler.Init.Prescaler=psc;                     //分频
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM2_Handler.Init.Period=arr;                        //自动装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM2_Handler);

    TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //上升沿捕获
    TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM2_CH1Config.ICFilter=0;                          //配置输入滤波器，不滤波
    HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);//配置TIM2通道1
    HAL_TIM_IC_Start(&TIM2_Handler,TIM_CHANNEL_1);      //开始捕获TIM2的通道1
}
  
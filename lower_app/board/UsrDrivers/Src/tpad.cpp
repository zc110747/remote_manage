
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
    
	hardware_init(TPAD_ARR_MAX_VAL,psc-1);//���÷�Ƶϵ��
	for(i=0;i<10;i++)//������ȡ10��
	{				 
		buf[i]=get_value();
		HAL_Delay(10);	    
	}				    
	for(i=0;i<9;i++)//����
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	for(i=2;i<8;i++)
        temp+=buf[i];//ȡ�м��8�����ݽ���ƽ��
	
    default_val=temp/6;
    printf("tpad default_val:%d\r\n",default_val);	
}

uint8_t tpad_driver::scan_key(uint8_t mode)
{
	static uint8_t keyen=0;	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	 
	uint8_t  res=0;
	uint8_t  sample=3;	//Ĭ�ϲ�������Ϊ3��	 
	uint16_t rval;
	if(mode)
	{
		sample=6;	//֧��������ʱ�����ò�������Ϊ6��
		keyen=0;	//֧������	  
	}
	rval=get_max_value(sample); 
	if(rval>(default_val*4/3)&&rval<(10*default_val))//����tpad_default_val+(1/3)*tpad_default_val,��С��10��tpad_default_val,����Ч
	{							 
		if(keyen==0)
            res=1;	//keyen==0,��Ч 
		//printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//����Ҫ�ٹ�3��֮����ܰ�����Ч   
	} 
	if(keyen)
        keyen--;		
    
	return res;
}	

void tpad_driver::reset()
{
    GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin = GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull = GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);	//PA5���0���ŵ�
    
    HAL_Delay(5);
    __HAL_TIM_CLEAR_FLAG(&TIM2_Handler,TIM_FLAG_CC1|TIM_FLAG_UPDATE);   //�����־λ
    __HAL_TIM_SET_COUNTER(&TIM2_Handler,0); //������ֵ��0
    
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //���츴��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    GPIO_Initure.Alternate=GPIO_AF1_TIM2;   //PA5����ΪTIM2ͨ��1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure); 
}

uint16_t tpad_driver::get_value(void)
{
    reset();
    
    while(__HAL_TIM_GET_FLAG(&TIM2_Handler,TIM_FLAG_CC1)==RESET) //�ȴ�����������
    {
        if(__HAL_TIM_GET_COUNTER(&TIM2_Handler)>TPAD_ARR_MAX_VAL-500) 
            return __HAL_TIM_GET_COUNTER(&TIM2_Handler);//��ʱ�ˣ�ֱ�ӷ���CNT��ֵ
    };
    return HAL_TIM_ReadCapturedValue(&TIM2_Handler,TIM_CHANNEL_1);
}

uint16_t tpad_driver::get_max_value(uint8_t n)
{
    uint16_t temp=0; 
	uint16_t res=0; 
	uint8_t lcntnum=n*2/3;//����2/3*n����Ч������,������Ч
	uint8_t okcnt=0;
	while(n--)
	{
		temp=get_value();//�õ�һ��ֵ
		if(temp>(default_val*5/4))
            okcnt++;//���ٴ���Ĭ��ֵ��5/4������Ч
		if(temp>res)
            res=temp;
	}
	if(okcnt>=lcntnum)
        return res;//����2/3�ĸ���,Ҫ����Ĭ��ֵ��5/4������Ч
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
   
    TIM2_Handler.Instance=TIM2;                          //ͨ�ö�ʱ��3
    TIM2_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM2_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM2_Handler);

    TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM2_CH1Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);//����TIM2ͨ��1
    HAL_TIM_IC_Start(&TIM2_Handler,TIM_CHANNEL_1);      //��ʼ����TIM2��ͨ��1
}
  
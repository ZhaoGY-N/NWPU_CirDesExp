#include "timer.h"
#include "led.h"
#include "usart3.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
int time = 0,second = 0 ,minute = 0,hour = 0,time_s = 0,time_flag = 0;
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		time++;//DS1翻转
		if(time>4)
		{
			time_flag=0;
			time=0;
			
			USART_SendData(USART3,0xA5);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			USART_SendData(USART3,0x5A);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			USART_SendData(USART3,0x04);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			USART_SendData(USART3,0x80);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			USART_SendData(USART3,0x03);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			USART_SendData(USART3,0x00);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			USART_SendData(USART3,0x01);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			
			LED0=0;
			
			TIM_Cmd(TIM3, DISABLE);
		}
		else
		{
			time_flag=1;
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}


////////////////////////////////////////////////////////////////////
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM4时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许定时器4更新中断
	TIM_Cmd(TIM4,ENABLE); //使能定时器4
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void sendTime()
{
		u8 data_to_send[3] = {0};
		USART_SendData(USART3,0xA5);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x5A);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x0A);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x82);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x00);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x08);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕

		data_to_send[0]=((second/10)<<4)+second%10;
		data_to_send[1]=((minute/10)<<4)+minute%10;
		data_to_send[2]=((hour/10)<<4)+hour%10;

		USART_SendData(USART3,0x21);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x05);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x30);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x07);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕

		USART_SendData(USART3,data_to_send[2]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,data_to_send[1]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕 
		USART_SendData(USART3,data_to_send[0]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕 
}

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{
		time_s++;
		if(time_s>10)
		{
			sendTime();
			second++;//DS1翻转

			if(second >= 60)
			{
				second = 0;
				minute = minute + 1;
				if (minute >= 60)
				{
					minute = 0;
					hour = hour + 1;
					if (hour >= 24)
					{
						hour = 0;
					}
				}
			}
			time_s=0;
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
}

////////////////////////////////////////////////////////////////////


void TIM2_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//TIM2时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断
	
	TIM_Cmd(TIM2,ENABLE);//开启定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}

void TIM2_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART3_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIM2更新中断标志    
		TIM_Cmd(TIM2, DISABLE);  //关闭TIM2 
	}	    
}

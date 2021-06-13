#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include "dac.h"
#include "key.h"
#include "led.h"
#include "timer.h"
#include "string.h"
#include "usart3.h"


//***************************************************************//
//****PA5为adc采样口or,		pc10 串口屏tx,pc11 串口屏rx,    pd2产生PWM方波
//***************************************************************//
extern int time;
u16 adcx;
u8 ind=0;
u16 len,fire_grade,fire_grades[3] = {0},res = 0;
int main(void)
{ 
	u8 data_to_send[2] = {0};
	float a_temp=0;                     	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);                               //初始化延时函数
	uart_init(115200);		                         //初始化串口波特率为115200
	usart3_init(115200);
	LED_Init();	        //led初始化
	TIM3_Int_Init(3049-1,84-1);//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数100次为10ms，频率为50HZ
	Adc_Init(); 				//adc初始化
	KEY_Init(); 				//按键初始化
	
//	LED0 = 0;						//工作，低电平有效
	while(1)
	{
		//0xff 0xff 包头
		//过流检测
		//adc采样，使用的通道是PA5，参考电压是3.3V
//		adcx=Get_Adc_Average(ADC_Channel_5,200);
		
		adcx=Get_Adc_rankAver();
		a_temp=(float)adcx*(2.5/4096.0);        //（这里ADC的参考电压需要确定）电流值
		if(adcx >=2500)
		{
			res = 1;
			fire_grades[ind++] = 1;
		}
		if(adcx <2300 &&adcx >= 1900)
		{
			res = 2;
			fire_grades[ind++] = 2;
		}
		if(adcx < 1800&&adcx >= 1500)
		{
			res = 3;
			fire_grades[ind++] = 3;
		}
		if(adcx <1450&&adcx >= 1300)
		{
			res = 4;
			fire_grades[ind++] = 4;
		}
		if(adcx <1250&&adcx >= 1150)
		{
			res = 5;
			fire_grades[ind++] = 5;
		}
		if(adcx <1120&&adcx >= 1050)
		{
			res = 6;
			fire_grades[ind++] = 6;
		}
		if(adcx <1040&&adcx >= 1000)
		{
			res = 7;
			fire_grades[ind++] = 7;
		}
		if(adcx <935&&adcx >= 910)
		{
			res = 8;
			fire_grades[ind++] = 8;
		}
		if(adcx < 900&&adcx >= 870)
		{
			res = 9;
			fire_grades[ind++] = 9;
		}
		if(adcx < 830)
		{
			res = 10;
			fire_grades[ind++] = 10;
		}
		if(ind>=3)
			ind=0;
		if(fire_grades[0]==fire_grades[1])
			fire_grade=fire_grades[0];
		else if(fire_grades[1]==fire_grades[2])
			fire_grade=fire_grades[1];
		else if(fire_grades[0]==fire_grades[2])
			fire_grade=fire_grades[2];
		else fire_grade=fire_grades[0];
		if(time>=20)
		{

			time = 0;
			memcpy(&data_to_send,&fire_grade,2);
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
			USART_SendData(USART3,fire_grade);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
			  

//			printf("%d\r\n",fire_grade);
//			printf("%d\r\n",adcx);
		}
		if(USART3_RX_STA&0x8000)
		{
			len=USART3_RX_STA&0x3fff;
			if(USART3_RX_BUF[len-1]== 0x11)
			{
			}
			if(USART3_RX_BUF[len-1]== 0x22)
			{
			}
			USART3_RX_STA=0;
		}	
	}	
}

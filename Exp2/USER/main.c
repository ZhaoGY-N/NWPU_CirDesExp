	#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "max31865.h"
#include "led.h"
#include "timer.h"
#include "string.h"
#include "usart3.h"

//***************************************************************//
//****软件模拟SPI   
//***************************************************************//
extern int time_s;
extern int time;
extern int second;
extern int minute;
extern int hour;
extern int time_flag;

char history[4][20]={0};

void SendData(u16 adress,int data)
{
		u8 data_to_send[2] = {0};
		memcpy(&data_to_send,&data,2);
		USART_SendData(USART3,0xA5);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x5A);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x05);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x82);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,adress>>8);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,(u8)adress);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,data_to_send[1]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,data_to_send[0]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕 
}

void setPage(u8 page)
{
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
	USART_SendData(USART3,page);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
}

void history_gen(int temp)
{
	u8 i=0;
	float tempf=(float)temp/10.0;
	memcpy(history[3],history[2],sizeof(history[2]));
	memcpy(history[2],history[1],sizeof(history[2]));
	memcpy(history[1],history[0],sizeof(history[2]));
	sprintf(history[0],"%02d:%02d:%02d   %3.1f",hour,minute,second,tempf);
	history[0][16]=0xff;
	history[0][17]=0xff;
	return ;
}

void history_send()
{
	u8 j,i,num;
				//历史数据地址 0x0030 0x0040 0x0050 0x0060
	
	for(j=0x30;j<0x70;j+=0x10)//循环发送数据
	{
		USART_SendData(USART3,0xA5);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x5A);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,21);  //字符串长度+3
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x82);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,0x00);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		USART_SendData(USART3,j);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		for(i=0;i<18;i++)
		{
			switch(j)
			{
				case 0x30:num=0;break;
				case 0x40:num=1;break;
				case 0x50:num=2;break;
				case 0x60:num=3;break;
				default:break;
			}
			USART_SendData(USART3,history[num][i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//循环发送,直到发送完毕
		}
	}
	return ;
}


u8 historied=0;
int main(void)
{
	u8 data_to_send[2] = {0};
	u8 alstatus;
	u8 resTemp=0;
	u16 len;
	int16_t temp = 0,temp_al=1000;
	float temp_1=0;
	RCC_ClocksTypeDef RCC_Clocks;                  	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);                               //初始化延时函数
	uart_init(115200);		                         //初始化串口波特率为115200
	usart3_init(115200);
	TIM3_Int_Init(10000-1,8400-1);//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数1000次为100ms
	TIM4_Int_Init(1000-1,8400-1);//1s
	LED_Init();	        //led初始化
	MAX31865_Init();
	MAX31865_Cfg();
	USART3_RX_STA=0;
	RCC_GetClocksFreq(&RCC_Clocks);
	
//	LED0 = 0;						//工作，低电平有效
	while(1)
	{

		temp_1=MAX31865_GetTemp();   //采样得到温度
		temp_1=tempCorrection(temp_1);
		temp = (int16_t)(temp_1*10);
		if(temp<2680&&temp>temp_al&&historied==0)
		{
			delay_ms(20);
			temp_1=MAX31865_GetTemp();   //采样得到温度
			temp_1=tempCorrection(temp_1);
			temp = (int16_t)(temp_1*10);
			if(temp<2680&&temp>temp_al)
			{
				TIM_SetCounter(TIM3,0);       		//计数器清空
				time=0;
				TIM_Cmd(TIM3, ENABLE); 	    			//使能定时器3
				setPage(0x03);
				LED0=1;
				
				historied=1;
				delay_ms(50);
				temp_1=MAX31865_GetTemp();   //采样得到温度
				temp_1=tempCorrection(temp_1);
				temp = (int16_t)(temp_1*10);
			
				history_gen(temp);
				history_send();
			}
		}
//		if(temp<2680&&temp>temp_al&&historied==1)
//		{
//			delay_ms(50);
//			temp_1=MAX31865_GetTemp();   //采样得到温度
//			temp_1=tempCorrection(temp_1);
//			temp = (int16_t)(temp_1*10);
//			if(temp<2680&&temp>temp_al)
//			{
//				TIM_SetCounter(TIM3,0);       		//计数器清空
//				time=0;
//				TIM_Cmd(TIM3, ENABLE); 	    			//使能定时器3
//				setPage(0x03);
//				LED0=1;
//			}
//		}
		if((temp<temp_al||temp>=2680)&&historied==1)
		{
			historied=0;
		}
		
		SendData(0x0010,temp);
		SendData(0x019A,temp_al);



		if(USART3_RX_STA&0x8000)
		{
			len=USART3_RX_STA&0x3fff;
			{
				if(USART3_RX_BUF[3]==0x83)
				{
					if(USART3_RX_BUF[4]==0x01&&USART3_RX_BUF[5]==0x9A)  //输入报警温度
					{
						temp_al = (int16_t)((USART3_RX_BUF[7]<<8)+USART3_RX_BUF[8]);
					}
				}
			}
			memset(USART3_RX_BUF,0,len+3);
			USART3_RX_STA=0;
		}




	}

}



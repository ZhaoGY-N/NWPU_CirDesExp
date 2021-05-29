#include "timer.h"
#include "led.h"
#include "usart3.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
int time = 0,second = 0 ,minute = 0,hour = 0,time_s = 0,time_flag = 0;
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		time++;//DS1��ת
		if(time>4)
		{
			time_flag=0;
			time=0;
			
			USART_SendData(USART3,0xA5);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
			USART_SendData(USART3,0x5A);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
			USART_SendData(USART3,0x04);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
			USART_SendData(USART3,0x80);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
			USART_SendData(USART3,0x03);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
			USART_SendData(USART3,0x00);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
			USART_SendData(USART3,0x01);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
			
			LED0=0;
			
			TIM_Cmd(TIM3, DISABLE);
		}
		else
		{
			time_flag=1;
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}


////////////////////////////////////////////////////////////////////
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///ʹ��TIM4ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //����ʱ��4�����ж�
	TIM_Cmd(TIM4,ENABLE); //ʹ�ܶ�ʱ��4
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void sendTime()
{
		u8 data_to_send[3] = {0};
		USART_SendData(USART3,0xA5);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x5A);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x0A);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x82);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x00);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x08);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������

		data_to_send[0]=((second/10)<<4)+second%10;
		data_to_send[1]=((minute/10)<<4)+minute%10;
		data_to_send[2]=((hour/10)<<4)+hour%10;

		USART_SendData(USART3,0x21);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x05);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x30);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,0x07);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������

		USART_SendData(USART3,data_to_send[2]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ���������
		USART_SendData(USART3,data_to_send[1]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ��������� 
		USART_SendData(USART3,data_to_send[0]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//ѭ������,ֱ��������� 
}

//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{
		time_s++;
		if(time_s>10)
		{
			sendTime();
			second++;//DS1��ת

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
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //����жϱ�־λ
}

////////////////////////////////////////////////////////////////////


void TIM2_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//TIM2ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�
	
	TIM_Cmd(TIM2,ENABLE);//������ʱ��2
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}

void TIM2_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART3_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIM2�����жϱ�־    
		TIM_Cmd(TIM2, DISABLE);  //�ر�TIM2 
	}	    
}

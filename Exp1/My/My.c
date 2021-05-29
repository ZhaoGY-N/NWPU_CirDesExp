#include "My.h"

extern uint32_t MySysTickBegin,MySysTickNow;
extern uint8_t WaveMode;
extern uint8_t MyAutoMode;
uint8_t MyKey0,MyKey1,MyInputTemp,MyInputLast,MyInput,MyState,MyTickCount5000;

uint8_t MySW1Count=0;
void MyGetInput(void)
{
	static uint8_t MyTickCount20=0;
	MySysTickNow = HAL_GetTick();
	if(MySysTickNow - MySysTickBegin>20)
	{
		MyKey0=HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin);
		MyKey1=HAL_GPIO_ReadPin(SW2_GPIO_Port,SW2_Pin);
		MyInputTemp= (((!MyKey0))+((!MyKey1)*2));
		if(MyInputTemp==MyInputLast)
			MyInput=MyInputTemp;
		else
			MyInputLast=MyInputTemp;
		if(MyTickCount20>250)
		{
			MyTickCount20=0;
			MyTickCount5000=1;
		}
		MyTickCount20++;
		MySysTickBegin=HAL_GetTick();
		
		if(MySW1Count)
		{
			if(MyInput&0x01)
				MySW1Count++;
			else
				MySW1Count=0;
			if(MySW1Count>50)
				MyAutoMode=(!MyAutoMode);
		}
	}
}

void MyLogicScan(void)
{
	if(MyInput!=MyState)
	{
		if(MyInput&0x01)
			MySW1Count=1;
		else
			MySW1Count=0;
		if(MyAutoMode==0)
		{
			if(MyInput&0x02)
			{
				WaveMode++;
				if(WaveMode>=4)
					WaveMode=0;
			}
		}
		MyState=MyInput;
	}
	if(MyAutoMode)
	{
		if(MyTickCount5000)
		{
			WaveMode++;
			if(WaveMode>=4)
				WaveMode=0;
			MyTickCount5000=0;
		}
	}
}

void MyOutPut(void)
{
	switch(WaveMode)
	{
		case 0:
			HAL_GPIO_WritePin(A_GPIO_Port,A_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(B_GPIO_Port,B_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(C_GPIO_Port,C_Pin,GPIO_PIN_SET);
			break;
		case 1:
			HAL_GPIO_WritePin(A_GPIO_Port,A_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(B_GPIO_Port,B_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(C_GPIO_Port,C_Pin,GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(A_GPIO_Port,A_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(B_GPIO_Port,B_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(C_GPIO_Port,C_Pin,GPIO_PIN_RESET);
			break;
		case 3:
			HAL_GPIO_WritePin(A_GPIO_Port,A_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(B_GPIO_Port,B_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(C_GPIO_Port,C_Pin,GPIO_PIN_SET);
			break;
	}
}

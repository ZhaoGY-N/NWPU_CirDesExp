#include "max31865.h"
#include "math.h"
#include "usart.h"
#include "delay.h"
/* MAX31865 初始化 */
void MAX31865_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure ;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = MAX31865_CS|MAX31865_SCLK|MAX31865_SDI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
	GPIO_Init(MAX31865_CONTROL_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31865_SDO|MAX31865_DRDY;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;				//输入
	GPIO_Init(MAX31865_CONTROL_PORT,&GPIO_InitStructure);
	
	MAX31865_CS_SET;
	MAX31865_SCLK_SET;
}


/* MAX31865 写寄存器 
addr:寄存器地址
data:数据
*/
void MAX31865_Write(unsigned char addr, unsigned char data)
{
	unsigned char i;
	
	MAX31865_CS_CLR;
	for(i=0;i<8;i++)  //写地址
	{
		MAX31865_SCLK_CLR;
		if(addr&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		MAX31865_SCLK_SET;
		addr<<=1;
	}
	for(i=0;i<8;i++)  //写数据
	{
		MAX31865_SCLK_CLR;
		if(data&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		MAX31865_SCLK_SET;
		data<<=1;
	}
	MAX31865_CS_SET;
}

/* MAX31865 读寄存器 
addr:寄存器地址
*/
unsigned char MAX31865_Read(unsigned char addr)
{
	unsigned char i;
	unsigned char data=0;
	
	MAX31865_CS_CLR;
	for(i=0;i<8;i++)  //写地址
	{
		MAX31865_SCLK_CLR;
		if(addr&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		MAX31865_SCLK_SET;
		addr<<=1;
	}
	for(i=0;i<8;i++)  //读数据
	{
		MAX31865_SCLK_CLR;
		data<<=1;		
		MAX31865_SCLK_SET;		
		if(MAX31865_SDO_READ) data|=0x01;
		else data|=0x00;
	}
	MAX31865_CS_SET;
	return data;
}

/* MAX31865 配置*/
void MAX31865_Cfg(void)
{
	MAX31865_Write(0x80, 0xD3); //BIAS ON,自动，3线，50HZ  根据文件修改四线还是三线  D3 1101 0011 
}

/* MAX31865 获取温度 */
float MAX31865_GetTemp(void)
{
	unsigned int data;
	float Rt;
	float Rt0 = 100;  //PT100
	float Z1,Z2,Z3,Z4,temp;
	float a = 3.9083e-3;
	float b = -5.775e-7;
	float rpoly;

	while(MAX31865_DRDY_READ);
	
	//MAX31865_Write(0x80, 0xD3);
	data=MAX31865_Read(0x01)<<8;
	data|=MAX31865_Read(0x02);
	data>>=1;  //去掉Fault位
	//printf("Read=0x%02X\r\n",data);
	Rt=(float)data/32768.0*RREF;
//	if(Rt>110)
	Rt=Rt+(Rt-50)*0.0022+0.55;
	//printf("Rt=%.1f\r\n",Rt);
	
	Z1 = -a;
  Z2 = a*a-4*b;
  Z3 = 4*b/Rt0;
  Z4 = 2*b;

  temp = Z2+Z3*Rt;
  temp = (sqrt(temp)+Z1)/Z4;

  if(temp>=0) return temp;

  rpoly = Rt;
  temp = -242.02;
  temp += 2.2228 * rpoly;
  rpoly *= Rt;  // square
  temp += 2.5859e-3 * rpoly;
  rpoly *= Rt;  // ^3
  temp -= 4.8260e-6 * rpoly;
  rpoly *= Rt;  // ^4
  temp -= 2.8183e-8 * rpoly;
  rpoly *= Rt;  // ^5
  temp += 1.5243e-10 * rpoly;

  return temp;
}

float tempCorrection(float temp_1)
{
	
		if(temp_1>-127&&temp_1<-123)
			temp_1=-125.0;  //-125.15
		if(temp_1>-102&&temp_1<-98)
			temp_1=-100.6;  //-100.63
		if(temp_1>-77&&temp_1<-73)
			temp_1=-75.8;  //-75.828
		if(temp_1>-52&&temp_1<-48)
			temp_1=-50.8;  //-50.77
	  if(temp_1>-27&&temp_1<-23)
			temp_1=-24.4;  //-25.49
		if(temp_1>-1.5&&temp_1<1.5)
			temp_1=0.0;  //0
		if(temp_1>24&&temp_1<28)
			temp_1=25.7;  //25.68
		if(temp_1>50&&temp_1<54)
			temp_1=51.6;  //51.57
		if(temp_1>76&&temp_1<80)
			temp_1=77.7;  //77.65
		if(temp_1>102&&temp_1<106)
			temp_1=104.0;  //103.94
		if(temp_1>128&&temp_1<132)
			temp_1=130.4;  //130.45
	  if(temp_1>156&&temp_1<158)
			temp_1=157.1;  //157.17
		if(temp_1>183&&temp_1<185)
			temp_1=184.2;  //184.11
		if(temp_1>209&&temp_1<213)
			temp_1=211.2;  //211.29
		if(temp_1>237&&temp_1<241)
			temp_1=238.6;  //238.70
		if(temp_1>264&&temp_1<268)
			temp_1=266.3;  //266.35
		return temp_1;
}


 
 #include "C8051F340.h"
#include <stdio.h>

#include "Usb_FunDec.h"
#include "Usb_Parameter.h"
#include "Usb_Register.h"
#include "Usb_Request.h"

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long

sbit OutBusy = P1^0;
unsigned char re;
unsigned char a='0';
int t=0;
void SystemClkInit(void)
{
	int i = 0;

	OSCICN |= 0x11;		//默认值为8分频，切换到12M全速运行
    CLKMUL    = 0x80;		//允许4倍乘法器
    for (i = 0; i < 20; i++);    // Wait 5us for initialization
    CLKMUL    |= 0xC0;			  //初始化4倍乘法器
    while ((CLKMUL & 0x20) == 0);				   //等待RDY位
    CLKSEL    = 0x03;						  //系统时钟选择48Mhz
//    CLKSEL    = 0x02;  			//系统时钟选24Mhz
//    CLKSEL    = 0x00;  			//系统时钟选内部12Mhz
}

void  TimerInit(void)
{
	TMOD = 0x21;		//定时器1工作在8位自动重载，0工作在16位模式
	CKCON = 0x00;		//定时器0和定时器1使用12分频系统时钟
	TH1 = 0x30;			//9600bps	 4000000/208/2 = 9615
	TL1 = 0x30;			//9600bps	 4000000/208/2 = 9615
	TR1 = 1;
}

#define c_TH0_1Ms_Value 0xF0
//#define c_TL0_1Ms_Value 0x60		//理论计算值
#define c_TL0_1Ms_Value 0x80		//修正误差

//65536 -4000 =61536=0xF060
//毫秒级延时函数
//备注：以7.373M为基准
void Delay(uchar ucCount)
{
	while(ucCount--)
	{
			
		TH0 = c_TH0_1Ms_Value;
		TL0 = c_TL0_1Ms_Value;
		TR0 =1;
		while(TF0 == 0);
		TR0 = 0;
		TF0 = 0;
	}
}



void UART0Init(void)
{
	SCON0 = 0x10;
	TI0 = 1;
}

#if 1
void WriteOneByte(unsigned char ucWriteData)
{
	while(TI0 == 0);
	TI0 = 0;
	SBUF0 = ucWriteData;

}

unsigned char recieve(void)
{
	while(RI0 == 0);
	RI0 = 0;
	re = SBUF0;
	return re;
}

#endif

void PORT_Init (void)
{
	
	P0MDIN |= 0xFF;			//1配置为数字，0配置为模拟
	P1MDIN |= 0xFF;			//1配置为数字
//	P2MDIN |= 0xFF;
	P3MDIN |= 0xff;		  	//1配置为数字，0配置为模拟
	P4MDIN = 0xff;
//
//	P0MDOUT = 0x7E;			//p0.0 TDI_IN IS INPUT
//	P1MDOUT = 0x5D;			//p1.1,p1.5,P1.7 INPUT
//	P2MDOUT = 0xFF;
    P1MDOUT   = 0xfb;	  	//P1.2 INPUT

	P0MDOUT = 0xff;		   //1配置为推挽输出

	P3MDOUT = 0xff;		   //1配置为推挽输出
	P4MDOUT = 0xFF;			//配置为开漏输出
//	P0 |= 0x01;
//	P1 |= 0XA2;                                       
   	XBR0 |= 0x01;		//UART0连接到端口
   	XBR1    = 0x40;     // Enable crossbar and enable  weak pull-ups
}

#if 0
void Delay(void)
{
	ulong ulCount;

	for(ulCount= 0; ulCount<0x1ffff; ulCount++)
	{
		;
	}
}
#endif

void TestNandFlash(void);
void NandFlash_Reset(void);		                                     //flash reset

 void main(void)
 {
    PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer



   	SystemClkInit();                 // enable)
	PORT_Init();
	TimerInit();
   	UART0Init();
	
	 
//	 WriteOneByte('K');
	
	printf("Hello world\r\n");
	NandFlash_Reset();
	Usb_Init();					// USB初始化
	EA=1;  				

 	while(1)
	{
		OutBusy = 1;
//		P4 = 0xff;
		Delay(250);
		Delay(250);
		Delay(250);
		Delay(250);
		OutBusy = 0;
//		P4 = 0x00;
		
		Delay(250);
		Delay(250);
		Delay(250);
		Delay(250);

//		printf("OK!\r\n");
//		TestNandFlash();
	}

 }
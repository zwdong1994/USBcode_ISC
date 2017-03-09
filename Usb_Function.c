#include "c8051F340.h"
#include "stdio.h"
#include "intrins.h"
/////////////////////////////
#include "Usb_FunDec.h"
#include "Usb_Parameter.h"
#include "Usb_Register.h"
#include "Usb_Request.h"
//////////////////////////////////////////////////////////////////////////////////////
void Usb_Init()
{
	UWrite_Byte(POWER, 0x08);       // 异步复位

    UWrite_Byte(IN1IE, 0x03);       // 允许端点0中断和端点1IN中断
    UWrite_Byte(OUT1IE,0x04);		// 允许端点2OUT中断
    UWrite_Byte(CMIE, 0x04);        // 允许复位中断，禁止挂起中断和恢复中断
   	USB0XCN=0xe0;					// USB收发器使能，上拉电阻使能，全速，正常工作方式
	UWrite_Byte(CLKREC,0x80);		// USB时钟恢复使能，正常校准方式，全速方式

	EIE1 |= 0x02;                   // 允许USB中断
	UWrite_Byte(POWER,0x00);		// 禁止ISO，允许USB工作、禁止挂起功能
}
                    
void Fifo_Read(unsigned char addr, unsigned int uNumBytes, unsigned char *pData)
{
   	unsigned int i;
   	USB0ADR  = (addr & 0x3f);        		//设置地址   
   	while(USB0ADR & 0x80)	//判断是否结束
	{}             
   	for(i=0;i< uNumBytes;i++)
   	{
   		USB0ADR |= 0x80; 		//启动读操作                  
        while(USB0ADR & 0x80)	//判断是否结束
		{}          
        pData[i] = USB0DAT;              
   	}
   	USB0ADR = 0;            	//停止读，将地址清零               
}

void Fifo_Write(unsigned char addr, unsigned int uNumBytes, unsigned char *pData)
{
   int i;
   USB0ADR  = addr;        		//设置地址           
   for(i=0;i< uNumBytes;i++)
   {
        while(USB0ADR & 0x80)	//判断是否结束
		{}          
        USB0DAT=pData[i];              
   }
   USB0ADR = 0;            		//地址清零           
                                          
}
///////////////////////////////////////////////////////////////////////////////////////////////////
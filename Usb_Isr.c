#include "c8051F340.h"
#include "stdio.h"
#include "intrins.h"
/////////////////////////////
#include "Usb_FunDec.h"
#include "Usb_Parameter.h"
#include "Usb_Register.h"
#include "Usb_Request.h"

extern unsigned char  Ep_Status[3];   	
extern unsigned char  USB_State;		
extern unsigned char  Bulk_Status;		

void Usb_Isr() interrupt 8
{
    unsigned char bCommon, bIn, bOut;
    URead_Byte(CMINT, bCommon);			//读中断寄存器
    URead_Byte(IN1INT, bIn);
	URead_Byte(OUT1INT, bOut);
	///////////////////////////////////////////////////////////////////////////////////////
	if (bCommon & rbRSUINT)           // 处理恢复中断
    {
//		printf("This is a Resume Interrupt\n");
		Usb_Resume();
    }
    if (bCommon & rbRSTINT)           // 处理复位中断t
    {
//		printf("Reset Interrupt\n");
		Usb_Reset();
    }
	if (bCommon & rbSUSINT)           // 处理挂起中断
    {
//		printf("This is a Susint Interrupt\n");
        Usb_Suspend();
    }
	///////////////////////////////////////////////////////////////////////////////////////
    if (bIn & rbEP0)                  				// 如果是端点0中断
    {
//		printf("This is a EP0 Interrupt\n");
        Handle_Setup();              				// 调用端点0中断处理函数
    }
	///////////////////////////////////////////////////////////////////////////////////////
    if (bIn & rbIN1)                  				// 如果是端点1中断
    { 
//		printf("This is a EdPoint1 Interrupt\n");                                      
		Handle_In1();				  				// 调用端点1中断处理函数
    }
	///////////////////////////////////////////////////////////////////////////////////////
    if (bOut & rbOUT2)                				// 如果是端点2中断
    { 
//		printf("This is a EdPoint2 Interrupt\n");                                      
        Handle_Out2();				  				// 调用端点2中断处理函数
    }   

}

void Usb_Reset()
{
   	USB_State = DEV_DEFAULT;             // 设备到默认状态
   	UWrite_Byte(POWER, 0x00);            
   	Ep_Status[0] = EP_IDLE;              // 端点到空闲状态
   	Ep_Status[1] = EP_HALT;
   	Ep_Status[2] = EP_HALT;
	Bulk_Status  = BULK_IDLE;			 // 批量传输端点状态到空闲状态
}
void Usb_Suspend()
{
	// 此开发中没涉及到相关的操作
}
void Usb_Resume()
{
	// 此开发中没涉及到相关的操作
}

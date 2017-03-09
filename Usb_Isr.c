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
    URead_Byte(CMINT, bCommon);			//���жϼĴ���
    URead_Byte(IN1INT, bIn);
	URead_Byte(OUT1INT, bOut);
	///////////////////////////////////////////////////////////////////////////////////////
	if (bCommon & rbRSUINT)           // ����ָ��ж�
    {
//		printf("This is a Resume Interrupt\n");
		Usb_Resume();
    }
    if (bCommon & rbRSTINT)           // ����λ�ж�t
    {
//		printf("Reset Interrupt\n");
		Usb_Reset();
    }
	if (bCommon & rbSUSINT)           // ��������ж�
    {
//		printf("This is a Susint Interrupt\n");
        Usb_Suspend();
    }
	///////////////////////////////////////////////////////////////////////////////////////
    if (bIn & rbEP0)                  				// ����Ƕ˵�0�ж�
    {
//		printf("This is a EP0 Interrupt\n");
        Handle_Setup();              				// ���ö˵�0�жϴ�����
    }
	///////////////////////////////////////////////////////////////////////////////////////
    if (bIn & rbIN1)                  				// ����Ƕ˵�1�ж�
    { 
//		printf("This is a EdPoint1 Interrupt\n");                                      
		Handle_In1();				  				// ���ö˵�1�жϴ�����
    }
	///////////////////////////////////////////////////////////////////////////////////////
    if (bOut & rbOUT2)                				// ����Ƕ˵�2�ж�
    { 
//		printf("This is a EdPoint2 Interrupt\n");                                      
        Handle_Out2();				  				// ���ö˵�2�жϴ�����
    }   

}

void Usb_Reset()
{
   	USB_State = DEV_DEFAULT;             // �豸��Ĭ��״̬
   	UWrite_Byte(POWER, 0x00);            
   	Ep_Status[0] = EP_IDLE;              // �˵㵽����״̬
   	Ep_Status[1] = EP_HALT;
   	Ep_Status[2] = EP_HALT;
	Bulk_Status  = BULK_IDLE;			 // ��������˵�״̬������״̬
}
void Usb_Suspend()
{
	// �˿�����û�漰����صĲ���
}
void Usb_Resume()
{
	// �˿�����û�漰����صĲ���
}

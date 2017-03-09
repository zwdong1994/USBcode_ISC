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
	UWrite_Byte(POWER, 0x08);       // �첽��λ

    UWrite_Byte(IN1IE, 0x03);       // ����˵�0�жϺͶ˵�1IN�ж�
    UWrite_Byte(OUT1IE,0x04);		// ����˵�2OUT�ж�
    UWrite_Byte(CMIE, 0x04);        // ����λ�жϣ���ֹ�����жϺͻָ��ж�
   	USB0XCN=0xe0;					// USB�շ���ʹ�ܣ���������ʹ�ܣ�ȫ�٣�����������ʽ
	UWrite_Byte(CLKREC,0x80);		// USBʱ�ӻָ�ʹ�ܣ�����У׼��ʽ��ȫ�ٷ�ʽ

	EIE1 |= 0x02;                   // ����USB�ж�
	UWrite_Byte(POWER,0x00);		// ��ֹISO������USB��������ֹ������
}
                    
void Fifo_Read(unsigned char addr, unsigned int uNumBytes, unsigned char *pData)
{
   	unsigned int i;
   	USB0ADR  = (addr & 0x3f);        		//���õ�ַ   
   	while(USB0ADR & 0x80)	//�ж��Ƿ����
	{}             
   	for(i=0;i< uNumBytes;i++)
   	{
   		USB0ADR |= 0x80; 		//����������                  
        while(USB0ADR & 0x80)	//�ж��Ƿ����
		{}          
        pData[i] = USB0DAT;              
   	}
   	USB0ADR = 0;            	//ֹͣ��������ַ����               
}

void Fifo_Write(unsigned char addr, unsigned int uNumBytes, unsigned char *pData)
{
   int i;
   USB0ADR  = addr;        		//���õ�ַ           
   for(i=0;i< uNumBytes;i++)
   {
        while(USB0ADR & 0x80)	//�ж��Ƿ����
		{}          
        USB0DAT=pData[i];              
   }
   USB0ADR = 0;            		//��ַ����           
                                          
}
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "c8051F340.h"
#include "stdio.h"
#include "intrins.h"
/////////////////////////////
#include "Usb_Descreptor.h"
#include "Usb_FunDec.h"
#include "Usb_Parameter.h"
#include "Usb_Register.h"
#include "Usb_Request.h"

//ȫ�ֱ���
static unsigned int   DataSize;                  		// ��Ҫ�˵�0���͵����ݴ�С
static unsigned int   DataSent;                  		// �˵�0�Ѿ����͵����ݴ�С
static unsigned char  *DataPtr;                  		// �˵�0�������ݵ�ָ��
static unsigned char  ONES_PACKET[2] = {0x01, 0x00};    // ��Get_Status��������Ӧ�õ�����
static unsigned char  ZERO_PACKET[2] = {0x00, 0x00};    // ��Get_Status��������Ӧ�õ�����

unsigned char  Ep_Status[3]	={EP_IDLE,EP_STALL,EP_STALL};   // ����˵��״̬
unsigned char  USB_State	=DEV_ATTACHED;	                // �����豸��״̬
struct Usb_Setup						// Setup�׶��յ���8�ֽڵ�����˴�����Ϊ�ṹ
{
	unsigned char bmRequestType;
	unsigned char bRequest;
	unsigned char wValue[2];
	unsigned char wIndex[2];
	unsigned char wLength[2];
}Setup;

///////////////////////////////////////////////////////////////////////////////////////////////
void Handle_Setup()						//�˵�0�жϴ�����
{
    unsigned char ControlReg=0;         // ��Ŷ˵�0����/״̬�Ĵ���ֵ�ı���
    UWrite_Byte(INDEX, 0);           	// ָ��˵�0
    URead_Byte(E0CSR, ControlReg);  	// ���˵�0���ƼĴ���
    if (ControlReg & rbSUEND)           // ����ϴ����ݽ׶���ǰ����
    {  
		UWrite_Byte(E0CSR, rbDATAEND);  // ��ǰ���ݴ���                                
        UWrite_Byte(E0CSR, rbSSUEND);   // ���SUEND��־
        Ep_Status[0] = EP_IDLE;         // �˵㷵�ص�����״̬
    }
    if (ControlReg & rbSTSTL)           // ����ϴδ���������������STALLʹ������ǰ����
    {                                  
        UWrite_Byte(E0CSR, 0);          // ���STALL
        Ep_Status[0] = EP_IDLE;			// �˵�0���ص�����״̬
    }

	if (ControlReg & rbOPRDY)           // ������յ�������ݰ�
	{
	    if (Ep_Status[0] == EP_IDLE)    // ����˵�0����
		{
			Fifo_Read(FIFO_EP0,8,(unsigned char *)&Setup);		// ��ȡ�������ݰ���8���ֽ�
			switch(Setup.bRequest)  				    // �ж���������   
			{                              
				case GET_STATUS:		Get_Status();		break;				     	
				case CLEAR_FEATURE:		Clear_Feature();	break;
				case SET_FEATURE:    	Set_Feature(); 		break;   
				case SET_ADDRESS:		Set_Address();	 	break;	
				case GET_DESCRIPTOR:	Get_Descriptor();	break;	
				case GET_CONFIGURATION:	Get_Configuration();break;
				case SET_CONFIGURATION:	Set_Configuration();break;
				case GET_INTERFACE:		Get_Interface();	break;
				case SET_INTERFACE:		Set_Interface();	break;		
				case GET_MAXLUN:		Get_Maxlun();		break;
				default:				Force_Stall();		break;			
	     	}	
			                 
		/////////////////////////////////////////////////////////////////////////////////////////////
		}
		else if(Ep_Status[0]==EP_RX)	// �����OUT״̬���˳����в���Ӷ˵�0����������Ϣ������û��صĲ���
		{}
	}
	if (Ep_Status[0] == EP_TX)			// �����IN״̬
	{
		    unsigned char ControlReg=0,TempReg=0;	// ��żĴ���ֵ�ı���                                      
			URead_Byte(E0CSR, ControlReg);			// ���˵�0���ƼĴ���                              
			TempReg = rbINPRDY;                     // �������׼����           
			if (DataSize >= EP0_PACKET_SIZE)		// ������������ݵĳ��ȴ��ڶ˵�0����
			{
				Fifo_Write(FIFO_EP0, EP0_PACKET_SIZE,DataPtr);	// �������ݰ�
		        DataPtr  += EP0_PACKET_SIZE;                    // ��������ָ��
		        DataSize -= EP0_PACKET_SIZE;                    // ���´����͵�������
		        DataSent += EP0_PACKET_SIZE;                    // �����ѷ��͵�������
		    }
			else                        			// ������������ݵĳ���С�ڶ˵�0����
			{
				Fifo_Write(FIFO_EP0, DataSize,DataPtr);	// ����ʣ�������       
		        TempReg |= rbDATAEND;                  	// ָʾ���ݷ����걸������״̬�׶�		               
		        Ep_Status[0] = EP_IDLE;                 // �˵㷵�ص�����״̬               
			}
			if (DataSent == (Setup.wLength[0]+256*Setup.wLength[1]))	//������͵������������Ƕ˵�0��������
			                            
		    {
		        TempReg |= rbDATAEND;    // ָʾ���ݷ����걸������״̬�׶�
		        Ep_Status[0] = EP_IDLE;  // �˵㷵�ص�����״̬
		    }
		        UWrite_Byte(E0CSR, TempReg);  // д�˵�0���ƼĴ���
    }
}

void Force_Stall()
{
	UWrite_Byte(INDEX, 0);
    UWrite_Byte(E0CSR, rbSDSTL);       // ����STALL
    Ep_Status[0] = EP_STALL;           // �˵�0��STALL״̬��ֱ���´θ�λ
}

//�����׼����ĺ���
void Get_Status()
{	
   	switch(Setup.bmRequestType)                  
   	{
      	case IN_DEVICE:                          	// ������豸��״̬
           	DataPtr = ZERO_PACKET;               	// ���� 0x00, ���߹��磬��֧��Զ�̻���
           	DataSize = 2;                        	
           	break;      
      	case IN_INTERFACE:                       	// �ӿ�״̬
           	DataPtr = ZERO_PACKET;               	// �ӿ�״̬��Զ����0
           	DataSize = 2;         
           	break;  
      	case IN_ENDPOINT:                        	// �˵�״̬
           	if (Setup.wIndex[0] == IN_EP1)          // ����Ƕ˵�1
			{
				if (Ep_Status[1] == EP_HALT)
				{                               // ����˵�1��EN_HALT״̬
					DataPtr = ONES_PACKET;		// ����ONES_PACKET,ָʾ�˵�1��HALT״̬
	                DataSize = 2;
	            }
	            else
	            {
	                DataPtr = ZERO_PACKET;		// ������ZERO_PACKET,ָʾ�˵�1�ڿ���״̬
	                DataSize = 2;
	            }
			}
	        if (Setup.wIndex[0] == OUT_EP2)
	        {                                 
				if (Ep_Status[2] == EP_HALT) 
				{
					DataPtr = ONES_PACKET;
	                DataSize = 2;
				}
	            else
				{
					DataPtr = ZERO_PACKET;
					DataSize = 2;
				}
			}
            break;
      default:	Force_Stall();	break;           
	}
   if (Ep_Status[0] != EP_STALL)
   {
   		UWrite_Byte(E0CSR, rbSOPRDY);                       
      	Ep_Status[0] = EP_TX;                     // �˵�0��EP_TX״̬
      	DataSent = 0;								// �ѷ�������������
   }
}

void Clear_Feature()
{
	if ((Setup.bmRequestType & 0x02)&&			// ����Ƕ˵�
        (Setup.wValue[0] == ENDPOINT_HALT)  && 	// ������Щ�˵��״̬��EP_HALT
       ((Setup.wIndex[0] == IN_EP1) ||         	// ����ѡ����Ƕ˵�1 IN
        (Setup.wIndex[0] == OUT_EP2)))         	// ��˵�2 OUT 
	{
         if (Setup.wIndex[0] == IN_EP1) 
         {
            UWrite_Byte (INDEX, 1);             // ʹtoggle��λΪ��
            UWrite_Byte (EINCSRL, rbInCLRDT);       
            Ep_Status[1] = EP_IDLE;             // �˵�1������״̬                    
         }
         else
         {
            UWrite_Byte (INDEX, 2);         
            UWrite_Byte (EOUTCSRL, rbOutCLRDT);         
            Ep_Status[2] = EP_IDLE;             
         }
	}
	else
    { 
         Force_Stall();                         
    }
   if (Ep_Status[0] != EP_STALL)
   {
   		UWrite_Byte(E0CSR, rbSOPRDY);
		UWrite_Byte(E0CSR, rbDATAEND);                       
   }
}
void Set_Feature()
{
	if ((Setup.bmRequestType  & 0x02) && 		// ����ͬ��
        (Setup.wValue[0] == ENDPOINT_HALT) &&  
       ((Setup.wIndex[0] == IN_EP1)      || 
        (Setup.wIndex[0] == OUT_EP2)))
	{
         if (Setup.wIndex[0] == IN_EP1) 
         {
            UWrite_Byte (INDEX, 1);         	  
            UWrite_Byte (EINCSRL, rbInSDSTL);   // rbInSDSTL=1,����STALL��Ϊ��IN���Ƶ���Ӧ��
												// �ڶ˵�1�жϴ������н�rbInSDSTl��rbInSTSTL����    
            Ep_Status[1] = EP_HALT;             // �˵�1��HALT״̬                     
         }
         else
         {
            UWrite_Byte(INDEX, 2);         		
            UWrite_Byte (EOUTCSRL, rbOutSDSTL); // ͬ��       
            Ep_Status[2] = EP_HALT;  		    
         }
	}
	else
    { 
		Force_Stall();                         	
    }  
   if (Ep_Status[0] != EP_STALL)
   {
   		UWrite_Byte(E0CSR, rbSOPRDY);
		UWrite_Byte(E0CSR, rbDATAEND);                       
   }
}
void Set_Address()
{
   	if (Setup.wValue[0] != 0) 
   	{
      	USB_State = DEV_ADDRESS;            // �յ���Ϊ0�ĵ�ַ���豸���ڵ�ַ״̬���˴δ����
      	UWrite_Byte(FADDR, Setup.wValue[0]);// ״̬�׶ν������µ�ַ��������
      	Ep_Status[0] = EP_IDLE;

   	}
   	else 
   	{
     	USB_State = DEV_DEFAULT;            // �յ��ĵ�ַΪ0���豸�Ծɴ���Ĭ��״̬
   	}
   	UWrite_Byte(E0CSR, rbSOPRDY);
	UWrite_Byte(E0CSR, rbDATAEND);                       
                                              
}
void Get_Descriptor()
{
	unsigned int In_DataSize=0;			// Ҫ���͵���������
	In_DataSize=Setup.wLength[0]+Setup.wLength[1]*256;
	switch(Setup.wValue[1])             // �ж�������������
    {                                            
		case DSC_DEVICE:         		// �豸������  
	       	DataPtr  = DeviceDesc;
			if(In_DataSize>18)
			{DataSize = 18;	}       
			else
			{DataSize = In_DataSize;}
        break;      
		case DSC_CONFIG:				// ����������
        	DataPtr  = ConfigDesc;
			if(In_DataSize==0x09)
			{
			 	DataSize=0x09;
			}
			else if(In_DataSize==0x20)
			{
			 	DataSize=0x20;
			}
			else if(In_DataSize==0xff)
			{
			 	DataSize=0x20;
			}
        break;
		case DSC_STRING:				// �ַ���������
			 DataPtr=&StringDes[Setup.wValue[0]];
			 DataSize=In_DataSize;
			 break;
		default:	         break;
   	}
	UWrite_Byte(E0CSR, rbSOPRDY);
    Ep_Status[0] = EP_TX;                      // �˵�0�ڷ���״̬
    DataSent = 0;                              
}
void Get_Configuration()
{
   if (USB_State == DEV_CONFIGURED)   // If the device is configured, then return value 0x01
   {                                  // since this software only supports one configuration
         DataPtr  = ONES_PACKET;
         DataSize = 1;
   }
   if (USB_State == DEV_ADDRESS)      // If the device is in address state, it is not
   {                                  // configured, so return 0x00
         DataPtr  = ZERO_PACKET;
         DataSize = 1;
   }
   if (Ep_Status[0] != EP_STALL)
   {
   		UWrite_Byte(E0CSR, rbSOPRDY);
        Ep_Status[0] = EP_TX;                 
        DataSent = 0;                         
   }
}
void Set_Configuration()
{
   if (Setup.wValue[0] > 0)                  // Any positive configuration request
   {                                         // results in configuration being set to 1
         USB_State = DEV_CONFIGURED;
		 ///////////////////////////////////////////////////////////////////////////////////
         UWrite_Byte(INDEX, 1);     
         UWrite_Byte(EINCSRH,0xa0); // ˫���塢ISO��FIFO�ָ��ǿ������Toggle��ֹ��IN�˵�
		 Ep_Status[1]=EP_IDLE;
		  
         UWrite_Byte(INDEX, 2);            
         UWrite_Byte(EOUTCSRH, 0x80);      // ˫�����ISO��ֹ��OUT�˵�
		 Ep_Status[2]=EP_IDLE; 
		 ///////////////////////////////////////////////////////////////////////////////////
         UWrite_Byte(INDEX, 0);           
   }
   else
   {
         USB_State = DEV_ADDRESS;         // �豸�Ծ��ڵ�ַ״̬
		 Ep_Status[1]=EP_STALL;
		 Ep_Status[2]=EP_STALL;
   }
   if (Ep_Status[0] != EP_STALL)
   {
   		UWrite_Byte(E0CSR, rbSOPRDY);
		UWrite_Byte(E0CSR, rbDATAEND);                
   }
}
void Get_Interface()
{
	DataPtr = ZERO_PACKET;            
	DataSize = 1;
    if (Ep_Status[0] != EP_STALL)
    { 
   		UWrite_Byte(E0CSR, rbSOPRDY);                  
        Ep_Status[0] = EP_TX;          
        DataSent = 0;
    }
}
void Set_Interface()
{
   	UWrite_Byte(E0CSR, rbSOPRDY);
	UWrite_Byte(E0CSR, rbDATAEND);             
}

void Get_Maxlun()			// U��֧��һ���߼���Ԫ������1����
{
	DataPtr = ZERO_PACKET;            
	DataSize = 1;
    if (Ep_Status[0] != EP_STALL)
    {                       
        Ep_Status[0] = EP_TX;           
        DataSent = 0;
    }
   	UWrite_Byte(E0CSR, rbSOPRDY);
}
////////////////////////////////////////////////////////////////////////////////////////////////////


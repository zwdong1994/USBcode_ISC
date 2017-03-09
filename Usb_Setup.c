#include "c8051F340.h"
#include "stdio.h"
#include "intrins.h"
/////////////////////////////
#include "Usb_Descreptor.h"
#include "Usb_FunDec.h"
#include "Usb_Parameter.h"
#include "Usb_Register.h"
#include "Usb_Request.h"

//全局变量
static unsigned int   DataSize;                  		// 需要端点0发送的数据大小
static unsigned int   DataSent;                  		// 端点0已经发送的数据大小
static unsigned char  *DataPtr;                  		// 端点0发送数据的指针
static unsigned char  ONES_PACKET[2] = {0x01, 0x00};    // 在Get_Status等命令中应用的数组
static unsigned char  ZERO_PACKET[2] = {0x00, 0x00};    // 在Get_Status等命令中应用的数组

unsigned char  Ep_Status[3]	={EP_IDLE,EP_STALL,EP_STALL};   // 定义端点的状态
unsigned char  USB_State	=DEV_ATTACHED;	                // 定义设备的状态
struct Usb_Setup						// Setup阶段收到的8字节的命令，此处定义为结构
{
	unsigned char bmRequestType;
	unsigned char bRequest;
	unsigned char wValue[2];
	unsigned char wIndex[2];
	unsigned char wLength[2];
}Setup;

///////////////////////////////////////////////////////////////////////////////////////////////
void Handle_Setup()						//端点0中断处理函数
{
    unsigned char ControlReg=0;         // 存放端点0控制/状态寄存器值的变量
    UWrite_Byte(INDEX, 0);           	// 指向端点0
    URead_Byte(E0CSR, ControlReg);  	// 读端点0控制寄存器
    if (ControlReg & rbSUEND)           // 如果上次数据阶段提前结束
    {  
		UWrite_Byte(E0CSR, rbDATAEND);  // 当前数据传完                                
        UWrite_Byte(E0CSR, rbSSUEND);   // 清除SUEND标志
        Ep_Status[0] = EP_IDLE;         // 端点返回到空闲状态
    }
    if (ControlReg & rbSTSTL)           // 如果上次传输因错误而发送了STALL使传输提前结束
    {                                  
        UWrite_Byte(E0CSR, 0);          // 清除STALL
        Ep_Status[0] = EP_IDLE;			// 端点0返回到空闲状态
    }

	if (ControlReg & rbOPRDY)           // 如果接收到输出数据包
	{
	    if (Ep_Status[0] == EP_IDLE)    // 如果端点0空闲
		{
			Fifo_Read(FIFO_EP0,8,(unsigned char *)&Setup);		// 读取设置数据包，8个字节
			switch(Setup.bRequest)  				    // 判断请求类型   
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
		else if(Ep_Status[0]==EP_RX)	// 如果在OUT状态，此程序中不会从端点0接收其它信息，所以没相关的操作
		{}
	}
	if (Ep_Status[0] == EP_TX)			// 如果在IN状态
	{
		    unsigned char ControlReg=0,TempReg=0;	// 存放寄存器值的变量                                      
			URead_Byte(E0CSR, ControlReg);			// 读端点0控制寄存器                              
			TempReg = rbINPRDY;                     // 输出数据准备好           
			if (DataSize >= EP0_PACKET_SIZE)		// 如果待发送数据的长度大于端点0长度
			{
				Fifo_Write(FIFO_EP0, EP0_PACKET_SIZE,DataPtr);	// 发送数据包
		        DataPtr  += EP0_PACKET_SIZE;                    // 更新数据指针
		        DataSize -= EP0_PACKET_SIZE;                    // 更新待发送的数据量
		        DataSent += EP0_PACKET_SIZE;                    // 更新已发送的数据量
		    }
			else                        			// 如果待发送数据的长度小于端点0长度
			{
				Fifo_Write(FIFO_EP0, DataSize,DataPtr);	// 发送剩余的数据       
		        TempReg |= rbDATAEND;                  	// 指示数据发送完备，进入状态阶段		               
		        Ep_Status[0] = EP_IDLE;                 // 端点返回到空闲状态               
			}
			if (DataSent == (Setup.wLength[0]+256*Setup.wLength[1]))	//如果发送的数据量正好是端点0的整数倍
			                            
		    {
		        TempReg |= rbDATAEND;    // 指示数据发送完备，进入状态阶段
		        Ep_Status[0] = EP_IDLE;  // 端点返回到空闲状态
		    }
		        UWrite_Byte(E0CSR, TempReg);  // 写端点0控制寄存器
    }
}

void Force_Stall()
{
	UWrite_Byte(INDEX, 0);
    UWrite_Byte(E0CSR, rbSDSTL);       // 发送STALL
    Ep_Status[0] = EP_STALL;           // 端点0到STALL状态，直到下次复位
}

//处理标准请求的函数
void Get_Status()
{	
   	switch(Setup.bmRequestType)                  
   	{
      	case IN_DEVICE:                          	// 如果是设备的状态
           	DataPtr = ZERO_PACKET;               	// 发送 0x00, 总线供电，不支持远程唤醒
           	DataSize = 2;                        	
           	break;      
      	case IN_INTERFACE:                       	// 接口状态
           	DataPtr = ZERO_PACKET;               	// 接口状态永远返回0
           	DataSize = 2;         
           	break;  
      	case IN_ENDPOINT:                        	// 端点状态
           	if (Setup.wIndex[0] == IN_EP1)          // 如果是端点1
			{
				if (Ep_Status[1] == EP_HALT)
				{                               // 如果端点1在EN_HALT状态
					DataPtr = ONES_PACKET;		// 发送ONES_PACKET,指示端点1在HALT状态
	                DataSize = 2;
	            }
	            else
	            {
	                DataPtr = ZERO_PACKET;		// 否则发送ZERO_PACKET,指示端点1在空闲状态
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
      	Ep_Status[0] = EP_TX;                     // 端点0在EP_TX状态
      	DataSent = 0;								// 已发送数据量清零
   }
}

void Clear_Feature()
{
	if ((Setup.bmRequestType & 0x02)&&			// 如果是端点
        (Setup.wValue[0] == ENDPOINT_HALT)  && 	// 并且这些端点的状态是EP_HALT
       ((Setup.wIndex[0] == IN_EP1) ||         	// 并且选择的是端点1 IN
        (Setup.wIndex[0] == OUT_EP2)))         	// 或端点2 OUT 
	{
         if (Setup.wIndex[0] == IN_EP1) 
         {
            UWrite_Byte (INDEX, 1);             // 使toggle复位为零
            UWrite_Byte (EINCSRL, rbInCLRDT);       
            Ep_Status[1] = EP_IDLE;             // 端点1到空闲状态                    
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
	if ((Setup.bmRequestType  & 0x02) && 		// 意义同上
        (Setup.wValue[0] == ENDPOINT_HALT) &&  
       ((Setup.wIndex[0] == IN_EP1)      || 
        (Setup.wIndex[0] == OUT_EP2)))
	{
         if (Setup.wIndex[0] == IN_EP1) 
         {
            UWrite_Byte (INDEX, 1);         	  
            UWrite_Byte (EINCSRL, rbInSDSTL);   // rbInSDSTL=1,发送STALL作为对IN令牌的响应，
												// 在端点1中断处理函数中将rbInSDSTl和rbInSTSTL清零    
            Ep_Status[1] = EP_HALT;             // 端点1到HALT状态                     
         }
         else
         {
            UWrite_Byte(INDEX, 2);         		
            UWrite_Byte (EOUTCSRL, rbOutSDSTL); // 同上       
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
      	USB_State = DEV_ADDRESS;            // 收到不为0的地址，设备处于地址状态，此次传输的
      	UWrite_Byte(FADDR, Setup.wValue[0]);// 状态阶段结束后新地址将起作用
      	Ep_Status[0] = EP_IDLE;

   	}
   	else 
   	{
     	USB_State = DEV_DEFAULT;            // 收到的地址为0，设备仍旧处于默认状态
   	}
   	UWrite_Byte(E0CSR, rbSOPRDY);
	UWrite_Byte(E0CSR, rbDATAEND);                       
                                              
}
void Get_Descriptor()
{
	unsigned int In_DataSize=0;			// 要发送的数据总量
	In_DataSize=Setup.wLength[0]+Setup.wLength[1]*256;
	switch(Setup.wValue[1])             // 判断描述符的类型
    {                                            
		case DSC_DEVICE:         		// 设备描述符  
	       	DataPtr  = DeviceDesc;
			if(In_DataSize>18)
			{DataSize = 18;	}       
			else
			{DataSize = In_DataSize;}
        break;      
		case DSC_CONFIG:				// 配置描述符
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
		case DSC_STRING:				// 字符串描述符
			 DataPtr=&StringDes[Setup.wValue[0]];
			 DataSize=In_DataSize;
			 break;
		default:	         break;
   	}
	UWrite_Byte(E0CSR, rbSOPRDY);
    Ep_Status[0] = EP_TX;                      // 端点0在发送状态
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
         UWrite_Byte(EINCSRH,0xa0); // 双缓冲、ISO、FIFO分割和强制数据Toggle禁止，IN端点
		 Ep_Status[1]=EP_IDLE;
		  
         UWrite_Byte(INDEX, 2);            
         UWrite_Byte(EOUTCSRH, 0x80);      // 双缓冲和ISO禁止，OUT端点
		 Ep_Status[2]=EP_IDLE; 
		 ///////////////////////////////////////////////////////////////////////////////////
         UWrite_Byte(INDEX, 0);           
   }
   else
   {
         USB_State = DEV_ADDRESS;         // 设备仍旧在地址状态
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

void Get_Maxlun()			// U盘支持一个逻辑单元，返回1即可
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


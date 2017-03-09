#ifndef _USB_FUNDEC_H_
#define _USB_FUNDEC_H_

// USB寄存器读写宏定义
#define URead_Byte(addr,target) USB0ADR = (0x80 | addr); while(USB0ADR & 0x80); target = USB0DAT
#define UWrite_Byte(addr,num)  USB0ADR = addr; USB0DAT = num; while(USB0ADR & 0x80)

void Usb_Init(void);  		//USB初始化函数
void Fifo_Read(unsigned char addr, unsigned int uNumBytes, unsigned char *pData);	//读FIFO
//addr为FIFO地址，uNumBytes为数据量，*pData为数据指针，以下同
void Fifo_Write(unsigned char addr, unsigned int uNumBytes, unsigned char *pData);	//写FIFO


void Usb_Isr(void);				//USB中断处理函数
void Usb_Resume(void);          // USB恢复
void Usb_Reset(void);           // USB复位
void Usb_Suspend(void);         // USB挂起
void Handle_Setup(void);		//处理端点0中断
void Handle_In1(void);		    //端点1中断处理函数
void Handle_Out2(void);		    //端点2中断处理函数

//////////////////////////////////////////////////////////////////////////////////////////
//USB标准请求处理函数
void Get_Status(void);                      
void Clear_Feature(void);
void Set_Feature(void);
void Set_Address(void);
void Get_Descriptor(void);
void Get_Configuration(void);
void Set_Configuration(void);
void Get_Interface(void);
void Set_Interface(void);
void Get_Maxlun(void);		
void Force_Stall(void);         
//////////////////////////////////////////////////////////////////////////////////////////
//SCSI命令处理函数
void SCSI_Inquiry(void);
void SCSI_Mode_Sense(void);
void SCSI_Read10(void);
void SCSI_Read_Capacity(void);
void SCSI_Test_Unit_Ready(void);
void SCSI_Verify(void);
void SCSI_Write10(void);
void SCSI_Medium_Removal(void);
void SCSI_Reserved(void);
void SCSI_Vender_Order(void);

/////////////////////////////////////////////////////////////////////////////////////////
#endif

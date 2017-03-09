#ifndef _USB_FUNDEC_H_
#define _USB_FUNDEC_H_

// USB�Ĵ�����д�궨��
#define URead_Byte(addr,target) USB0ADR = (0x80 | addr); while(USB0ADR & 0x80); target = USB0DAT
#define UWrite_Byte(addr,num)  USB0ADR = addr; USB0DAT = num; while(USB0ADR & 0x80)

void Usb_Init(void);  		//USB��ʼ������
void Fifo_Read(unsigned char addr, unsigned int uNumBytes, unsigned char *pData);	//��FIFO
//addrΪFIFO��ַ��uNumBytesΪ��������*pDataΪ����ָ�룬����ͬ
void Fifo_Write(unsigned char addr, unsigned int uNumBytes, unsigned char *pData);	//дFIFO


void Usb_Isr(void);				//USB�жϴ�����
void Usb_Resume(void);          // USB�ָ�
void Usb_Reset(void);           // USB��λ
void Usb_Suspend(void);         // USB����
void Handle_Setup(void);		//����˵�0�ж�
void Handle_In1(void);		    //�˵�1�жϴ�����
void Handle_Out2(void);		    //�˵�2�жϴ�����

//////////////////////////////////////////////////////////////////////////////////////////
//USB��׼��������
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
//SCSI�������
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

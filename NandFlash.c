
#include "C8051F340.h"

#include "DataTypeDef.h"
#include "string.h"
#include <stdio.h>
#include <intrins.h>

//sfr P4 = 0xC0;

sbit NandFlash_CLE= P1^4;
sbit NandFlash_ALE = P1^1;
sbit NandFlash_RB = P1^2;
sbit NandFlash_CS = P1^5;
sbit NandFlash_WR = P1^7;
sbit NandFlash_RD = P1^6;

#define Nandflash_CMD_LATCH_HIGH()      NandFlash_CLE = 1 
#define Nandflash_CMD_LATCH_LOW()       NandFlash_CLE = 0
///////////////////////////////////////////////////////////////////////
#define Nandflash_ADDR_LATCH_HIGH()      NandFlash_ALE = 1 
#define Nandflash_ADDR_LATCH_LOW()       NandFlash_ALE = 0

///////////////////////////////////////////////////////////////////////
//#define ADD_LATCH_HIGH()      (*AT91C_PIOA_SODR |= ADD_LATCH) 
//#define ADD_LATCH_LOW()       (*AT91C_PIOA_CODR |= ADD_LATCH)
///////////////////////////////////////////////////////////////////////

#define NWRITE_HIGH()         NandFlash_WR = 1
#define NWRITE_LOW()          NandFlash_WR = 0
///////////////////////////////////////////////////////////////////////
#define NREAD_HIGH()          NandFlash_RD = 1
#define NREAD_LOW()           NandFlash_RD = 0
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#define NF_CS_HIGH()          NandFlash_CS = 1
#define NF_CS_LOW()           NandFlash_CS = 0

#define NF_DataPort P4
uchar xdata ucNFPageBuff[2048];		//NandFlash��д����������

extern void WriteOneByte(unsigned char ucWriteData);

void Delay(uchar ucCount);
void ComShowString(uchar *ucPrintfString);
void ComShowUChar(unsigned char value);

#if 0
void ComShowString(uchar *ucPrintfString)
{
	while(*ucPrintfString)
	{
		WriteOneByte(*ucPrintfString++);
	}
}
#endif

#if 0
code  unsigned char lcd_hexchars[17] = "0123456789abcdef";
void ComShowUChar(unsigned char value)
{
  	WriteOneByte(lcd_hexchars[(value>>4)&0xf]);
  	WriteOneByte(lcd_hexchars[(value)&0xf]);
}
#endif

void port_WriteDataNoAdd(uchar ucWriteData)
{

	P4MDOUT = 0xff;			//����Ϊ�������
 	//Delay(1);

	NF_DataPort = ucWriteData;

   	NWRITE_LOW();
	_nop_(); 
//	_nop_(); 
	//Delay(1);
//  	Delay(1);
   	NWRITE_HIGH();  
}

//----------------------------------------------------------------------
uchar port_ReadDataNoAdd(void)
{
   	uchar  temp8;
   
	P4MDOUT = 0x00;			//����Ϊ��©���
	P4 = 0xff;		//����Ϊ��©��Ч״̬
   //Delay(1);
   	NREAD_LOW();
 	//Delay(1);
	_nop_(); 		//���û�������ʱ����ȡ���ݻ����
//	_nop_(); 

   	temp8 = NF_DataPort;   
   	NREAD_HIGH();
   
   return temp8;  
}

#define	COMMAND				0x03	
#define	ADDRESS				0x05	
#define	D_DATA				0x01
#define	INACTIVE			0x09	



void NandFlash_Reset(void)		                                     //flash reset
{ 
	unsigned int i;

	NF_CS_LOW();
	Nandflash_CMD_LATCH_HIGH();
	Nandflash_ADDR_LATCH_LOW();
	
	port_WriteDataNoAdd(0xff);
	NF_CS_HIGH();

	for (i=0; i<3000; i++) ;	                                 //delay 	
}

#if 0
void NandFlash_ReadID(uchar *IDBuff)
{
	uchar xdata ucIDBuff[5];

	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x90);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();
	port_WriteDataNoAdd(0x00);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	P0= 0xff;
	P4MDOUT = 0x00;			//����Ϊ��©���

	Delay(1);
	ucIDBuff[0] = port_ReadDataNoAdd();
	ucIDBuff[1] = port_ReadDataNoAdd();
	ucIDBuff[2] = port_ReadDataNoAdd();
	ucIDBuff[3] = port_ReadDataNoAdd();
	ucIDBuff[4] = port_ReadDataNoAdd();
	memcpy(IDBuff,ucIDBuff,5);

	NF_CS_HIGH();
}

#endif

#if 0
//���NandFlashоƬ�Ƿ�������ͨѶ
bool bCheckNandFlashDevice(void)
{
	uchar ucIDBuff[5];

	NandFlash_Reset();
	NandFlash_ReadID(ucIDBuff);

//	ComShowString("\r\nRead ID Value is ");
//	ComShowUChar(ucIDBuff[0]);
//	ComShowUChar(ucIDBuff[1]);
//	ComShowUChar(ucIDBuff[2]);
//	ComShowUChar(ucIDBuff[3]);
// 	ComShowString("\r\n ");
//
	if((ucIDBuff[0] == 0xEC)&& (ucIDBuff[1] == 0xF1) && (ucIDBuff[3] == 0x95))
	{
		ComShowString("Check NandFlash OK\r\n");
		return(true);
	}
	else
	{
		return(false);
	}	
}
#endif

//��Flash�ж�ȡ1���������ݵ�������
unsigned char xdata Sector_Buf[512];		//�ļ���������
//char xdata Sector_AdditionalDataBuff[64];		//�����еĸ������ݡ�
unsigned int  uiAddressColumn,uiAddressRow;		//�е�ַ0-11���е�ַ0-15
//��黵�����ݣ����ؼ���з��ֵĻ�����������
#if 0
uint  NandFlash_CheckBadBlock(void)
{
	uchar xdata ucReadInvalidBlockFlagData;
	uint xdata uiBadBlockCount;
	uint xdata uiCount;


	NF_CS_LOW();

	uiBadBlockCount = 0;
		
	for(uiCount=1; uiCount<1024; uiCount++)
	{
		uiAddressColumn = 2048;		 	//��ȡ���ݵĵ�ַ
		uiAddressRow = uiCount*64;		//ÿ����64ҳ

		//���Ե�1���Ƿ��л���
		Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x00);
	
		Nandflash_CMD_LATCH_LOW();
		Nandflash_ADDR_LATCH_HIGH();		//����4λ��ַ����
		port_WriteDataNoAdd(uiAddressColumn & 0xFF);
		port_WriteDataNoAdd(uiAddressColumn>>8);
	 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
		port_WriteDataNoAdd(uiAddressRow>>8);
	
		Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x30);
		NandFlash_RB =1;
		Delay(1);
	
		while(NandFlash_RB == 0);		//�ȴ�BUSY�ź�
	
	
		Nandflash_CMD_LATCH_LOW();
		Nandflash_ADDR_LATCH_LOW();
	
	//	P0= 0xff;
		Delay(1);
		ucReadInvalidBlockFlagData = port_ReadDataNoAdd();
	
		if(ucReadInvalidBlockFlagData != 0xFF)
		{
			 uiBadBlockCount ++;
			 continue;
		}

		//���Ե�2���Ƿ��л���
		uiAddressRow++;			//ÿ1��ĵ�2ҳ
		Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x00);
	
		Nandflash_CMD_LATCH_LOW();
		Nandflash_ADDR_LATCH_HIGH();		//����4λ��ַ����
		port_WriteDataNoAdd(uiAddressColumn & 0xFF);
		port_WriteDataNoAdd(uiAddressColumn>>8);
	 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
		port_WriteDataNoAdd(uiAddressRow>>8);
	
		Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x30);
		NandFlash_RB =1;
		Delay(1);
	
		while(NandFlash_RB == 0);		//�ȴ�BUSY�ź�
	
	
		Nandflash_CMD_LATCH_LOW();
		Nandflash_ADDR_LATCH_LOW();
	
//		P0= 0xff;
		Delay(1);
		ucReadInvalidBlockFlagData = port_ReadDataNoAdd();
	
		if(ucReadInvalidBlockFlagData != 0xFF)
		{
			 uiBadBlockCount ++;
		}
	}

	NF_CS_HIGH();

	return(uiBadBlockCount);
}

#endif

//����ָ���顣
//1G08�ܹ���1024���顣
bool NandFlash_EraseBlock(uint uiBlockNo)
{
	uchar ucReadStatusData;

	NF_CS_LOW();

	uiAddressRow = uiBlockNo*64;			//ÿ����64ҳ

	Nandflash_CMD_LATCH_HIGH();		  	//�����Զ�������������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x60);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//����2λ��ַ����
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//���Ͳ������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0xD0);		  	
	NandFlash_RB =1;
	Delay(1);

	while(NandFlash_RB == 0);		//�ȴ�BUSY�ź�

	Nandflash_CMD_LATCH_HIGH();		  	//���Ͳ������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x70);		  	

	//��ȡ��������״̬λ
	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	P0= 0xff;
	Delay(1);
	ucReadStatusData = port_ReadDataNoAdd();
	
	NF_CS_HIGH();

	if(ucReadStatusData & 0x01)		  //D0Ϊ1ʱ������������ʧ��
	{
		return(false);
	}
	else
	{
		return(true);
	}

}
code FlashBadBlockTabMask[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
//void MCU_ReadSector( INT16U sector, INT8U *buffer );
//void MCU_WriteSector( INT16U sector, INT8U *buffer );
//�������д�����ݵĿ�
#if 0
bool NandFlash_EraseAllDataBlock(void)
{
  	uint uiCount;
	uint uiOffset;
	uchar ucModeValue;


	ComShowString("\r\n Start Erase All Block");
//	MCU_ReadSector(0,Sector_Buf);		//��ȡ��ַ���ڵ����ݵ��ڴ�

	for(uiOffset = 256; uiOffset <256+128; uiOffset++)
	{
		Sector_Buf[uiOffset] = 0x00;		//�������¼��ȫ������
	}

  	for(uiCount=0; uiCount<1024; uiCount++)
  	{
  	 	if(NandFlash_EraseBlock(uiCount) == false)
	 	{
	 		ComShowString("\r\nErase Err! Address is :");
			ComShowUChar(uiCount>>8);
			ComShowUChar(uiCount);
//			ComShowString("\r\nErase End\r\n");

			uiOffset =  uiCount/8;
			ucModeValue	= uiCount%8;
			Sector_Buf[uiOffset+256] |= FlashBadBlockTabMask[ucModeValue]; 
//	 	return(false);
	 	}
	 	else
	 	{
//	 	ComShowString("Erase One Block Ok\r\n");
	 	}
  	}

//	MCU_WriteSector(0,Sector_Buf);	//������������

  	return(true);
}

//��ȡ���������ݵ�������
//ÿ���豸=1024���飬ÿ����Ϊ64ҳ��ÿ��Ϊ2K
void   NandFlash_ReadSector(ulong ulSectorNo)
{
	uint uiCount;

	uiAddressColumn = (ulSectorNo%4)*512;		 //��ȡ���ݵĵ�ַ
	uiAddressRow = ulSectorNo/4;				//ҳ��ַ

  //	printf("uiAddressColumn = %d\r\n",uiAddressColumn);
	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x00);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//����4λ��ַ����
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x30);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//�ȴ�BUSY�źű�Ϊ����
	//ҳ�����Ѿ���ȡ��оƬ��Buffer��
	
//	Nandflash_CMD_LATCH_HIGH();		  	//������������оƬ
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x05);
//
//	Nandflash_CMD_LATCH_LOW();
//	Nandflash_ADDR_LATCH_HIGH();		//����2λ�е�ַ����
//	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
//	port_WriteDataNoAdd(uiAddressColumn>>8);
//
//	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x0E0);

 	P4MDOUT = 0x00;			//����Ϊ��©���
	P4 = 0xff;		//����Ϊ��©��Ч״̬

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	Delay(1);

	for(uiCount=0; uiCount<512; uiCount++)
//	for(uiCount=0; uiCount<528; uiCount++)
	{
//		Sector_Buf[uiCount] = port_ReadDataNoAdd();
	 	NREAD_LOW();
		_nop_(); 		//���û�������ʱ����ȡ���ݻ����

   		Sector_Buf[uiCount] = NF_DataPort;   
   		NREAD_HIGH();	
	}
	NF_CS_HIGH();	
}
#endif


//��ȡ���������ݵ�������
//ÿ���豸=1024���飬ÿ����Ϊ64ҳ��ÿ��Ϊ2K
void   NF_ReadSectorToBuff(ulong ulSectorNo,uchar *ucDataBuffer)
{
	uint uiCount;

	uiAddressColumn = (ulSectorNo%4)*512;		 //��ȡ���ݵĵ�ַ
	uiAddressRow = ulSectorNo/4;				//ҳ��ַ

  //	printf("uiAddressColumn = %d\r\n",uiAddressColumn);
	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x00);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//����4λ��ַ����
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x30);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//�ȴ�BUSY�źű�Ϊ����
	//ҳ�����Ѿ���ȡ��оƬ��Buffer��
	
//	Nandflash_CMD_LATCH_HIGH();		  	//������������оƬ
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x05);
//
//	Nandflash_CMD_LATCH_LOW();
//	Nandflash_ADDR_LATCH_HIGH();		//����2λ�е�ַ����
//	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
//	port_WriteDataNoAdd(uiAddressColumn>>8);
//
//	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x0E0);

 	P4MDOUT = 0x00;			//����Ϊ��©���
	P4 = 0xff;		//����Ϊ��©��Ч״̬

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	Delay(1);

	for(uiCount=0; uiCount<512; uiCount++)
//	for(uiCount=0; uiCount<528; uiCount++)
	{
//		Sector_Buf[uiCount] = port_ReadDataNoAdd();
	 	NREAD_LOW();
		_nop_(); 		//���û�������ʱ����ȡ���ݻ����

   		*ucDataBuffer++= NF_DataPort;   
   		NREAD_HIGH();	
	}
	NF_CS_HIGH();	
}



//��ȡ���������ݵ�������
//ÿ���豸=1024���飬ÿ����Ϊ64ҳ��ÿ��Ϊ2K
void   NF_ReadPage(ulong ulSectorNo)
{
	uint uiCount;

	uiAddressColumn = (ulSectorNo%4)*512;		 //��ȡ���ݵĵ�ַ
	uiAddressRow = ulSectorNo/4;				//ҳ��ַ

  //	printf("uiAddressColumn = %d\r\n",uiAddressColumn);
	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x00);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//����4λ��ַ����
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x30);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//�ȴ�BUSY�źű�Ϊ����
	//ҳ�����Ѿ���ȡ��оƬ��Buffer��
	
//	Nandflash_CMD_LATCH_HIGH();		  	//������������оƬ
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x05);
//
//	Nandflash_CMD_LATCH_LOW();
//	Nandflash_ADDR_LATCH_HIGH();		//����2λ�е�ַ����
//	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
//	port_WriteDataNoAdd(uiAddressColumn>>8);
//
//	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x0E0);

 	P4MDOUT = 0x00;			//����Ϊ��©���
	P4 = 0xff;		//����Ϊ��©��Ч״̬

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	Delay(1);

	for(uiCount=0; uiCount<2048; uiCount++)
//	for(uiCount=0; uiCount<528; uiCount++)
	{
//		Sector_Buf[uiCount] = port_ReadDataNoAdd();
	 	NREAD_LOW();
		_nop_(); 		//���û�������ʱ����ȡ���ݻ����

   		ucNFPageBuff[uiCount]= NF_DataPort;   
   		NREAD_HIGH();	
	}
	NF_CS_HIGH();	
}


#define C_FileDataStartBlockNo   1
#if 0
//1024���飬ÿ��64ҳ��ÿҳ2K
//�˺������ҳд���ܣ�ÿ��д1ҳ��4������������
void   NandFlash_WritePageStart(ulong ulSectorNo)
{

//	ulSectorNo += 64;				//����64ҳ���ڼ�¼�ļ����������Ϣ
	uiAddressColumn = (ulSectorNo%4)*512;		 //��ȡ���ݵĵ�ַ
	uiAddressRow = ulSectorNo/4  ;			//ҳ��ַ��

	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x80);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//����4λ��ַ����
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

}
 
void   NandFlash_WriteOneSectorDataToPage(void)
{
	uint uiCount;

	for(uiCount=0; uiCount<512; uiCount++)
	{
		NF_DataPort = Sector_Buf[uiCount];	
	   	NWRITE_LOW(); 
//		Delay(1);
		_nop_();
 	   	NWRITE_HIGH();  
	}
	NWRITE_HIGH();  

}
#endif

bool  NandFlash_WritePageEnd(void)
{
	uchar ucReadStatusData;

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x10);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//�ȴ�BUSY�źű�Ϊ����

	Nandflash_CMD_LATCH_HIGH();		  	//����70���оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x70);		  	

	//��ȡ����״̬λ
	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

	P0= 0xff;
//	Delay(1);
	ucReadStatusData = port_ReadDataNoAdd();
	
	NF_CS_HIGH();

	if(ucReadStatusData & 0x01)		  //D0Ϊ1ʱ��������ʧ��
	{
		return(false);
	}
	else
	{
		return(true);
	}
}
//NandFlashҳд������1ҳ2K�ֽڣ�д֮ǰ��������Ӧ������д��ҳ����洢��
bool NandFlash_WritePage(ulong ulSectorNo)
{
	uint uiCount;

 //����ҳ��̲�����д���ַ
	uiAddressColumn = (ulSectorNo%4)*512;		 //��ȡ���ݵĵ�ַ
	uiAddressRow = ulSectorNo/4  ;			//ҳ��ַ��

	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//�������оƬ
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x80);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//����4λ��ַ����
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//������ݵ�оƬ��д������
	for(uiCount=0; uiCount<2048; uiCount++)
	{
 	   	NWRITE_HIGH();  
		NF_DataPort = ucNFPageBuff[uiCount];	
	   	NWRITE_LOW(); 
//		Delay(1);
//		_nop_();
// 	   	NWRITE_HIGH();  
	}
	NWRITE_HIGH();  

   return(NandFlash_WritePageEnd());
}

#if 0
bool NandFlash_WriteScector(ulong ulSectorNo)
{
	uchar ucSectorIndexNo;	//������ҳ�е�������
	uint uiCount;
  	ucSectorIndexNo = ulSectorNo%4 ;
	if(ucSectorIndexNo == 0)
	{
		NF_ReadSectorToBuff(ulSectorNo+1,ucNFPageBuff+512);	
		NF_ReadSectorToBuff(ulSectorNo+2,ucNFPageBuff+1024);	
		NF_ReadSectorToBuff(ulSectorNo+3,ucNFPageBuff+1536);	
	  	for(uiCount= 0 ; uiCount<512; uiCount++)
		{
			ucNFPageBuff[uiCount] = Sector_Buf[uiCount];
		}
	}
	else if(ucSectorIndexNo == 1)
	{
		NF_ReadSectorToBuff(ulSectorNo,ucNFPageBuff);	
		NF_ReadSectorToBuff(ulSectorNo+2,ucNFPageBuff+1024);	
		NF_ReadSectorToBuff(ulSectorNo+3,ucNFPageBuff+1536);	
	  	for(uiCount= 0 ; uiCount<512; uiCount++)
		{
			ucNFPageBuff[uiCount+512] = Sector_Buf[uiCount];
		}
	}
	else if(ucSectorIndexNo == 2)
	{
		NF_ReadSectorToBuff(ulSectorNo,ucNFPageBuff);	
		NF_ReadSectorToBuff(ulSectorNo+1,ucNFPageBuff+512);	
		NF_ReadSectorToBuff(ulSectorNo+3,ucNFPageBuff+1536);	
	  	for(uiCount= 0 ; uiCount<512; uiCount++)
		{
			ucNFPageBuff[uiCount+1024] = Sector_Buf[uiCount];
		}
	}
	else
	{
		NF_ReadSectorToBuff(ulSectorNo,ucNFPageBuff);	
		NF_ReadSectorToBuff(ulSectorNo+1,ucNFPageBuff+512);	
		NF_ReadSectorToBuff(ulSectorNo+2,ucNFPageBuff+1024);	
	  	for(uiCount= 0 ; uiCount<512; uiCount++)
		{
			ucNFPageBuff[uiCount+1536] = Sector_Buf[uiCount];
		}
	}
	return(NandFlash_WritePage(ulSectorNo));
}
#endif

#if 0

void TestNandFlash(void)
{
 	uchar xdata ucIDDataBuff[5];
	uint uiCount;

	NandFlash_Reset();
	TI0 = 1;
	while(1)
	{
		NandFlash_ReadID(ucIDDataBuff);
		printf("Read NandFlash Id Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)ucIDDataBuff[0],(uint)ucIDDataBuff[1],(uint)ucIDDataBuff[2],(uint)ucIDDataBuff[3],(uint)ucIDDataBuff[4]);
		printf("Check Bad Block Count is %d\r\n",NandFlash_CheckBadBlock());

		if(NandFlash_EraseBlock(0))
		{
			printf("erase Block 0 success!\r\n");
		  	NandFlash_ReadSector(0);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(1);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(2);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(3);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);


			printf("\r\n Start Write Page\r\n");
		   for(uiCount=0; uiCount<512; uiCount++)
		   {
		   	 //Sector_Buf[uiCount] = uiCount & 0xff;
			  ucNFPageBuff[uiCount] = uiCount;
		   }

		   for(uiCount=512; uiCount<1024; uiCount++)
		   {
		   	 //Sector_Buf[uiCount] = (uiCount & 0xff + 1);
			 ucNFPageBuff[uiCount] = 0xAA;
//			 ucNFPageBuff[uiCount] = 0x68;

		   }

		   for(uiCount=1024; uiCount<1536; uiCount++)
		   {
		   	 //Sector_Buf[uiCount] = (uiCount & 0xff + 2);
			 ucNFPageBuff[uiCount] = 0x5A;
		   }

		   for(uiCount=1536; uiCount<2048; uiCount++)
		   {
		   	// Sector_Buf[uiCount] = (uiCount & 0xff + 3);
			 ucNFPageBuff[uiCount] = 0xA5;
		   }
		   if(NandFlash_WritePage(0))
		   {
		   		printf("Programer One Page Ok!\r\n");
		   }
		   else
		   {
		   		printf("Programer One Page Err!\r\n");
		   }
				
		  	NandFlash_ReadSector(0);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(1);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(2);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(3);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);

		   	while(1);
		}
		else
		{
			printf("Erase Block 0 Err!\r\n");
		}


	}
}

#endif

/*
void TestNandFlash(void)
{
	uchar xdata ucIDDataBuff[5];
	uint uiCount;

	NandFlash_Reset();
	TI = 1;
	while(1)
	{
		NandFlash_ReadID(ucIDDataBuff);
		printf("Read NandFlash Id Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)ucIDDataBuff[0],(uint)ucIDDataBuff[1],(uint)ucIDDataBuff[2],(uint)ucIDDataBuff[3],(uint)ucIDDataBuff[4]);
		printf("Check Bad Block Count is %d\r\n",NandFlash_CheckBadBlock());
		if(NandFlash_EraseBlock(0))
		{
			printf("erase Block 0 success!\r\n");
		}
		else
		{
			printf("Erase Block 0 Err!\r\n");
		}
		{
			//����д���ݵ�Flash��
			printf("Page Write Start\r\n");
		   NandFlash_WritePageStart(0);

		   for(uiCount=0; uiCount<512; uiCount++)
		   {
		   	 //Sector_Buf[uiCount] = uiCount & 0xff;
			  Sector_Buf[uiCount] = uiCount;
		   }
		  	NandFlash_WriteOneSectorDataToPage();
		   	printf("Page Write Data Finish\r\n");

		   for(uiCount=0; uiCount<512; uiCount++)
		   {
		   	 //Sector_Buf[uiCount] = (uiCount & 0xff + 1);
			 Sector_Buf[uiCount] = 0xAA;
		   }
		  	NandFlash_WriteOneSectorDataToPage();
		   	printf("Page Write Data Finish\r\n");

		   for(uiCount=0; uiCount<512; uiCount++)
		   {
		   	 //Sector_Buf[uiCount] = (uiCount & 0xff + 2);
			 Sector_Buf[uiCount] = 0x5A;
		   }
		  	NandFlash_WriteOneSectorDataToPage();
		   	printf("Page Write Data Finish\r\n");

		   for(uiCount=0; uiCount<512; uiCount++)
		   {
		   	// Sector_Buf[uiCount] = (uiCount & 0xff + 3);
			 Sector_Buf[uiCount] = 0xA5;
		   }
		  	NandFlash_WriteOneSectorDataToPage();
			printf("Page Write End\r\n");
		   if(NandFlash_WritePageEnd())
		   {
		   		printf("Programer One Page Ok!\r\n");
		   }
		   else
		   {
		   		printf("Programer One Page Err!\r\n");
		   }
				
		  	NandFlash_ReadSector(0);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[511],(uint)Sector_Buf[512],(uint)Sector_Buf[513]);
		  	NandFlash_ReadSector(1);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(2);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(3);//��1�����������ݵ�������
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);

		}
		Delay(250);
		while(1);
	}	
}
*/




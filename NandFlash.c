
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
uchar xdata ucNFPageBuff[2048];		//NandFlash读写操作缓冲区

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

	P4MDOUT = 0xff;			//配置为推挽输出
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
   
	P4MDOUT = 0x00;			//配置为开漏输出
	P4 = 0xff;		//设置为开漏有效状态
   //Delay(1);
   	NREAD_LOW();
 	//Delay(1);
	_nop_(); 		//如果没有这个延时，读取数据会出错。
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
	P4MDOUT = 0x00;			//配置为开漏输出

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
//检查NandFlash芯片是否能正常通讯
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

//从Flash中读取1个扇区内容到缓冲区
unsigned char xdata Sector_Buf[512];		//文件缓冲区。
//char xdata Sector_AdditionalDataBuff[64];		//扇区中的附加数据。
unsigned int  uiAddressColumn,uiAddressRow;		//列地址0-11，行地址0-15
//检查坏块数据，返回检查中发现的坏块总数量。
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
		uiAddressColumn = 2048;		 	//读取数据的地址
		uiAddressRow = uiCount*64;		//每块有64页

		//测试第1块是否有坏块
		Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x00);
	
		Nandflash_CMD_LATCH_LOW();
		Nandflash_ADDR_LATCH_HIGH();		//发送4位地址数据
		port_WriteDataNoAdd(uiAddressColumn & 0xFF);
		port_WriteDataNoAdd(uiAddressColumn>>8);
	 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
		port_WriteDataNoAdd(uiAddressRow>>8);
	
		Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x30);
		NandFlash_RB =1;
		Delay(1);
	
		while(NandFlash_RB == 0);		//等待BUSY信号
	
	
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

		//测试第2块是否有坏块
		uiAddressRow++;			//每1块的第2页
		Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x00);
	
		Nandflash_CMD_LATCH_LOW();
		Nandflash_ADDR_LATCH_HIGH();		//发送4位地址数据
		port_WriteDataNoAdd(uiAddressColumn & 0xFF);
		port_WriteDataNoAdd(uiAddressColumn>>8);
	 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
		port_WriteDataNoAdd(uiAddressRow>>8);
	
		Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
		Nandflash_ADDR_LATCH_LOW();
		port_WriteDataNoAdd(0x30);
		NandFlash_RB =1;
		Delay(1);
	
		while(NandFlash_RB == 0);		//等待BUSY信号
	
	
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

//擦除指定块。
//1G08总共有1024个块。
bool NandFlash_EraseBlock(uint uiBlockNo)
{
	uchar ucReadStatusData;

	NF_CS_LOW();

	uiAddressRow = uiBlockNo*64;			//每块有64页

	Nandflash_CMD_LATCH_HIGH();		  	//发送自动块擦除设置命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x60);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//发送2位地址数据
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//发送擦除命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0xD0);		  	
	NandFlash_RB =1;
	Delay(1);

	while(NandFlash_RB == 0);		//等待BUSY信号

	Nandflash_CMD_LATCH_HIGH();		  	//发送擦除命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x70);		  	

	//读取擦除操作状态位
	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	P0= 0xff;
	Delay(1);
	ucReadStatusData = port_ReadDataNoAdd();
	
	NF_CS_HIGH();

	if(ucReadStatusData & 0x01)		  //D0为1时表明擦除操作失败
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
//擦除所有存放数据的块
#if 0
bool NandFlash_EraseAllDataBlock(void)
{
  	uint uiCount;
	uint uiOffset;
	uchar ucModeValue;


	ComShowString("\r\n Start Erase All Block");
//	MCU_ReadSector(0,Sector_Buf);		//读取地址所在的内容到内存

	for(uiOffset = 256; uiOffset <256+128; uiOffset++)
	{
		Sector_Buf[uiOffset] = 0x00;		//将坏块记录表全部清零
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

//	MCU_WriteSector(0,Sector_Buf);	//更新闪存内容

  	return(true);
}

//读取扇区的内容到缓冲区
//每个设备=1024个块，每个块为64页，每面为2K
void   NandFlash_ReadSector(ulong ulSectorNo)
{
	uint uiCount;

	uiAddressColumn = (ulSectorNo%4)*512;		 //读取数据的地址
	uiAddressRow = ulSectorNo/4;				//页地址

  //	printf("uiAddressColumn = %d\r\n",uiAddressColumn);
	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x00);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//发送4位地址数据
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x30);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//等待BUSY信号变为空闲
	//页数据已经读取到芯片的Buffer中
	
//	Nandflash_CMD_LATCH_HIGH();		  	//发送随机读命令到芯片
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x05);
//
//	Nandflash_CMD_LATCH_LOW();
//	Nandflash_ADDR_LATCH_HIGH();		//发送2位列地址数据
//	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
//	port_WriteDataNoAdd(uiAddressColumn>>8);
//
//	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x0E0);

 	P4MDOUT = 0x00;			//配置为开漏输出
	P4 = 0xff;		//设置为开漏有效状态

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	Delay(1);

	for(uiCount=0; uiCount<512; uiCount++)
//	for(uiCount=0; uiCount<528; uiCount++)
	{
//		Sector_Buf[uiCount] = port_ReadDataNoAdd();
	 	NREAD_LOW();
		_nop_(); 		//如果没有这个延时，读取数据会出错。

   		Sector_Buf[uiCount] = NF_DataPort;   
   		NREAD_HIGH();	
	}
	NF_CS_HIGH();	
}
#endif


//读取扇区的内容到缓冲区
//每个设备=1024个块，每个块为64页，每面为2K
void   NF_ReadSectorToBuff(ulong ulSectorNo,uchar *ucDataBuffer)
{
	uint uiCount;

	uiAddressColumn = (ulSectorNo%4)*512;		 //读取数据的地址
	uiAddressRow = ulSectorNo/4;				//页地址

  //	printf("uiAddressColumn = %d\r\n",uiAddressColumn);
	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x00);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//发送4位地址数据
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x30);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//等待BUSY信号变为空闲
	//页数据已经读取到芯片的Buffer中
	
//	Nandflash_CMD_LATCH_HIGH();		  	//发送随机读命令到芯片
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x05);
//
//	Nandflash_CMD_LATCH_LOW();
//	Nandflash_ADDR_LATCH_HIGH();		//发送2位列地址数据
//	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
//	port_WriteDataNoAdd(uiAddressColumn>>8);
//
//	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x0E0);

 	P4MDOUT = 0x00;			//配置为开漏输出
	P4 = 0xff;		//设置为开漏有效状态

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	Delay(1);

	for(uiCount=0; uiCount<512; uiCount++)
//	for(uiCount=0; uiCount<528; uiCount++)
	{
//		Sector_Buf[uiCount] = port_ReadDataNoAdd();
	 	NREAD_LOW();
		_nop_(); 		//如果没有这个延时，读取数据会出错。

   		*ucDataBuffer++= NF_DataPort;   
   		NREAD_HIGH();	
	}
	NF_CS_HIGH();	
}



//读取扇区的内容到缓冲区
//每个设备=1024个块，每个块为64页，每面为2K
void   NF_ReadPage(ulong ulSectorNo)
{
	uint uiCount;

	uiAddressColumn = (ulSectorNo%4)*512;		 //读取数据的地址
	uiAddressRow = ulSectorNo/4;				//页地址

  //	printf("uiAddressColumn = %d\r\n",uiAddressColumn);
	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x00);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//发送4位地址数据
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x30);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//等待BUSY信号变为空闲
	//页数据已经读取到芯片的Buffer中
	
//	Nandflash_CMD_LATCH_HIGH();		  	//发送随机读命令到芯片
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x05);
//
//	Nandflash_CMD_LATCH_LOW();
//	Nandflash_ADDR_LATCH_HIGH();		//发送2位列地址数据
//	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
//	port_WriteDataNoAdd(uiAddressColumn>>8);
//
//	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
//	Nandflash_ADDR_LATCH_LOW();
//	port_WriteDataNoAdd(0x0E0);

 	P4MDOUT = 0x00;			//配置为开漏输出
	P4 = 0xff;		//设置为开漏有效状态

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//	Delay(1);

	for(uiCount=0; uiCount<2048; uiCount++)
//	for(uiCount=0; uiCount<528; uiCount++)
	{
//		Sector_Buf[uiCount] = port_ReadDataNoAdd();
	 	NREAD_LOW();
		_nop_(); 		//如果没有这个延时，读取数据会出错。

   		ucNFPageBuff[uiCount]= NF_DataPort;   
   		NREAD_HIGH();	
	}
	NF_CS_HIGH();	
}


#define C_FileDataStartBlockNo   1
#if 0
//1024个块，每块64页，每页2K
//此函数完成页写功能，每次写1页即4个扇区的数据
void   NandFlash_WritePageStart(ulong ulSectorNo)
{

//	ulSectorNo += 64;				//留出64页用于记录文件索引相关信息
	uiAddressColumn = (ulSectorNo%4)*512;		 //读取数据的地址
	uiAddressRow = ulSectorNo/4  ;			//页地址，

	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x80);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//发送4位地址数据
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

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x10);
	NandFlash_RB =1;
//	Delay(1);

	while(NandFlash_RB == 0);		//等待BUSY信号变为空闲

	Nandflash_CMD_LATCH_HIGH();		  	//发送70命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x70);		  	

	//读取操作状态位
	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

	P0= 0xff;
//	Delay(1);
	ucReadStatusData = port_ReadDataNoAdd();
	
	NF_CS_HIGH();

	if(ucReadStatusData & 0x01)		  //D0为1时表明操作失败
	{
		return(false);
	}
	else
	{
		return(true);
	}
}
//NandFlash页写操作，1页2K字节，写之前将扇区对应的数据写入页缓冲存储器
bool NandFlash_WritePage(ulong ulSectorNo)
{
	uint uiCount;

 //启动页编程操作并写入地址
	uiAddressColumn = (ulSectorNo%4)*512;		 //读取数据的地址
	uiAddressRow = ulSectorNo/4  ;			//页地址，

	NF_CS_LOW();

	Nandflash_CMD_LATCH_HIGH();		  	//发送命令到芯片
	Nandflash_ADDR_LATCH_LOW();
	port_WriteDataNoAdd(0x80);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_HIGH();		//发送4位地址数据
	port_WriteDataNoAdd(uiAddressColumn & 0xFF);
	port_WriteDataNoAdd(uiAddressColumn>>8);
 	port_WriteDataNoAdd(uiAddressRow & 0xFF);
	port_WriteDataNoAdd(uiAddressRow>>8);

	Nandflash_CMD_LATCH_LOW();
	Nandflash_ADDR_LATCH_LOW();

//填充数据到芯片的写缓冲区
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
	uchar ucSectorIndexNo;	//扇区在页中的索引号
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
		  	NandFlash_ReadSector(0);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(1);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(2);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(3);//读1个扇区的内容到缓冲区
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
				
		  	NandFlash_ReadSector(0);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(1);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(2);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(3);//读1个扇区的内容到缓冲区
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
			//测试写数据到Flash中
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
				
		  	NandFlash_ReadSector(0);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[511],(uint)Sector_Buf[512],(uint)Sector_Buf[513]);
		  	NandFlash_ReadSector(1);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(2);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);
		  	NandFlash_ReadSector(3);//读1个扇区的内容到缓冲区
			printf("Read NandFlash Data Value is:%02X,%02X,%02X,%02X,%02X\r\n",(uint)Sector_Buf[0],(uint)Sector_Buf[1],(uint)Sector_Buf[2],(uint)Sector_Buf[3],(uint)Sector_Buf[4]);

		}
		Delay(250);
		while(1);
	}	
}
*/




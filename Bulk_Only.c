#include "c8051F340.h"
#include "stdio.h"
#include "intrins.h"
/////////////////////////////

#include "Usb_FunDec.h"
#include "Usb_Parameter.h"
#include "Usb_Register.h"
#include "Usb_Request.h"
#include "Usb_SCSI_Descreptor.h"

#define uchar unsigned char  
#define uint unsigned int 
#define ulong unsigned long
#define bool bit

///////////////////////////////////////////////////////////////////////////////////////////
unsigned char b[513];
unsigned char key[17]={'s','h','e','j','k','s','a','h','g','a','s','d','x','c','d','f'};
unsigned char skey[15*16];//加密后的密文或者解密后的明文放在b数组里面
static code unsigned char sbox[256]={
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,
0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,
0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,
0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,
0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,
0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,
0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,
0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,
0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,
0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,
0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,
0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,
0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,
0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,
0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,
0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,
0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16,
};
//逆向 S 盒
static code unsigned char contrary_sbox[256]={
0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,
0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,
0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,
0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,
0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,
0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,
0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,
0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,
0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,
0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,
0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,
0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,
0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,
0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,
0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,
0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,
0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d,
};
/*The key schedule rcon table*/ static code unsigned char Rcon[10]={
0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};
/*The xtime() function */ static unsigned char xtime(unsigned char x)
{
    int temp;
    temp=x<<1;
    if ( x& 0x80)
    {
        temp^=0x1b;
    }
    return temp;
}
/*MixColumns: Process the entire block*/
static void MixColumns(unsigned char *col)//列混合
{
    unsigned char xt[4]; int x,i;
    for(x=0;x<4;x++)
    {
        for(i=0;i<4;i++)
        xt[i]=xtime(col[x*4+i])^(col[x*4+(i+1)%4]^xtime(col[x*4+(i+1)%4]))^col[x*4+(i+2)%4]^col[x*4+(i+3)%4];
        for(i=0;i<4;i++)
        col[x*4+i]=xt[i];
    }
}
//逆向列混淆
static void Contrary_MixColumns(unsigned char *col)
{   unsigned char xt[4];
    int x,i;
    for(x=0;x<4;x++)
    {
        for(i=0;i<4;i++)
        xt[i]=(xtime(xtime(xtime(col[x*4+i])))^xtime(xtime(col[x*4+i]))^xtime(col[x*4+i]))^(xtime(xtime(xtime(col[x*4+(i+1)%4])))^xtime(col[x*4+(i+1)%4])^col[x*4+(i+1)%4])^(xtime(xtime(xtime(col[x*4+(i+2)%4])))^xtime(xtime(col[x*4+(i+2)%4]))^col[x*4+(i+2)%4])^(xtime(xtime(xtime(col[x*4+(i+3)%4])))^col[x*4+(i+3)%4]);
        for(i=0;i<4;i++)
        col[x*4+i]=xt[i];
    }
}
/*ShiftRows:Shifts the entire block*/ static void ShiftRows(unsigned char *col)//正向行移位
{ unsigned char t;
/*2nd row*/
t=col[1];col[1]=col[5];col[5]=col[9]; col[9]=col[13];col[13]=t;
/*3rd row*/ t=col[2];col[2]=col[10];col[10]=t; t=col[6];col[6]=col[14];col[14]=t;
/*4th row*/ t=col[15];col[15]=col[11];col[11]=col[7]; col[7]=col[3];col[3]=t;
}
//逆向行移位
static void Contrary_ShiftRows(unsigned char *col)
{ unsigned char t;
/*2nd row*/ t=col[13];col[13]=col[9];col[9]=col[5]; col[5]=col[1];col[1]=t;
/*3rd row*/ t=col[2];col[2]=col[10];col[10]=t; t=col[6];col[6]=col[14];col[14]=t;
/*4th row*/ t=col[3];col[3]=col[7];col[7]=col[11]; col[11]=col[15];col[15]=t;
}
/*SubBytes*/ static void SubBytes(unsigned char *col)//字节代换
{
    int x;
    for(x=0;x<16;x++)
{
     col[x]=sbox[col[x]];
}
}
//逆向字节代换
static void Contrary_SubBytes(unsigned char *col)
{
    int x;
for(x=0;x<16;x++)
{
    col[x]=contrary_sbox[col[x]];
}
}
/*AddRoundKey*/ static void AddRoundKey(unsigned char *col,unsigned char *key,int round)//密匙加
{
    int x;
    for(x=0;x<16;x++)
{
    col[x]^=key[(round<<4)+x];
}
}
/*Encrypt a single block with Nr Rounds(10,12,14)*/ void AesEncrypt(unsigned char *blk,unsigned char *key,int Nr)//加密一个区块
{
    int x;
AddRoundKey(blk,key,0);
for(x=1;x<=(Nr-1);x++)
{
SubBytes(blk);
ShiftRows(blk);
MixColumns(blk);
AddRoundKey(blk,key,x);
}
SubBytes(blk);
ShiftRows(blk);
AddRoundKey(blk,key,Nr);
}
//AES 解密
void Contrary_AesEncrypt(unsigned char *blk,unsigned char *key,int Nr)
{
    int x;
/*	unsigned char *contrary_key=key; for(x=0;x<11;x++,key+=16)
Contrary_MixColumns(key);*/
AddRoundKey(blk,key,Nr);
Contrary_ShiftRows(blk);
Contrary_SubBytes(blk);
for(x=(Nr-1);x>=1;x--)
{
AddRoundKey(blk,key,x);
Contrary_MixColumns(blk);
Contrary_ShiftRows(blk);
Contrary_SubBytes(blk);
}
AddRoundKey(blk,key,0);
}
/*Schedule a secret key for use.
*outkey[] must be 16*15 bytes in size
*Nk==number of 32 bit words in the key,e.g.,4,6,8
*Nr==number of rounds,e.g.,10,12,14
*/
void ScheduleKey(unsigned char *inkey,unsigned char *outkey,int Nk,int Nr)//安排一个保密密钥使用
{
    unsigned char temp[4],t;
    int	x,i;
/*copy the key*/ for(i=0;i<(4*Nk);i++)
{
    outkey[i]=inkey[i];
}
i=Nk;
while(i<(4*(Nr+1)))
{
/*temp=w[i]*/ for(x=0;x<4;x++)
temp[x]=outkey[((i-1)<<2)+x];
if(i%Nk==0)
{
/*RotWord()*/ t=temp[0];temp[0]=temp[1]; temp[1]=temp[2];temp[2]=temp[3];temp[3]=t;
/*SubWord()*/ for(x=0;x<4;x++)
{
    temp[x]=sbox[temp[x]];
}
temp[0]^=Rcon[(i/Nk)-1];
}
else if(Nk>6 && ( i%Nk )==4) { /*SubWord*/
    for(x=0;x<4;x++)
{
    temp[x]=sbox[temp[x]];
}
}
/*w[i]=w[i-Nk] xor temp*/ for(x=0;x<4;x++)
{
    outkey[(i<<2)+x]=outkey[((i-Nk)<<2)+x]^temp[x];
}
++ i;
} }
void AES_jiami(unsigned char *a,unsigned char *b)//a[513]是要加密的明文
{
    int i;
    int j;
    unsigned char pt[17];
    ScheduleKey(key,skey,4,10);
    for(i=0;i<32;i++)
    {
        for(j=0;j<16;j++)
        {
            pt[j]=a[i*16+j];
        }
        AesEncrypt(pt,skey,10);//AES 加密
        for(j=0;j<16;j++)
        {
            b[i*16+j]=pt[j];
        }
    }
	b[512]='\0';
}
void AES_jiemi(unsigned char *a,unsigned char *b)//a[513]为要解密的密文
{
    int i;
    int j;
    unsigned char pt[17];
    ScheduleKey(key,skey,4,10);
    for(i=0;i<32;i++)
    {
        for(j=0;j<16;j++)
        {
            pt[j]=a[i*16+j];
        }
        Contrary_AesEncrypt(pt,skey,10);//AES 解密
        for(j=0;j<16;j++)
        {
            b[i*16+j]=pt[j];
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////
sbit    LED			= P0^0;		//LED控制脚
// Bulk传输处理函数
void Interpret_CBW(void);		// 解释CBW命令
void TransDataGoOn(void);		// 继续传输
void TransCSW(void);			// 发送CSW
void Bulk_Receive_Data(void);	// 接收数据
void Copy(unsigned char *PEnd,unsigned char *PSource,unsigned int num);	// 在Bulk_Out过程中将Bulk_Buffer中的数据拷贝到Bulk_PACKET中
// 外部函数调用
extern void 			Flash_Read_Page(unsigned int BLockNum,unsigned char BlockPageNum,unsigned char *P,unsigned int Data_Length);
extern unsigned char 	Flash_Write_Page(unsigned int BLockNum,unsigned char BlockPageNum,unsigned char *P,unsigned int Data_Length);
extern unsigned char 	Flash_Erase_Block(unsigned int BlockNum);	
// 外部变量
extern unsigned char Ep_Status[3];
// BUlk传输变量
unsigned char Bulk_Status = BULK_IDLE;				// Bulk传输状态
unsigned char Bulk_CSW[13]={0x55,0x53,0x42,0x53,};

typedef struct CBWCB
{
	unsigned char type;// 0
	unsigned char no1; // 1
	//UL32 addr;            // 2,3,4,5
	UL16 addr_no;
	UL16 addr;
	unsigned char no2;//6
	unsigned int length; // 7,8
	UL32 no3;//9,10,11,12
	unsigned int no4;//13,14,
	unsigned char no5;//15
	

}Bulk_CBWCB;

typedef struct CBW
{
	UL32 dCBWstagnature;
	UL32 dCBWTag;
	UL32 dCBWDataTransLength;
	unsigned char bmCBWFlags;
	unsigned char dCBWLun;
	unsigned char dCBWCBLength;
	Bulk_CBWCB CBWCB;
}s_Bulk_CBW;
typedef union
{
unsigned char Bulk_PACKET[512];
//unsigned char Buffer[64];	// FIFO缓冲区
 s_Bulk_CBW Bulk_CBW;
}s_Bulk_Buffer;

xdata s_Bulk_Buffer Bulk_Buffer;
//xdata unsigned char Bulk_PACKET[512];	// 读写FLASH时的数据缓冲区
//xdata unsigned char Bulk_Buffer[64];	// FIFO缓冲区
//unsigned int LBA=0;	// LBA
S_LBA LBA;
//S_LBA LBA_old;
xdata unsigned char Bulk_READ[512];
unsigned char *usb_buf = Bulk_Buffer.Bulk_PACKET;

unsigned int Transfer_Length=0;			// 要传输的长度，单位为扇区
unsigned int nCurrentBlock=0;			// 当前的Block地址			K9F1G08为1024个块
unsigned char nCurrentPage=0;			// 当前的Page地址			K9F1G08为64个页
unsigned char nCurrentSectorOffsetInPage;		//当前页内扇区偏移号		K9F1G08为4个

unsigned char *pCurrentBuf;				// Bulk传输中的指针
unsigned int nBufCount=0;				// FIFO读写过程中传输的数据量
unsigned char nBegainPage=0;			// Bulk_Out传输中的开始Page地址	  
unsigned char nBegainSectorOffsetInPage;  //开始的页内偏移

extern uchar xdata ucNFPageBuff[2048];		//NandFlash读写操作缓冲区


//读取扇区的内容到缓冲区
//每个设备=1024个块，每个块为64页，每面为2K
void   NF_ReadSectorToBuff(ulong ulSectorNo,uchar *ucDataBuffer);
bool NandFlash_EraseBlock(uint uiBlockNo);
bool NandFlash_WritePage(ulong ulSectorNo);
void   NF_ReadSectorToBuff(ulong ulSectorNo,uchar *ucDataBuffer);
void   NF_ReadPage(ulong ulSectorNo);

/////////////////////////////////////////////////////////////////////////////////////////////			
void Handle_In1()
{
    unsigned char ControlReg;
   	UWrite_Byte(INDEX, 1);           
   	URead_Byte(EINCSRL, ControlReg); 
   	if (Ep_Status[1] == EP_HALT)         
   	{
      	UWrite_Byte(EINCSRL, rbInSDSTL);
   	}
   	else                                 
   	{
		if (ControlReg & rbInSTSTL)       
      	{
         	UWrite_Byte(EINCSRL, rbInCLRDT);//清除SDSTL和STSTL，并将CLRDT写1使toggle复位为0
      	}
		if (ControlReg & rbInUNDRUN)      	
      	{
         	UWrite_Byte(EINCSRL, 0x00);		// 清除UNDRUN，注意toggle位不复位
      	}
		if(!(ControlReg & rbInINPRDY))
		{
			switch(Bulk_Status)
			{
				case BULK_DATA_TRANS:	TransDataGoOn();	break;
				case BULK_DATA_END:		TransCSW();			break;
				default:									break;
			} 
		}                                      
   	}                                   
}
/////////////////////////////////////////////////////////////////////////////////////////////			
// Handle_Out2
void Handle_Out2()
{
	unsigned int   Count=0;
   	unsigned char  ControlReg;
   	UWrite_Byte(INDEX, 2);           		// Set index to endpoint 2 registers
   	URead_Byte(EOUTCSRL, ControlReg);
   	if (Ep_Status[2] == EP_HALT)         	// If endpoint is halted, send a stall
   	{
      	UWrite_Byte(EOUTCSRL, rbOutSDSTL);

   	}
	else
	{
	    if (ControlReg & rbOutSTSTL)      	
	    {
	         UWrite_Byte(EOUTCSRL, rbOutCLRDT);	//清除SDSTL和STSTL，并将CLRDT写1使toggle复位为0
	    }
		  	////////////////////////////////////////////////////////////////
		if(ControlReg & rbOutOPRDY)
		{
			UWrite_Byte(INDEX, 2); 
			URead_Byte(EOUTCNTL, Count);
			Fifo_Read(FIFO_EP2, Count, usb_buf);
			UWrite_Byte(INDEX, 2);           
	      		UWrite_Byte(EOUTCSRL, 0);   	// Clear Out Packet ready bit
			if((Bulk_Status !=BULK_DATA_RECIEVE) && (Bulk_Buffer.Bulk_PACKET[0]==0x55))
			{
				Interpret_CBW();
			}	
			else
			{			
				Bulk_Receive_Data();
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void Interpret_CBW()
{
	unsigned char i=0;
	Bulk_CSW[4] = Bulk_Buffer.Bulk_PACKET[4];
	Bulk_CSW[5] = Bulk_Buffer.Bulk_PACKET[5];
	Bulk_CSW[6] = Bulk_Buffer.Bulk_PACKET[6];
	Bulk_CSW[7] = Bulk_Buffer.Bulk_PACKET[7];

	switch(Bulk_Buffer.Bulk_CBW.CBWCB.type)
	{
		case INQUIRY:					SCSI_Inquiry();					break;
		case MODE_SENSE:				SCSI_Mode_Sense();				break;
		case READ10:					SCSI_Read10();					break;
		case READ_CAPACITY: 			SCSI_Read_Capacity();			break;
		case TEST_UNIT_READY: 			SCSI_Test_Unit_Ready();			break;
		case VERIFY:					SCSI_Verify();					break;
		case WRITE10: 					SCSI_Write10();					break;
		case MEDIUM_REMOVAL: 			SCSI_Medium_Removal();			break;

		case Vender_Order:				SCSI_Vender_Order();			break;
		default:						SCSI_Reserved();				break;
	}	
}
///////////////////////////////////////////////////////////////////////////////////////////////////			
void SCSI_Inquiry(void)
{
	if(Bulk_Buffer.Bulk_CBW.CBWCB.addr.r[0]!=0)
	{
		Bulk_Status=BULK_DATA_END;
		Fifo_Write(FIFO_EP1,sizeof(Device_InquiryData),Device_InquiryData);
		UWrite_Byte(INDEX, 1);
		UWrite_Byte(EINCSRL, rbInINPRDY);
	}
	else
	{}
}
void SCSI_Mode_Sense(void)
{
	Bulk_Status=BULK_DATA_END;
	Fifo_Write(FIFO_EP1,sizeof(Device_ModeSense),Device_ModeSense);
	UWrite_Byte(INDEX, 1);
	UWrite_Byte(EINCSRL, rbInINPRDY);
}
void SCSI_Read_Capacity(void)
{
	Bulk_Status=BULK_DATA_END;
	Fifo_Write(FIFO_EP1,sizeof(Device_Capacity),Device_Capacity);
	UWrite_Byte(INDEX, 1);
	UWrite_Byte(EINCSRL, rbInINPRDY);
}
void SCSI_Vender_Order()
{
	Bulk_Status=BULK_DATA_END;
	Fifo_Write(FIFO_EP1,sizeof(Vender_Order),Vender_Order);
	UWrite_Byte(INDEX, 1);
	UWrite_Byte(EINCSRL, rbInINPRDY);
}
void SCSI_Test_Unit_Ready(void)
{
	TransCSW();
}
void SCSI_Medium_Removal(void)
{
	TransCSW();
}
void SCSI_Verify(void)
{
	TransCSW();
}
void SCSI_Reserved(void)
{
}
void TransCSW()
{
	Fifo_Write(FIFO_EP1, 13, Bulk_CSW);
	UWrite_Byte(INDEX, 1);
	UWrite_Byte(EINCSRL, rbInINPRDY);   
	Bulk_Status=BULK_IDLE;                                 
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void SCSI_Read10()
{
	unsigned int i=0;
	LED=~LED;
	
	LBA.val = Bulk_Buffer.Bulk_CBW.CBWCB.addr.l;
	Transfer_Length = Bulk_Buffer.Bulk_CBW.CBWCB.length;
	//Transfer_Length = Bulk_CBW.CBWCB[7]*256 + Bulk_CBW.CBWCB[8];	//传输的长度，CBWCB的第7和8位代表传输长度
	nCurrentBlock = LBA.bits.block;		//计算FLASH的物理地址	
	nCurrentPage  = LBA.bits.page;
//	K9FReadPageData();
    
	NF_ReadSectorToBuff(LBA.val,Bulk_READ);
	AES_jiemi(Bulk_READ,Bulk_READ);
	//Flash_Read_Page(nCurrentBlock,nCurrentPage,Bulk_PACKET,512);	//向页缓冲区读1页

	Transfer_Length--;		//待传输的扇区数减1
	//nCurrentPage++;			//当前页序号加1
	LBA.val++;
	Bulk_Status = BULK_DATA_TRANS;		//Bulk在传输状态
	pCurrentBuf = Bulk_READ;			//指针指向页缓冲区
	nBufCount=0;	

	Fifo_Write(FIFO_EP1,EP1_PACKET_SIZE,pCurrentBuf+nBufCount);		//在双缓冲方式，一次可以向FIFO写两个数据包
	UWrite_Byte(INDEX, 1);
	UWrite_Byte(EINCSRL, rbInINPRDY);
}
void TransDataGoOn()
{
	unsigned int i=0;
	
	if(nBufCount<448)
	{
		nBufCount+=EP1_PACKET_SIZE;		
		Fifo_Write(FIFO_EP1,EP1_PACKET_SIZE,pCurrentBuf+nBufCount);	//在双缓冲方式，一次可以向FIFO写两个数据包
		UWrite_Byte(INDEX, 1);
		UWrite_Byte(EINCSRL, rbInINPRDY);
	}
	else if(Transfer_Length>0)
	{

		//Flash_Read_Page(nCurrentBlock,nCurrentPage,Bulk_PACKET,512);
//		K9FReadPageData();
		NF_ReadSectorToBuff(LBA.val,Bulk_READ);
		AES_jiemi(Bulk_READ,Bulk_READ);
		LBA.val++;
		Transfer_Length--;
		nBufCount=0;
		pCurrentBuf=Bulk_READ;
		Bulk_Status=BULK_DATA_TRANS;
		Fifo_Write(FIFO_EP1,EP1_PACKET_SIZE,pCurrentBuf+nBufCount);//在双缓冲方式，一次可以向FIFO写两个数据包
		UWrite_Byte(INDEX, 1);
		UWrite_Byte(EINCSRL, rbInINPRDY);	
	}
	else
	{
		Bulk_Status=BULK_DATA_END;
		nBufCount=0;
		TransCSW();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void SCSI_Write10(void)
{
	unsigned int i=0;
	unsigned int uiTemp;

	LED=~LED;
	LBA.val = Bulk_Buffer.Bulk_CBW.CBWCB.addr.l;
	Transfer_Length = Bulk_Buffer.Bulk_CBW.CBWCB.length;
	
	nCurrentBlock = (LBA.val)/256;		//计算FLASH的物理地址,当前块号
	uiTemp = (LBA.val)%256;

	
	nCurrentPage  = uiTemp/4;		//计算当前页号
//	uiTemp %= 4;

	nCurrentSectorOffsetInPage = uiTemp % 4;	//求出当前页内的偏移

	LBA.bits.block = Bufer_Block;
	LBA.bits.page = nCurrentPage;
	nBegainPage = nCurrentPage;
 	nBegainSectorOffsetInPage = nCurrentSectorOffsetInPage ;

	Bulk_Status = BULK_DATA_RECIEVE;
	//pCurrentBuf = Bulk_Buffer.Bulk_PACKET;
	nBufCount   = 0;

//	K9FEraseBlockData(Bufer_Block<<5);			//擦除缓冲区BLOCK
	NandFlash_EraseBlock(Bufer_Block);			//擦除缓冲区BLOCK,入口参数指定块号
}

void Bulk_Receive_Data()		  /*计算机到U盘*/
{
	unsigned int i=0;
	//Copy(pCurrentBuf+nBufCount,Bulk_Buffer,EP2_PACKET_SIZE);
	nBufCount+=EP2_PACKET_SIZE;
	//pCurrentBuf
	usb_buf+=EP2_PACKET_SIZE;
								 
	if(nBufCount==512)							//如果一扇区内容接收完毕
	{
		AES_jiami(Bulk_Buffer.Bulk_PACKET,Bulk_Buffer.Bulk_PACKET);						   			  // 在这里添加加密函数
		if(nBegainSectorOffsetInPage!= 0)  //处理如果不是从页的第1个扇区开始位置写
		{
			ulong ulTempSectorCount;
			 //计算当前扇区的绝对位置
			ulTempSectorCount = nCurrentBlock *(256)+ nBegainPage*4 + nBegainSectorOffsetInPage;

			ulTempSectorCount--;	//查找前1个扇区的位置
			if(nBegainSectorOffsetInPage == 1)
			{
				  NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff);
			}
			else if(nBegainSectorOffsetInPage == 2)
			{
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+512);
				ulTempSectorCount--;
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff);
			}
			else if(nBegainSectorOffsetInPage == 3)
			{
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+1024);
				ulTempSectorCount--;
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+512);
				ulTempSectorCount--;
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff);

			}
			//
			nBegainSectorOffsetInPage =0;

		}

		//LBA.bits.block = Bufer_Block;
		if(nCurrentSectorOffsetInPage == 0)
		{
			uint uiTemp;

			for(uiTemp=0; uiTemp<512; uiTemp++)
			{
				ucNFPageBuff[uiTemp] = Bulk_Buffer.Bulk_PACKET[uiTemp];
			}
			nCurrentSectorOffsetInPage++;
		}
		else if(nCurrentSectorOffsetInPage == 1)
		{
			uint uiTemp;

			for(uiTemp=0; uiTemp<512; uiTemp++)
			{
				ucNFPageBuff[uiTemp+512] = Bulk_Buffer.Bulk_PACKET[uiTemp];
			}
			nCurrentSectorOffsetInPage++;
		}
		else if(nCurrentSectorOffsetInPage == 2)
		{
			uint uiTemp;

			for(uiTemp=0; uiTemp<512; uiTemp++)
			{
				ucNFPageBuff[uiTemp+1024] = Bulk_Buffer.Bulk_PACKET[uiTemp];
			}
			nCurrentSectorOffsetInPage++;
		}
		else if(nCurrentSectorOffsetInPage == 3)
		{
			uint uiTemp;

			for(uiTemp=0; uiTemp<512; uiTemp++)
			{
				ucNFPageBuff[uiTemp+1536] = Bulk_Buffer.Bulk_PACKET[uiTemp];
			}


			NandFlash_WritePage((Bufer_Block*64+nCurrentPage)*4l);
			nCurrentSectorOffsetInPage = 0;
			nCurrentPage++;							//页指针加1

		}

	//	K9FProgramData(Bulk_Buffer.Bulk_PACKET);
		//Flash_Write_Page(Bufer_Block,nCurrentPage,Bulk_PACKET,512);	//将数据暂时写到缓冲区BLOCK	
		LBA.val++;
		Transfer_Length--;						//待传输数据长度减1

		nBufCount=0;							//清空数据缓冲区
		usb_buf = Bulk_Buffer.Bulk_PACKET;
	}
//if(Transfer_Length==0x18)
//{
//	i++;					   
//}
	if((Transfer_Length>0)&&(nCurrentPage == 64))		//如果一个BLOCK写完毕
	{
		
		LBA.bits.page=0;
		for(i=0;i<nBegainPage;i++)
		{
			//LBA.bits.block = nCurrentBlock;
			//K9FReadPageData();
			//LBA.bits.block = Bufer_Block;
			//K9FProgramData(Bulk_READ);
			
			NF_ReadPage((nCurrentBlock*64+i)*4l);
			NandFlash_WritePage((Bufer_Block*64+i)*4l);
	//		LBA.val++;
			//Flash_Read_Page(nCurrentBlock,i,Bulk_PACKET,512);
			//Flash_Write_Page(Bufer_Block,i,Bulk_PACKET,512);
		}
		//K9FEraseBlockData(nCurrentBlock<<5);
		NandFlash_EraseBlock(nCurrentBlock);			//擦除缓冲区BLOCK,入口参数指定块号
	
		
		LBA.bits.page = 0;
		//将缓冲数据块中的内容拷贝会当前有效数据块
		for(i=0;i<64;i++)
		{
	//		LBA.bits.block = Bufer_Block;
	//		K9FReadPageData();
	//		LBA.bits.block = nCurrentBlock;
	//		K9FProgramData(Bulk_READ);
	//		LBA.val++;
			NF_ReadPage((Bufer_Block *64+i)*4l);
			NandFlash_WritePage((nCurrentBlock*64+i)*4l);
		}
	//	nCurrentBlock=LBA.bits.block;
		nCurrentBlock++;		//指向下1个数据块
		nCurrentPage=0;
		nBufCount=0;
		nBegainPage=0;
//		LBA.bits.block = Bufer_Block;
//		K9FEraseBlockData(Bufer_Block<<5);			//擦除缓冲区BLOCK
		NandFlash_EraseBlock(Bufer_Block);			//擦除缓冲区BLOCK,入口参数指定块号
		
	}

	if(Transfer_Length==0)
	{  //最后1包的处理
		LBA.bits.page=0;

	   //处理当前页内后半部分，1页中有4个扇区，将当前扇区索引号到页尾的内容拷贝到待写页中
		if(nCurrentSectorOffsetInPage!= 0)  //处理当前页内扇区索引号为1，2，3的情况
		{
			ulong ulTempSectorCount;
			 //计算当前扇区的绝对位置
			ulTempSectorCount = nCurrentBlock *(256)+ nCurrentPage*4 + nCurrentSectorOffsetInPage;
	
	//		ulTempSectorCount--;	//查找前1个扇区的位置
			//读取USB数据填充到写缓冲区时，扇区偏移已经加1了。
			if(nCurrentSectorOffsetInPage == 1)
			{
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+512);
				
				ulTempSectorCount++;
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+1024);

				ulTempSectorCount++;
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+1536);

			}
			else if(nCurrentSectorOffsetInPage == 2)
			{
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+1024);

				ulTempSectorCount++;
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+1536);
			}
			else if(nCurrentSectorOffsetInPage == 3)
			{
				NF_ReadSectorToBuff(ulTempSectorCount, ucNFPageBuff+1536);	
			}
			//将当前页写入缓冲扇区区
			NandFlash_WritePage((Bufer_Block*64+nCurrentPage)*4l);
			nCurrentPage++;			//指向下一页

			nCurrentSectorOffsetInPage =0;
		}		

		//处理前半部分页
		for(i=0;i<nBegainPage;i++)
		{
//			LBA.bits.block = nCurrentBlock;
//			K9FReadPageData();
//			LBA.bits.block = Bufer_Block;
//			K9FProgramData(Bulk_READ);
//			LBA.val++;
			NF_ReadPage((nCurrentBlock*64+i)*4l);
			NandFlash_WritePage((Bufer_Block*64+i)*4l);

		}
		LBA.bits.page=nCurrentPage;

		//处理后半部分页面
		for(i=nCurrentPage;i<64;i++)
		{
//			LBA.bits.block = nCurrentBlock;
//			K9FReadPageData();
//			LBA.bits.block = Bufer_Block;
//			K9FProgramData(Bulk_READ);
//			LBA.val++;
			NF_ReadPage((nCurrentBlock*64+i)*4l);
			NandFlash_WritePage((Bufer_Block*64+i)*4l);
		}

//		K9FEraseBlockData(nCurrentBlock<<5);
		NandFlash_EraseBlock(nCurrentBlock);			//擦除用户待改写区BLOCK,入口参数指定块号

		LBA.bits.page = 0;
		for(i=0;i<64;i++)
		{
//			LBA.bits.block = Bufer_Block;
//			K9FReadPageData();
//			LBA.bits.block = nCurrentBlock;
//			K9FProgramData(Bulk_READ);
//			LBA.val++;
			NF_ReadPage((Bufer_Block *64+i)*4l);
			NandFlash_WritePage((nCurrentBlock*64+i)*4l);
		}
		TransCSW();	                                  
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
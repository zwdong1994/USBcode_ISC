#ifndef _USB_SCSI_DESCREPTOR_H_
#define _USB_SCSI_DESCREPTOR_H_


code unsigned char Device_Capacity[8]=				//����Ϊ16M 00007d00  ����Ϊ32M 0000ff00 
{
//	0x00,0x00,0xff,0x00,	0x00,0x00,0x02,0x00		//ǰ4�ֽ�Ϊ����������4�ֽ�Ϊÿ�����Ĵ�С
	0x00,0x03,0xE8,0x00,	0x00,0x00,0x02,0x00		//ǰ4�ֽ�Ϊ����������4�ֽ�Ϊÿ�����Ĵ�С,256000������
};	
//�����3������������������壬���Բο����U�̷�����Ӧ�����ݼ���
code unsigned char Device_InquiryData[36]=
{
	0x00,0x80,0x02,0x02  ,0x1f,0x00,0x00,0x00  ,0x47,0x65,0x6e,0x65  ,0x72,0x69,0x63,0x20,	//........Generic
	0x55,0x53,0x42,0x20  ,0x46,0x6c,0x61,0x73  ,0x68,0x20,0x44,0x72  ,0x69,0x76,0x65,0x20,	//USB Flash Drive
	0x25,0x7a,0x21,0x59																		//$Z!Y
};	
code unsigned char Device_ModeSense[12]=
{
	0x0b,0x00,0x00,0x08  ,0x00,0x00,0x00,0x00  ,0x00,0x00,0x02,0x00
};
code unsigned char Vender[12]=
{
	0x00,0x00,0x00,0x08	,0x00,0x03,0xe3,0x00  ,0x02,0x00,0x02,0x00
};
//////////////////////////////////////////////////////////////////////////////////////////////////
#endif
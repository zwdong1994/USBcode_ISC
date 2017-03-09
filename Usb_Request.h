#ifndef _USB_REQUEST_H_   
#define _USB_REQUEST_H_  

/////////////////////////////////////////////////////////////////////////////////////////////
//标准请求
#define  GET_STATUS              0x00        // Code for Get Status
#define  CLEAR_FEATURE           0x01        // Code for Clear Feature
#define  SET_FEATURE             0x03        // Code for Set Feature
#define  SET_ADDRESS             0x05        // Code for Set Address
#define  GET_DESCRIPTOR          0x06        // Code for Get Descriptor
#define  SET_DESCRIPTOR          0x07        // Code for Set Descriptor(not used)
#define  GET_CONFIGURATION       0x08        // Code for Get Configuration
#define  SET_CONFIGURATION       0x09        // Code for Set Configuration
#define  GET_INTERFACE           0x0A        // Code for Get Interface
#define  SET_INTERFACE           0x0B        // Code for Set Interface
#define  SYNCH_FRAME             0x0C        // Code for Synch Frame(not used)
//这个是U盘用命令
#define  GET_MAXLUN              0xFE		 // 此命令用来获取U盘支持的逻辑单元的数量
//////////////////////////////////////////////////////////////////////////////////////////////
//SCSI命令
#define  INQUIRY 					0x12
#define  MODE_SENSE 				0x1a
#define  READ10 					0x28
#define  READ_CAPACITY 				0x25
#define  TEST_UNIT_READY	 		0x00
#define  VERIFY 					0x2f		
#define  WRITE10  	 				0x2a
#define  MEDIUM_REMOVAL 			0x1e

#define  Vender_Order			    0x23		//厂商自定义的命令
////////////////////////////////////////////////////////////////////////////////////////////////
#endif
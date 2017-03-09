#ifndef _USB_PARAMETER_H_
#define _USB_PARAMETER_H_
//标准描述符的类型
#define  DSC_DEVICE			0x01        // Device Descriptor
#define  DSC_CONFIG         0x02        // Configuration Descriptor
#define  DSC_STRING         0x03        // String Descriptor
#define  DSC_INTERFACE      0x04        // Interface Descriptor
#define  DSC_ENDPOINT       0x05        // Endpoint Descriptor

//HID 设备描述符类型
#define DSC_HID				0x21		// HID Class Descriptor
#define DSC_HID_REPORT		0x22		// HID Report Descriptor
//端点的大小
#define  EP0_PACKET_SIZE    0x40
#define  EP1_PACKET_SIZE    0x40      
#define  EP2_PACKET_SIZE    0x40  

//设备的状态
#define  DEV_ATTACHED       0x00        // Device is in Attached State
#define  DEV_POWERED        0x01        // Device is in Powered State
#define  DEV_DEFAULT        0x02        // Device is in Default State
#define  DEV_ADDRESS        0x03        // Device is in Addressed State
#define  DEV_CONFIGURED     0x04        // Device is in Configured State
#define  DEV_SUSPENDED      0x05        // Device is in Suspended State

//端点的状态
#define  EP_IDLE            0x00        // This signifies Endpoint Idle State
#define  EP_TX              0x01        // Endpoint Transmit State
#define  EP_RX              0x02        // Endpoint Receive State
#define  EP_HALT            0x03        // Endpoint Halt State (return stalls)
#define  EP_STALL           0x04        // Endpoint Stall (send procedural stall next status phase)
#define  EP_ADDRESS         0x05        // Endpoint Address (change FADDR during next status phase)

//BULK_ONLY传输的状态
#define BULK_IDLE  			0x00
#define BULK_DATA_TRANS		0x01
#define BULK_DATA_RECIEVE 			0x02
#define BULK_DATA_END 		0x03
/////////////////////////////////////////////////////////////////////////////////////////////////////
// 设置阶段的命令数据包
// Define bmRequestType bitmaps
#define  OUT_DEVICE              0x00        // Request made to device, direction is OUT
#define  IN_DEVICE               0x80        // Request made to device, direction is IN 
#define  OUT_INTERFACE           0x01        // Request made to interface, direction is OUT
#define  IN_INTERFACE            0x81        // Request made to interface, direction is IN
#define  OUT_ENDPOINT            0x02        // Request made to endpoint, direction is OUT
#define  IN_ENDPOINT             0x82        // Request made to endpoint, direction is IN

// Define wIndex bitmaps
#define  IN_EP1                  0x81        // Index values used by Set and Clear feature
#define  OUT_EP1                 0x01        // commands for Endpoint_Halt
#define  IN_EP2                  0x82
#define  OUT_EP2                 0x02

// Define wValue bitmaps for Standard Feature Selectors
#define  DEVICE_REMOTE_WAKEUP    0x01        // Remote wakeup feature(not used)
#define  ENDPOINT_HALT           0x00        // Endpoint_Halt feature selector

/////////////////////////////////////////////////////////////////////////////////////////////////////
#define  Bufer_Block			 1010 	     // 写FLASHBlock时的缓冲区，为FLASH的第1021个Block
//#define  Bufer_Block			 2044          //32M 2048 Block
#endif
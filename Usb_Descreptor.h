#ifndef _USB_DESCREPTOR_H_
#define _USB_DESCREPTOR_H_

//…Ë±∏√Ë ˆ∑˚
code unsigned char DeviceDesc[18] = 
{
   	18,                        // bLength
   	0x01,                      // bDescriptorType
   	0x00, 0x02,                // bcdUSB (lsb first)
   	0x00,                      // bDeviceClass
   	0x00,                      // bDeviceSubClass
   	0x00,                      // bDeviceProtocol
   	64,                        // bMaxPacketSize0
   	0x43,0x10,
   	0x01,0x81,
   	0x00,0x01,
	0x04,
	0x0b,
	0x1b,
   	0x01                       // bNumConfigurations; //end of DeviceDesc
};

code unsigned char ConfigDesc[32] =
{

// ≈‰÷√√Ë ˆ∑˚
   0x09,                      // Length
   0x02,                      // Type
   0x20, 0x00,                // TotalLength (lsb first)
   0x01,                      // NumInterfaces
   0x01,                      // bConfigurationValue
   0x00,                      // iConfiguration
   0x80,                      // bmAttributes (no remote wakeup)
   0x64,                      // MaxPower (*2mA)

// Ω”ø⁄√Ë ˆ∑˚
   0x09,                      // bLength
   0x04,                      // bDescriptorType
   0x00,                      // bInterfaceNumber
   0x00,                      // bAlternateSetting
   0x02,                      // bNumEndpoints
   0x08,                      // bInterfaceClass
   0x06,                      // bInterfaceSubClass
   0x50,                      // bInterfaceProcotol
   0x00,                      // iInterface

// ∂Àµ„1√Ë ˆ∑˚
   0x07,                      // bLength
   0x05,                      // bDescriptorType
   0x81,                      // bEndpointAddress (ep1, IN)
   0x02,                      // bmAttributes (Bulk)
   0x40, 0x00,                // wMaxPacketSize (lsb first)
   0xff,                      // bInterval

// ∂Àµ„2√Ë ˆ∑˚
   0x07,                      // bLength
   0x05,                      // bDescriptorType
   0x02,                      // bEndpointAddress (ep2, OUT)
   0x02,                      // bmAttributes (Bulk)
   0x40, 0x00,                // wMaxPacketSize (lsb first)
   0xff                       // bInterval
};

code unsigned char StringDes[65]=
{
//”Ô—‘√Ë ˆ∑˚
	0x04,
	0x03,
	0x09,0x04,

//÷∆‘Ï…Ã√Ë ˆ∑˚
	0x07,0x03,
	0x43,0x68,0x69,0x6e,0x61,		//China

//≤˙∆∑√Ë ˆ∑˚
	0x10,0x03,
	0x46,0x4c,0x41,0x53,0x48,0x20,0x44,0x69,0x73,0x6b,0x20,0x32,0x2e,0x30,	//FLASH_Disk_2.0

//≤˙∆∑–Ú∫≈√Ë ˆ∑˚
	0x26,0x03,
	0x00,0x80,0x02,0x02,0x1f,0x00,0x00,0x00,0x44,0x49,0x59,0x30,0x30,0x31,	//........DIY001
	0x20,0x20,0x46,0x4c,0x41,0x53,0x48,0x20,0x44,0x49,0x53,0x4b,0x20,0x20,  //..FLASH DISK..
	0x20,0x20,0x20,0x20,0x32,0x2e,0x30,0x30 								//..........2.00
};
///////////////////////////////////////////////////////////////////////////////////////////////


#endif
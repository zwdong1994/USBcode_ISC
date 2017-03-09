/*------------------------------------------------------------------
* CopyRight (C) 2005,江苏大唐电子产品有限公司
* All Rights Reserved.
*
* 文件名称：DataType.h
* 文件标识：
* 摘    要：数据类型缩写定义
* 
* 当前版本：1.0
* 作    者：熊刚
* 完成日期：2005年10月7日
*
* 取代版本： 
* 原作者  ：
* 完成日期：
--------------------------------------------------------------------*/

#ifndef	DATA_TYPE_H
#define DATA_TYPE_H		

#define uchar  unsigned char
#define uint   unsigned int
#define ulong  unsigned long

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned int   INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   int   INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 16-bit wide                    */

#define BYTE           INT8S                     /* Define data types for backward compatibility ...   */
#define UBYTE          INT8U                     /* ... to uC/OS V1.xx.  Not actually needed for ...   */
#define WORD           INT16S                    /* ... uC/OS-II.                                      */
#define UWORD          INT16U
#define LONG           INT32S
#define ULONG          INT32U


#define  uint8  unsigned char
#define uint32  unsigned long
#define uint16  unsigned int

#define int32  long
#define bool   bit
//#define bool     char
#define c_True   1
#define c_False  0

#define true 1
#define false 0

#define	UINT8		unsigned char
#define	UINT16		unsigned short
#define	UINT32		unsigned long

#endif


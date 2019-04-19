//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : Basic Data Type Define
// 
// $Source: /home/cvsroot/TAVI020/tavi/include/TypeDef.h,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// Author: Sang-jin Ryu (etnlwind@newmedialife.com, etnlwind@naver.com)
//
//*****************************************************************************
// $Log: TypeDef.h,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.3  2005/12/22 05:13:43  etnlwind
// Update for New Version
//
// Revision 1.2  2005/10/13 01:25:25  etnlwind
// Update for Header Info
//
//*****************************************************************************


/*
 * TAVI-PMP Version 1.0
 * new media LIFE CO., LTD.
 * TypeDef.h
 * Sang-jin Ryu (etnlwind@newmedialife.com)
 *
 */


#ifndef ___TYPEDEF_H__
#define ___TYPEDEF_H__

//typedef unsigned char			BOOL;
typedef unsigned char			U8;
typedef unsigned short			U16;
typedef unsigned long			U32;
typedef unsigned long long		U64;
typedef unsigned char			U128[16];		// Unsigned 128-bits
typedef signed char			S8;
typedef signed short			S16;
typedef signed long			S32;
typedef float					F32;
typedef double				D32;


typedef volatile unsigned char          REG8;
typedef volatile unsigned short         REG16;
typedef volatile unsigned long          REG32;

 
#ifndef NULL                                        
    #define NULL                            (void *)0x0
#endif
#define TRUE                            1
#define FALSE                           0
#define ON                              	TRUE        
#define OFF                             	FALSE       
#define YES                             	TRUE        /**< Signal not to use something */
#define NO                              	FALSE       /**< Signal to use something */ 
#define SUPPORT                       TRUE
#define NOT_SUPPORT               FALSE
#define EOF                             (-1)
 
#define MAX_U8                          255
#define MAX_S8                          127
#define MIN_S8                          (-128)
                                        
#define MAX_U16                         65535
#define MAX_S16                         32767 
#define MIN_S16                         (-32768)
                                        
#define MAX_U32                         0xffffffff
#define MAX_S32                         2147483647
#define MIN_S32                         (-2147483647-1)
                                        
#define SIZE_1K                         (1024)            
#define SIZE_2K                         ( 2*SIZE_1K)            
#define SIZE_4K                         ( 4*SIZE_1K)            
#define SIZE_8K                         ( 8*SIZE_1K)
#define SIZE_16K                        (16*SIZE_1K)
#define SIZE_32K                        (32*SIZE_1K)
#define SIZE_64K                        (64*SIZE_1K)
#define SIZE_128K                       (128*SIZE_1K)
#define SIZE_256K                       (256*SIZE_1K)
#define SIZE_512K                       (512*SIZE_1K)
                                        
#define SIZE_1M                         (1024*SIZE_1K)
#define SIZE_2M                         ( 2*SIZE_1M)
#define SIZE_4M                         ( 4*SIZE_1M)            
#define SIZE_8M                         ( 8*SIZE_1M)
#define SIZE_16M                        (16*SIZE_1M)
#define SIZE_32M                        (32*SIZE_1M)
#define SIZE_64M                        (64*SIZE_1M)
#define SIZE_128M                       (128*SIZE_1M)
#define SIZE_256M                       (256*SIZE_1M)
#define SIZE_512M                       (512*SIZE_1M)
                                        
#define SIZE_1G                         (1024*SIZE_1M)
 
#endif /* __TYPEDEF_H__ */




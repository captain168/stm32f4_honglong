/****************************************************************************
* Copyright (C), 2009-2010, www.armfly.com
*
* 文件名: ASC16.h
* 内容简述: ASCII字符点阵头文件
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.1    2010-01-04 armfly  创建该文件
*
*/

#ifndef __ASCII16_H_
#define __ASCII16_H_

extern unsigned char const Ascii16[];

extern unsigned char const g_Hz16[];

#define HZK16_ADDR 0x803EA00		/* 汉字库地址 */

#define USE_SMALL_FONT	1	/* 1表示使用小字库 */

#define HZ_COUNT 66		/* 汉字个数 */

#endif

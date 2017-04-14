/*****************************************************************************
*  @file     LeakDetector.h       
*  @brief    使用Crt的方式来检测内存泄漏                                               
*  @note   
*  通过“_CrtSetBreakAlloc(132);”语句来对某次动态内存分配打下断点  
*  
*
*----------------------------------------------------------------------------*
*  Change History :
* <Version> |                       <Description>
*----------------------------------------------------------------------------*
*  0.0.0.1  |                        创建头文件
*----------------------------------------------------------------------------*
*
*****************************************************************************/
#ifndef __LEAKDETECTOR_H
#define __LEAKDETECTOR_H
#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

/** 
* @brief: 开始检测内存泄漏
* @return 无
* @note   
* 通常在主函数的首行里调用此函数，注意，仅在Debug下有效
*/
void EnableMemLeakCheck();
#endif


/*****************************************************************************
*  @file     LeakDetector.h       
*  @brief    ʹ��Crt�ķ�ʽ������ڴ�й©                                               
*  @note   
*  ͨ����_CrtSetBreakAlloc(132);���������ĳ�ζ�̬�ڴ������¶ϵ�
*  ��չ֪ʶ��
*  �������õĹ��߻���vld��Bounds Checker��
*****************************************************************************/
#ifndef __LEAKDETECTOR_H
#define __LEAKDETECTOR_H
#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

/** 
* @brief: ��ʼ����ڴ�й©
* @return ��
* @note   
* ͨ��������������������ô˺�����ע�⣬����Debug����Ч
*/
void EnableMemLeakCheck();
#endif


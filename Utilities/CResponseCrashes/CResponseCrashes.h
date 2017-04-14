/*****************************************************************************
*  @file     DumpCallStack.h
*  @brief    在程序崩溃退出前自定义处理
*  @note
*  主要目的是为了弹出Dump文件，方便开发者跟踪导致程序崩溃的起因
*  源码来源于：《让程序在崩溃时体面的退出之总结》
*  链接：http://blog.csdn.net/starlee/article/details/6662011
*  MSDN相关介绍：https://msdn.microsoft.com/en-us/library/windows/desktop/ms679294(v=vs.85).aspx
*  此库需要兼容VC 6.0，最好是通过动态调用的形式来直接使用此库
*****************************************************************************/
#ifndef __CRESPONSECRASHES_H
#define __CRESPONSECRASHES_H

#include <windows.h>

#if (_MSC_VER>1200)
#include <DbgHelp.h>  
#pragma comment(lib, "dbghelp.lib")
#else
#ifndef _DBGHELP_
//VC 6.0时，Windows SDK并没有提供DbgHelp库
//类似的情况还有Gdiplus等
#include "DbgHelp/DbgHelp.h"
#pragma comment(lib,"Misc/CResponseCrashes/DbgHelp/DbgHelp.lib")
#endif
#endif


static const int nMaxLenOfAddress = 32;
static const int nMaxLenOfName = 1024;

/**
* @class CResponseCrashes
* @brief 程序崩溃处理库                                                                                         
* @note   
* 此类使用了单例模式，需要通过GetInstance()来访问成员函数
*/
class CResponseCrashes
{
private:
	CResponseCrashes(){}

public:
	static CResponseCrashes &GetInstance();
	void MonitorApplicationCrash();
	
public:
	/** @brief 崩溃信息 */
	struct CrashInfo
	{
		char ErrorCode[nMaxLenOfAddress];	/**< 错误码 */
		char Address[nMaxLenOfAddress];		/**< 崩溃时访问到的内存地址 */
		char Flags[nMaxLenOfAddress];		/**< value */
	};

	/** @brief 调用堆栈信息 */
	struct CallStackInfo
	{
		char ModuleName[nMaxLenOfName];		/**< 模块名称 */
		char MethodName[nMaxLenOfName];		/**< 函数名称 */
		char FileName[nMaxLenOfName];		/**< 文件名称 */
		char LineNumber[nMaxLenOfName];		/**< 代码行 */
	};

	/**
	* @brief: 处理Unhandled Exception的回调函数
	* @param[in] EXCEPTION_POINTERS*  pException  指向程序崩溃时所对应的异常信息
	* @return LONG 返回值为EXCEPTION_EXECUTE_HANDLER
	* @note
	*
	*/
	static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);
};

#endif

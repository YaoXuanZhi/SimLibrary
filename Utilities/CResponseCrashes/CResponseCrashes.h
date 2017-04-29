/*****************************************************************************
*  @file     DumpCallStack.h
*  @brief    �ڳ�������˳�ǰ�Զ��崦��
*  @note
*  ��ҪĿ����Ϊ�˵���Dump�ļ������㿪���߸��ٵ��³������������
*  Դ����Դ�ڣ����ó����ڱ���ʱ������˳�֮�ܽᡷ
*  ���ӣ�http://blog.csdn.net/starlee/article/details/6662011
*  MSDN��ؽ��ܣ�https://msdn.microsoft.com/en-us/library/windows/desktop/ms679294(v=vs.85).aspx
<<<<<<< HEAD
*  ��չ���ϣ���TortoiseGit��Դ��Ŀ֮�У�ext\CrashServerĿ¼�£�ר��Ϊ�������ʱ�ռ���Ϣ��д��һ��
*  ���----Doctor Dump SDK��������ۣ�https://drdump.com/crash-reporting-system �� 
*  http://www.geardownload.com/development/crashrpt-cpp-download.html
=======
*  ��չ���ӣ�https://drdump.com/crash-reporting-system
*  �˿���Ҫ����VC 6.0�������ͨ����̬���õ���ʽ��ֱ��ʹ�ô˿�
>>>>>>> 8322139f7e130b58a142d2f20da6fea663e4afd4
*****************************************************************************/
#ifndef __CRESPONSECRASHES_H
#define __CRESPONSECRASHES_H

#include <windows.h>

#if (_MSC_VER>1200)
#include <DbgHelp.h>  
#pragma comment(lib, "dbghelp.lib")
#else
#ifndef _DBGHELP_
//VC 6.0ʱ��Windows SDK��û���ṩDbgHelp��
//���Ƶ��������Gdiplus��
#include "DbgHelp/DbgHelp.h"
#pragma comment(lib,"Misc/CResponseCrashes/DbgHelp/DbgHelp.lib")
#endif
#endif

static const int nMaxLenOfAddress = 32;
static const int nMaxLenOfName = 1024;

/**
* @class CResponseCrashes
* @brief ������������                                                                                         
* @note   
* ����ʹ���˵���ģʽ����Ҫͨ��GetInstance()�����ʳ�Ա����
*/
class CResponseCrashes
{
private:
	CResponseCrashes(){}

public:
	static CResponseCrashes &GetInstance();
<<<<<<< HEAD
	void StartMonitor();
=======

	/** 
	* @brief: ��ʼ��س������
	* @return ��
	* @note   
	* 
	*/
	void MonitorApplicationCrash();
>>>>>>> 8322139f7e130b58a142d2f20da6fea663e4afd4
	
public:
	/** @brief ������Ϣ */
	struct CrashInfo
	{
		char ErrorCode[nMaxLenOfAddress];	/**< ������ */
		char Address[nMaxLenOfAddress];		/**< ����ʱ���ʵ����ڴ��ַ */
		char Flags[nMaxLenOfAddress];		/**< value */
	};

	/** @brief ���ö�ջ��Ϣ */
	struct CallStackInfo
	{
		char ModuleName[nMaxLenOfName];		/**< ģ������ */
		char MethodName[nMaxLenOfName];		/**< �������� */
		char FileName[nMaxLenOfName];		/**< �ļ����� */
		char LineNumber[nMaxLenOfName];		/**< ������ */
	};

	/**
	* @brief: ����Unhandled Exception�Ļص�����
	* @param[in] EXCEPTION_POINTERS*  pException  ָ��������ʱ����Ӧ���쳣��Ϣ
	* @return LONG ����ֵΪEXCEPTION_EXECUTE_HANDLER
	* @note
	*
	*/
	static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);
};

#endif

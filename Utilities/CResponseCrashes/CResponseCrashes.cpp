#include "CResponseCrashes.h"
#include <vector>
#include <fstream>
#include <time.h>
#pragma warning(disable:4996)

static void SafeStrCpy(char* szDest, size_t nMaxDestSize, const char* szSrc)
{
	if (nMaxDestSize <= 0) return;
	if (strlen(szSrc) < nMaxDestSize)
	{
		strcpy(szDest, szSrc);
	}
	else
	{
		strncpy(szDest, szSrc, nMaxDestSize);
		szDest[nMaxDestSize - 1] = '\0';
	}
}

static char* GetProcessName()
{
	static char szFileFullPath[MAX_PATH] = { 0 }, szProcessName[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH);
	size_t length = strlen(szFileFullPath);
	for (size_t i = length - 1; i >= 0; i--)//从路径后面开始找\，即倒着找右斜杠  
	{
		if (szFileFullPath[i] == '\\')//找到第一个\，就可以马上获取进程名称了  
		{
			i++;
			for (size_t j = 0; i <= length; j++)//结束符\0不能少,即i=length  
			{
				szProcessName[j] = szFileFullPath[i++];
			}
			break;
		}
	}
	return szProcessName;
}

/**
* @brief: 创建Dump文件
* @param[in] LPCWSTR  lpstrDumpFilePathName  被创建的Dump文件的所在路径
* @param[in] EXCEPTION_POINTERS*  pException  指向程序崩溃时所对应的异常信息
* @return 无
* @note
*
*/
static void CreateDumpFile(const char* lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	//创建Dump文件  
	HANDLE hDumpFile = CreateFileA(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//Dump信息  
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	//写入Dump文件内容  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}

/**
* @brief: 得到程序崩溃信息
* @param[in] const  EXCEPTION_RECORD*  pRecord  包含崩溃信息的结构体
* @return CrashInfo 获得转换后的数据
* @note
* 其实就是将EXCEPTION_RECORD类型转换为CrashInfo类型
*/
static CResponseCrashes::CrashInfo GetCrashInfo(const EXCEPTION_RECORD* pRecord)
{
	CResponseCrashes::CrashInfo crashinfo;
	SafeStrCpy(crashinfo.Address, nMaxLenOfAddress, "N/A");
	SafeStrCpy(crashinfo.ErrorCode, nMaxLenOfAddress, "N/A");
	SafeStrCpy(crashinfo.Flags, nMaxLenOfAddress, "N/A");

	sprintf(crashinfo.Address, "%p", pRecord->ExceptionAddress);
	sprintf(crashinfo.ErrorCode, "%08X", pRecord->ExceptionCode);
	sprintf(crashinfo.Flags, "%08X", pRecord->ExceptionFlags);
	return crashinfo;
}

/**
* @brief: 得到CallStack信息
* @param[in] const CONTEXT*  pContext  包含程序异常时的上下文信息
* @return std::vector<CallStackInfo> 返回程序崩溃时的整体堆栈信息
* @note
* pContext被包含在EXCEPTION_POINTERS结构体内
*/
static std::vector<CResponseCrashes::CallStackInfo> GetCallStack(const CONTEXT* pContext)
{
	HANDLE hProcess = GetCurrentProcess();
	SymInitialize(hProcess, NULL, TRUE);
	std::vector<CResponseCrashes::CallStackInfo> arrCallStackInfo;
	CONTEXT c = *pContext;
	STACKFRAME64 sf;
	memset(&sf, 0, sizeof(STACKFRAME64));
	DWORD dwImageType = IMAGE_FILE_MACHINE_I386;

	//不同的CPU类型，具体信息可查询MSDN  
#ifdef _M_IX86  
	sf.AddrPC.Offset = c.Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;
#elif _M_X64  
	dwImageType = IMAGE_FILE_MACHINE_AMD64;
	sf.AddrPC.Offset = c.Rip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Rsp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Rsp;
	sf.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64  
	dwImageType = IMAGE_FILE_MACHINE_IA64;
	sf.AddrPC.Offset = c.StIIP;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.IntSp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrBStore.Offset = c.RsBSP;
	sf.AddrBStore.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.IntSp;
	sf.AddrStack.Mode = AddrModeFlat;
#else  
#error "Platform not supported!"  
#endif  

	HANDLE hThread = GetCurrentThread();

	bool bIsLoop = true;
	while (bIsLoop)
	{
		//该函数是实现这个功能的最重要的一个函数  
		//函数的用法以及参数和返回值的具体解释可以查询MSDN  
		if (!StackWalk64(dwImageType, hProcess, hThread, &sf, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
		{
			bIsLoop = false;
			break;
		}

		if (sf.AddrFrame.Offset == 0)
		{
			bIsLoop = false;
			break;
		}

		CResponseCrashes::CallStackInfo callstackinfo;
		SafeStrCpy(callstackinfo.MethodName, nMaxLenOfName, "N/A");
		SafeStrCpy(callstackinfo.FileName, nMaxLenOfName, "N/A");
		SafeStrCpy(callstackinfo.ModuleName, nMaxLenOfName, "N/A");
		SafeStrCpy(callstackinfo.LineNumber, nMaxLenOfName, "N/A");

		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + nMaxLenOfName];
		IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;
		memset(pSymbol, 0, sizeof(IMAGEHLP_SYMBOL64) + nMaxLenOfName);

		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = nMaxLenOfName;

		//得到函数名  
		if (SymGetSymFromAddr64(hProcess, sf.AddrPC.Offset, NULL, pSymbol))
		{
			SafeStrCpy(callstackinfo.MethodName, nMaxLenOfName, pSymbol->Name);
		}

		IMAGEHLP_LINE64 lineInfo;
		memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));

		lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD dwLineDisplacement;

		//得到文件名和所在的代码行  
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
		{
			SafeStrCpy(callstackinfo.FileName, nMaxLenOfName, lineInfo.FileName);
			sprintf(callstackinfo.LineNumber, "%d", lineInfo.LineNumber);
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));

		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

		//得到模块名  
		if (SymGetModuleInfo64(hProcess, sf.AddrPC.Offset, &moduleInfo))
		{
			SafeStrCpy(callstackinfo.ModuleName, nMaxLenOfName, moduleInfo.ModuleName);
		}

		arrCallStackInfo.push_back(callstackinfo);
	}

	SymCleanup(hProcess);
	return arrCallStackInfo;
}

CResponseCrashes & CResponseCrashes::GetInstance()
{
	static CResponseCrashes rcObj;
	return rcObj;
}

void CResponseCrashes::MonitorApplicationCrash()
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}

LONG CResponseCrashes::ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	time_t CurTime;
	time(&CurTime);
	struct tm* timeinfo;
	timeinfo = localtime(&CurTime);
	char szCurrentDate[128] = { 0 };
	strftime(szCurrentDate, 128, "%Y-%m-%d %H%M%S", timeinfo);

	//获得本程序的名称
	char szDumpName[MAX_PATH] = { 0 };
	sprintf(szDumpName, "%s(%s).dmp", GetProcessName(), szCurrentDate);
	CreateDumpFile(szDumpName, pException);

	//确保有足够的栈空间  
#ifdef _M_IX86  
	if (pException->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
	{
		static char TempStack[1024 * 128];
		__asm mov eax, offset TempStack[1024 * 128];
		__asm mov esp, eax;
	}
#endif    

	//创建当前异常日志
	std::ofstream logfile;
	char szErrorLogName[MAX_PATH] = { 0 };
	sprintf(szErrorLogName, "CallCrash(%s).log", szCurrentDate);
	logfile.open(szErrorLogName);

	CResponseCrashes::CrashInfo crashinfo = GetCrashInfo(pException->ExceptionRecord);

	//输出Crash信息  
	logfile << "ExceptionAddress: " << crashinfo.ErrorCode << std::endl;
	logfile << "ExceptionCode: " << crashinfo.Address << std::endl;
	logfile << "ExceptionFlags: " << crashinfo.Flags << std::endl;

	std::vector<CResponseCrashes::CallStackInfo> arrCallStackInfo = GetCallStack(pException->ContextRecord);

	//输出CallStack  
	logfile << "CallStack: " << std::endl;
	for (std::vector<CResponseCrashes::CallStackInfo>::iterator i = arrCallStackInfo.begin(); i != arrCallStackInfo.end(); ++i)
	{
		CResponseCrashes::CallStackInfo callstackinfo = (*i);
		logfile << callstackinfo.MethodName << "() : [" << callstackinfo.ModuleName << "] (File: " << callstackinfo.FileName << " @Line " << callstackinfo.LineNumber << ")" << std::endl;
	}

	//输出最近的错误异常码
	//显示错误信息
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		NULL
		);
	logfile << "GetLastError结果：" << (char*)lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);

	logfile.close();

	//这里弹出一个错误对话框并退出程序  
	FatalAppExitW(NULL, L"*** 程序出现了异常，请重新启动程序并告知开发者! ***");
	return EXCEPTION_EXECUTE_HANDLER;
}

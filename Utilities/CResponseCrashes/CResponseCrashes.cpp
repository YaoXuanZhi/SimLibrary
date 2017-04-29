#include "CResponseCrashes.h"
#include <vector>
#include <fstream>
#include <time.h>
#pragma warning(disable:4996)

/**
* @brief: ��ȫ�����ַ�������
* @param[out] char*  szDest  Ŀ���ַ���
* @param[in] size_t  nMaxDestSize  Ŀ���ַ�������󳤶�
* @param[in] const char*  szSrc  Դ�ַ���
* @return ��
* @note
*
*/
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

/**
* @brief: ��ñ�����Ľ�����
* @return TCHAR* ���ر�����Ľ�����
* @note
*
*/
static char* GetProcessName()
{
	static char szFileFullPath[MAX_PATH] = { 0 }, szProcessName[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH);
	size_t length = strlen(szFileFullPath);
	for (size_t i = length - 1; i >= 0; i--)//��·�����濪ʼ��\������������б��  
	{
		if (szFileFullPath[i] == '\\')//�ҵ���һ��\���Ϳ������ϻ�ȡ����������  
		{
			i++;
			for (size_t j = 0; i <= length; j++)//������\0������,��i=length  
			{
				szProcessName[j] = szFileFullPath[i++];
			}
			break;
		}
	}
	return szProcessName;
}

/**
* @brief: ����Dump�ļ�
* @param[in] LPCWSTR  lpstrDumpFilePathName  ��������Dump�ļ�������·��
* @param[in] EXCEPTION_POINTERS*  pException  ָ��������ʱ����Ӧ���쳣��Ϣ
* @return ��
* @note
*
*/
static void CreateDumpFile(const char* lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	//����Dump�ļ�  
	HANDLE hDumpFile = CreateFileA(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//Dump��Ϣ  
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	//д��Dump�ļ�����  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}

/**
* @brief: �õ����������Ϣ
* @param[in] const  EXCEPTION_RECORD*  pRecord  ����������Ϣ�Ľṹ��
* @return CrashInfo ���ת���������
* @note
* ��ʵ���ǽ�EXCEPTION_RECORD����ת��ΪCrashInfo����
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
* @brief: �õ�CallStack��Ϣ
* @param[in] const CONTEXT*  pContext  ���������쳣ʱ����������Ϣ
* @return std::vector<CallStackInfo> ���س������ʱ�������ջ��Ϣ
* @note
* pContext��������EXCEPTION_POINTERS�ṹ����
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

	//��ͬ��CPU���ͣ�������Ϣ�ɲ�ѯMSDN  
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
		//�ú�����ʵ��������ܵ�����Ҫ��һ������  
		//�������÷��Լ������ͷ���ֵ�ľ�����Ϳ��Բ�ѯMSDN  
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

		//�õ�������  
		if (SymGetSymFromAddr64(hProcess, sf.AddrPC.Offset, NULL, pSymbol))
		{
			SafeStrCpy(callstackinfo.MethodName, nMaxLenOfName, pSymbol->Name);
		}

		IMAGEHLP_LINE64 lineInfo;
		memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));

		lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD dwLineDisplacement;

		//�õ��ļ��������ڵĴ�����  
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
		{
			SafeStrCpy(callstackinfo.FileName, nMaxLenOfName, lineInfo.FileName);
			sprintf(callstackinfo.LineNumber, "%d", lineInfo.LineNumber);
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));

		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

		//�õ�ģ����  
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

void CResponseCrashes::StartMonitor()
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

	//��ñ����������
	char szDumpName[MAX_PATH] = { 0 };
	sprintf(szDumpName, "%s(%s).dmp", GetProcessName(), szCurrentDate);
	CreateDumpFile(szDumpName, pException);

	//ȷ�����㹻��ջ�ռ�  
#ifdef _M_IX86  
	if (pException->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
	{
		static char TempStack[1024 * 128];
		__asm mov eax, offset TempStack[1024 * 128];
		__asm mov esp, eax;
	}
#endif    

	//������ǰ�쳣��־
	std::ofstream logfile;
	char szErrorLogName[MAX_PATH] = { 0 };
	sprintf(szErrorLogName, "CallCrash(%s).log", szCurrentDate);
	logfile.open(szErrorLogName);

	CResponseCrashes::CrashInfo crashinfo = GetCrashInfo(pException->ExceptionRecord);

	//���Crash��Ϣ  
	logfile << "ExceptionAddress: " << crashinfo.ErrorCode << std::endl;
	logfile << "ExceptionCode: " << crashinfo.Address << std::endl;
	logfile << "ExceptionFlags: " << crashinfo.Flags << std::endl;

	std::vector<CResponseCrashes::CallStackInfo> arrCallStackInfo = GetCallStack(pException->ContextRecord);

	//���CallStack  
	logfile << "CallStack: " << std::endl;
	for (std::vector<CResponseCrashes::CallStackInfo>::iterator i = arrCallStackInfo.begin(); i != arrCallStackInfo.end(); ++i)
	{
		CResponseCrashes::CallStackInfo callstackinfo = (*i);
		logfile << callstackinfo.MethodName << "() : [" << callstackinfo.ModuleName << "] (File: " << callstackinfo.FileName << " @Line " << callstackinfo.LineNumber << ")" << std::endl;
	}

	//�������Ĵ����쳣��
	//��ʾ������Ϣ
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
	logfile << "GetLastError�����" << (char*)lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);

	logfile.close();

	//���ﵯ��һ������Ի����˳�����  
	FatalAppExitW(NULL, L"*** ����������쳣���������������򲢸�֪������! ***");
	return EXCEPTION_EXECUTE_HANDLER;
}

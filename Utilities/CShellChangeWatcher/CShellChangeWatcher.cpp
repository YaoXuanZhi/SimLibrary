#pragma warning(disable:4996)
#include "CShellChangeWatcher.h"
#include <stdio.h>
SHChangeNotifyDLL::SHChangeNotifyDLL() :m_hShell32(NULL),
m_pfnSHChangeNotifyRegister(NULL),
m_pfnChangeNotifyDeregister(NULL)
{
	m_hShell32 = LoadLibraryA("Shell32.dll");
	if (NULL == m_hShell32)
	{
		return;
	}

	m_pfnSHChangeNotifyRegister = (pfnSHChangeNotifyRegister)GetProcAddress(m_hShell32, "SHChangeNotifyRegister");
	m_pfnChangeNotifyDeregister = (pfnSHChangeNotifyDeregister)GetProcAddress(m_hShell32, "SHChangeNotifyDeregister");
	m_pfnGetSpecialFolderLocation = (pfnSHGetSpecialFolderLocation)GetProcAddress(m_hShell32, "SHGetSpecialFolderLocation");
	m_pfnGetPathFromIDListW = (pfnSHGetPathFromIDListW)GetProcAddress(m_hShell32, "SHGetPathFromIDListW");
	m_pfnGetPathFromIDListA = (pfnSHGetPathFromIDListA)GetProcAddress(m_hShell32, "SHGetPathFromIDListA");
	m_pfnChangeNotification_Lock = (pfnSHChangeNotification_Lock)GetProcAddress(m_hShell32, "SHChangeNotification_Lock");
	m_pfnChangeNotification_Unlock = (pfnSHChangeNotification_Unlock)GetProcAddress(m_hShell32, "SHChangeNotification_Unlock");
	m_pfnSimpleIDListFromPath = (pfnSHSimpleIDListFromPath)GetProcAddress(m_hShell32, "SHSimpleIDListFromPath");

	if (NULL == m_pfnSHChangeNotifyRegister ||
		NULL == m_pfnChangeNotifyDeregister ||
		NULL == m_pfnGetSpecialFolderLocation ||
		NULL == m_pfnGetPathFromIDListW ||
		NULL == m_pfnGetPathFromIDListA ||
		NULL == m_pfnChangeNotification_Lock ||
		NULL == m_pfnChangeNotification_Unlock ||
		NULL == m_pfnSimpleIDListFromPath
		)
	{
		FreeLibrary(m_hShell32);
		m_hShell32 = NULL;
		m_pfnSHChangeNotifyRegister = NULL;
		m_pfnChangeNotifyDeregister = NULL;
		m_pfnGetSpecialFolderLocation = NULL;
		m_pfnGetPathFromIDListW = NULL;
		m_pfnGetPathFromIDListA = NULL;
		m_pfnChangeNotification_Lock = NULL;
		m_pfnChangeNotification_Unlock = NULL;
		m_pfnSimpleIDListFromPath = NULL;
		return;
	}
}

SHChangeNotifyDLL & SHChangeNotifyDLL::GetInstance()
{
	static SHChangeNotifyDLL Obj;
	return Obj;
}

BOOL SHChangeNotifyDLL::SHChangeNotifyDeregister(ULONG ulID)
{
	if (NULL != m_pfnChangeNotifyDeregister)
	{
		return m_pfnChangeNotifyDeregister(ulID);
	}
	return FALSE;
}

ULONG SHChangeNotifyDLL::SHChangeNotifyRegister(HWND hWnd, int fSource, 
	LONG fEvents, UINT wMsg, int cEntries, const SHChangeNotifyEntryPlus * pfsne)
{
	if (NULL != m_pfnSHChangeNotifyRegister)
	{
		return m_pfnSHChangeNotifyRegister(hWnd, fSource, fEvents, wMsg, cEntries, pfsne);
	}
	return NULL;
}

HRESULT SHChangeNotifyDLL::SHGetSpecialFolderLocation(HWND hwnd, int csidl, PIDLIST_ABSOLUTEPLUG *ppidl)
{
	if (NULL != m_pfnGetSpecialFolderLocation)
	{
		return m_pfnGetSpecialFolderLocation(hwnd, csidl, ppidl);
	}
	return S_FALSE;
}

BOOL SHChangeNotifyDLL::SHGetPathFromIDListW(LPCITEMIDLISTPLUS pidl, LPWSTR pszPath)
{
	if (NULL != m_pfnGetPathFromIDListW)
	{
		return m_pfnGetPathFromIDListW(pidl, pszPath);
	}
	return FALSE;
}

BOOL SHChangeNotifyDLL::SHGetPathFromIDListA(LPCITEMIDLISTPLUS pidl, LPSTR pszPath)
{
	if (NULL != m_pfnGetPathFromIDListA)
	{
		return m_pfnGetPathFromIDListA(pidl, pszPath);
	}
	return FALSE;
}

BOOL SHChangeNotifyDLL::SHGetPathFromIDListT(LPCITEMIDLISTPLUS pidl, LPTSTR pszPath)
{
#ifndef _UNICODE
	return SHGetPathFromIDListA(pidl, pszPath);
#else
	return SHGetPathFromIDListW(pidl,pszPath);
#endif
}

HANDLE SHChangeNotifyDLL::SHChangeNotification_Lock(HANDLE hChange, DWORD dwProcId, 
	PIDLIST_ABSOLUTEPLUG **pppidl, LONG *plEvent)
{
	if (NULL != m_pfnChangeNotification_Lock)
	{
		return m_pfnChangeNotification_Lock(hChange, dwProcId, pppidl, plEvent);
	}
	return NULL;
}

BOOL SHChangeNotifyDLL::SHChangeNotification_Unlock(HANDLE hLock)
{
	if (NULL != m_pfnChangeNotification_Unlock)
	{
		return m_pfnChangeNotification_Unlock(hLock);
	}
	return FALSE;
}

PIDLIST_ABSOLUTEPLUG SHChangeNotifyDLL::SHSimpleIDListFromPath(PCWSTR pszPath)
{
	if (NULL != m_pfnSimpleIDListFromPath)
	{
		return m_pfnSimpleIDListFromPath(pszPath);
	}
	return NULL;
}

CShellItemChangeWatcher::CShellItemChangeWatcher() : _ulRegister(0)
{

}

CShellItemChangeWatcher::~CShellItemChangeWatcher()
{
	StopWatching();
}

HRESULT CShellItemChangeWatcher::StartWatching(HWND hwnd, UINT uMsg, long lEvents, BOOL fRecursive)
{
	StopWatching();
	PIDLIST_ABSOLUTEPLUG pidlDesktop;
	HRESULT hres = SHChangeNotifyDLL::GetInstance().SHGetSpecialFolderLocation(hwnd, CSIDL_DESKTOP, &pidlDesktop);
	if (SUCCEEDED(hres)&& pidlDesktop)
	{
		SHChangeNotifyEntryPlus const entries2[] = { pidlDesktop,fRecursive };
		int const nSources = SHCNRF_ShellLevelPLUS | 
			SHCNRF_InterruptLevelPLUS | SHCNRF_NewDeliveryPLUS;

		_ulRegister = SHChangeNotifyDLL::GetInstance().SHChangeNotifyRegister(hwnd, 
			nSources, lEvents, uMsg, ARRAYSIZE(entries2), entries2);
		hres = _ulRegister != 0 ? S_OK : E_FAIL;

		CoTaskMemFree((void*)pidlDesktop);
	}
	return hres;
}

void CShellItemChangeWatcher::StopWatching()
{
	if (_ulRegister)
	{
		SHChangeNotifyDLL::GetInstance().SHChangeNotifyDeregister(_ulRegister);
		_ulRegister = 0;
	}
}

TCHAR* CShellItemChangeWatcher::PrintEventName(long lEvent)
{
	TCHAR* psz = NULL;

#ifndef MAP_ENTRY	
#ifdef _UNICODE
#define MAP_ENTRY(x) {L#x, x}
#else
#define MAP_ENTRY(x) {#x, x}
#endif
#endif
	static const struct { TCHAR* pszName; long lEvent; } c_rgEventNames[] =
	{
		MAP_ENTRY(SHCNE_RENAMEITEMPLUS),
		MAP_ENTRY(SHCNE_CREATEPLUS),
		MAP_ENTRY(SHCNE_DELETEPLUS),
		MAP_ENTRY(SHCNE_MKDIRPLUS),
		MAP_ENTRY(SHCNE_RMDIRPLUS),
		MAP_ENTRY(SHCNE_MEDIAINSERTEDPLUS),
		MAP_ENTRY(SHCNE_MEDIAREMOVEDPLUS),
		MAP_ENTRY(SHCNE_DRIVEREMOVEDPLUS),
		MAP_ENTRY(SHCNE_DRIVEADDPLUS),
		MAP_ENTRY(SHCNE_NETSHAREPLUS),
		MAP_ENTRY(SHCNE_NETUNSHAREPLUS),
		MAP_ENTRY(SHCNE_ATTRIBUTESPLUS),
		MAP_ENTRY(SHCNE_UPDATEDIRPLUS),
		MAP_ENTRY(SHCNE_UPDATEITEMPLUS),
		MAP_ENTRY(SHCNE_SERVERDISCONNECTPLUS),
		MAP_ENTRY(SHCNE_DRIVEADDGUIPLUS),
		MAP_ENTRY(SHCNE_RENAMEFOLDERPLUS),
		MAP_ENTRY(SHCNE_FREESPACEPLUS),
	};
#undef MAP_ENTRY
	for (int i = 0; i < ARRAYSIZE(c_rgEventNames); i++)
	{
		if (c_rgEventNames[i].lEvent == lEvent)
		{
			psz = c_rgEventNames[i].pszName;
			break;
		}
	}
	return psz;
}

void CShellItemChangeWatcher::PrintPidlInfo(PIDLIST_ABSOLUTEPLUG pidl)
{
	if (pidl)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		SHChangeNotifyDLL::GetInstance().SHGetPathFromIDListT(pidl, szPath);
		OutputDebugString(szPath);
		OutputDebugStringA("\n");
	}
}

void CShellItemChangeWatcher::OnChangeMessage(WPARAM wParam, LPARAM lParam)
{
	long lEvent;
	PIDLIST_ABSOLUTEPLUG *rgpidl;
	HANDLE hNotifyLock = SHChangeNotifyDLL::GetInstance().SHChangeNotification_Lock((HANDLE)wParam, 
		(DWORD)lParam, &rgpidl, &lEvent);
	if (hNotifyLock)
	{
		if (IsItemNotificationEvent(lEvent))
		{
			if (lEvent == SHCNE_RENAMEITEM || lEvent == SHCNE_RENAMEFOLDER)
			{
				if (rgpidl[0] && rgpidl[1])
				{
					TCHAR szTemp[1024] = { 0 };
					TCHAR szPath1[MAX_PATH] = { 0 };
					SHChangeNotifyDLL::GetInstance().SHGetPathFromIDListT(rgpidl[0], szPath1);
					TCHAR szPath2[MAX_PATH] = { 0 };
					SHChangeNotifyDLL::GetInstance().SHGetPathFromIDListT(rgpidl[1], szPath2);
					_stprintf(szTemp, _T("%s  %s ==> %s\n"), PrintEventName(lEvent), szPath1, szPath2);
					OutputDebugString(szTemp);
				}
			}
			else {
				PrintPidlInfo(rgpidl[0]);
				PrintPidlInfo(rgpidl[1]);
				OutputDebugString(PrintEventName(lEvent));
				OutputDebugStringA("\n");
			}
		}
		else
		{
			// dispatch non-item events here in the future
		}
		SHChangeNotifyDLL::GetInstance().SHChangeNotification_Unlock(hNotifyLock);
	}
}

bool CShellItemChangeWatcher::IsItemNotificationEvent(long lEvent)
{
	return !(lEvent & (SHCNE_UPDATEIMAGE | 
		SHCNE_ASSOCCHANGED |
		SHCNE_EXTENDED_EVENT | 
		SHCNE_FREESPACE | 
		SHCNE_DRIVEADDGUI | 
		SHCNE_SERVERDISCONNECT));
}

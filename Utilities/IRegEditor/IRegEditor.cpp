#include "IRegEditor.h"
#include <assert.h>
#include <algorithm>
#pragma warning(disable:4996)

RegKeyCtx::RegKeyCtx()
{
	m_hKey = NULL;
	m_szKeyPathWithoutRoot.clear();
	m_szKeyName.clear();
}

RegKeyCtx::RegKeyCtx(const RegKeyCtx & RegKeyObj)
{
	m_hKey = RegKeyObj.GetMainKeyEx();
	m_szKeyPathWithoutRoot = RegKeyObj.GetKeyPath();
	m_szKeyName = RegKeyObj.GetKeyName();
}

RegKeyCtx::RegKeyCtx(HKEY hNewKey, tstring szNewKeyPath, tstring szNewKeyName)
{
	m_hKey = hNewKey;
	m_szKeyPathWithoutRoot = szNewKeyPath;
	m_szKeyName = szNewKeyName;
}

RegKeyCtx::RegKeyCtx(tstring szKeyPathWithRoot, tstring szKeyName)
{
	HKEY hKey = NULL;
	TCHAR szKeyPath[MAX_PATH] = { 0 };
	IRegEditor::AnalyzeKeyPathWithRoot(&szKeyPathWithRoot[0], hKey, szKeyPath);
	m_hKey = hKey;
	m_szKeyPathWithoutRoot = szKeyPath;
	m_szKeyName = szKeyName;
}

tstring RegKeyCtx::CtxObj2String(RegKeyCtx KeyCtxObj)
{
	tstring szKeyCtx = tstring(_T("KeyPathWithRoot:")) + KeyCtxObj.GetKeyPathWithRoot()
		+ tstring(_T(" KeyName:"))+KeyCtxObj.GetKeyNameEx();
	ToUpperForString(szKeyCtx);
	return szKeyCtx;
}

void RegKeyCtx::ReplaceForString(tstring & srcStr, tstring oldStr, tstring newStr, int nMatchCount)
{
	bool bLimitMatched = nMatchCount > 0 ? true : false;
	for (tstring::size_type pos(0); pos != tstring::npos; pos += newStr.length())
	{
		if ((pos = srcStr.find(oldStr, pos)) != tstring::npos)
		{
			srcStr.replace(pos, oldStr.length(), newStr);
			if (bLimitMatched)
			{
				if (nMatchCount > 0)
				{
					if (--nMatchCount < 1)
						return;
				}
			}
		}
		else {
			break;
		}
	}
}

bool RegKeyCtx::operator<(const RegKeyCtx & dstObj) const
{
	tstring szSrcTemp = CtxObj2String(*this);
	tstring szDstTemp = CtxObj2String(dstObj);

	//ע�⣬ע�����HKEY_LOCAL_MACHINE��systemĿ¼�£�����������ע����Ӽ�
	//ControlSet001��CurrentControlSet���������Ӽ�·��������ʵ������һ���ģ�
	//Ҳ����˵������һ���޸ģ�����һ��Ҳ����֮�޸ģ�������������ü�ֵ����ʱ��
	//�������ߵ�������Ϊһ�����ɡ�
	if (szSrcTemp.find(_T("CONTROLSET001") && szDstTemp.find(_T("CURRENTCONTROLSET"))))
	{
		ReplaceForString(szSrcTemp,_T("CONTROLSET001"),_T("CURRENTCONTROLSET"));
	}else if (szSrcTemp.find(_T("CONTROLSET001") && szDstTemp.find(_T("CURRENTCONTROLSET"))))
	{
		ReplaceForString(szDstTemp, _T("CONTROLSET001"), _T("CURRENTCONTROLSET"));
	}
	return szSrcTemp < szDstTemp;
}

bool RegKeyCtx::operator==(const RegKeyCtx & dstObj) const
{
	tstring szSrcTemp = CtxObj2String(*this);
	tstring szDstTemp = CtxObj2String(dstObj);
	return szSrcTemp == szDstTemp;
}

bool RegKeyCtx::SetMainKeyEx(HKEY hMainKey)
{
	if (IsVaildForMainKey(hMainKey))
	{
		m_hKey = hMainKey;
		return true;
	}
	return false;
}

HKEY RegKeyCtx::GetMainKeyEx() const
{
	if (IsVaildForMainKey(m_hKey))
	{
		return m_hKey;
	}
	else {
		return NULL;
	}
}

void RegKeyCtx::SetKeyPathEx(tstring szKeyPath)
{
	m_szKeyPathWithoutRoot = szKeyPath;
}

tstring RegKeyCtx::GetKeyPathEx() const
{
	return m_szKeyPathWithoutRoot;
}

const TCHAR * RegKeyCtx::GetKeyPath() const
{
	return &m_szKeyPathWithoutRoot[0];
}

tstring RegKeyCtx::GetKeyPathWithRoot() const
{
	HKEY hMainKey = GetMainKeyEx();
	tstring szHKey;
	if (hMainKey == HKEY_CLASSES_ROOT)		  { szHKey = _T("HKEY_CLASSES_ROOT"); }
	else if (hMainKey == HKEY_CURRENT_USER)	  { szHKey = _T("HKEY_CLASSES_ROOT"); }
	else if (hMainKey == HKEY_LOCAL_MACHINE)  { szHKey = _T("HKEY_LOCAL_MACHINE"); }
	else if (hMainKey == HKEY_USERS)		  { szHKey = _T("HKEY_USERS"); }
	else if (hMainKey == HKEY_CURRENT_CONFIG) { szHKey = _T("HKEY_CURRENT_CONFIG"); }
	tstring szKeyPathWithRoot = szHKey + tstring(_T("\\")) + GetKeyPathEx();
	return szKeyPathWithRoot;
}

void RegKeyCtx::SetKeyNameEx(tstring szKeyName)
{
	//������Ϊ�յ�ʱ�򣬻ᱻRegEditor.exeʶ��Ϊ��@��
	m_szKeyName = szKeyName;
}

tstring RegKeyCtx::GetKeyNameEx() const
{
	return m_szKeyName;
}

const TCHAR * RegKeyCtx::GetKeyName() const
{
	return &m_szKeyName[0];
}

bool RegKeyCtx::IsVaildCondition()
{
	if (!IsVaildForMainKey(m_hKey))
		return false;
	return true;
}

bool RegKeyCtx::IsVaildForMainKey(HKEY hKey) const
{
	if (NULL == hKey)
		return false;
	if (hKey != HKEY_CLASSES_ROOT
		&&hKey != HKEY_CURRENT_USER
		&&hKey != HKEY_LOCAL_MACHINE
		&&hKey != HKEY_USERS
		&&hKey != HKEY_CURRENT_CONFIG
		)
		return false;
	return true;
}

void RegKeyCtx::ToUpperForString(tstring & szSrc)
{
	std::transform(szSrc.begin(), szSrc.end(), szSrc.begin(), ::toupper);
}

RegValueCtx::RegValueCtx()
{
	m_dwVarType = REG_NONE;
	InitBufferDatas();
}

RegValueCtx::RegValueCtx(const TCHAR * szBuffer, unsigned long dwValueType)
{
	InitBufferDatas();
	SetValueType(dwValueType);
	SetSTRING(szBuffer);
}

RegValueCtx::RegValueCtx(unsigned long long dw64Value)
{
	InitBufferDatas();
	SetValueType(REG_QWORD);
	SetQWORD(dw64Value);
}

RegValueCtx::RegValueCtx(unsigned long dwValue) 
{
	InitBufferDatas();
	SetValueType(REG_DWORD);
	SetDWORD(dwValue);
}

RegValueCtx::RegValueCtx(void * pBuffer, unsigned long dwcbSize) 
{
	InitBufferDatas();
	SetValueType(REG_BINARY);
	SetBINARY(pBuffer,dwcbSize);
}

RegValueCtx::~RegValueCtx()
{
	ReleaseMemory();
}

RegValueCtx::RegValueCtx(const RegValueCtx &KeyValueObj)
{
	InitBufferDatas();
	m_dwVarType = KeyValueObj.GetValueType();
	ResetMemorySize(KeyValueObj.GetBufferSize());
	memcpy(GetBuffer(), KeyValueObj.GetBuffer(), m_dwcbSize);
}

bool RegValueCtx::operator==(const RegValueCtx & dstObj) const
{
	DWORD dwType = GetValueType();
	if(dwType==dstObj.GetValueType())
	{ 
		DWORD dwcbSize = GetBufferSize();
		if (dwcbSize == dstObj.GetBufferSize())
		{
			if (0 == memcmp(GetBuffer(), dstObj.GetBuffer(), dwcbSize))
			{
				return true;
			}
		}		
	} 
	return false;
}

bool RegValueCtx::operator!=(const RegValueCtx & dstObj) const
{
	return !(dstObj==*this);
}

bool RegValueCtx::operator<(const RegValueCtx & dstObj) const
{
	DWORD dwcbSize = GetBufferSize() > dstObj.GetBufferSize() ? 
		dstObj.GetBufferSize() : GetBufferSize();
	int nResult = memcmp(GetBuffer(), dstObj.GetBuffer(), dwcbSize);
	return nResult <= 0 ? true : false;
}

RegValueCtx& RegValueCtx::operator=(const RegValueCtx & dstObj)
{
	SetValueType(dstObj.GetValueType());
	ResetMemorySize(dstObj.GetBufferSize());
	memcpy(GetBuffer(), dstObj.GetBuffer(), m_dwcbSize);
	return *this;
}

void RegValueCtx::SetValueType(unsigned long dwNewType)
{
	m_dwVarType = dwNewType;
}

unsigned long RegValueCtx::GetValueType() const
{
	return m_dwVarType;
}

unsigned long RegValueCtx::GetBufferSize() const
{
	return m_dwcbSize;
}

void RegValueCtx::InitBufferDatas()
{
	m_pBuffer = NULL;
	m_dwcbSize = 0;
}

void RegValueCtx::ReleaseMemory()
{
	if (NULL != m_pBuffer && m_dwcbSize > 0 && REG_NONE != m_dwVarType)
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
	}
	InitBufferDatas();
}

bool RegValueCtx::IsVaildMemory() const
{
	if (m_dwcbSize == 0 || NULL == m_pBuffer || REG_NONE == m_dwVarType)
		return false;
	return true;
}

void RegValueCtx::ResetMemorySize(unsigned long dwcbSize)
{
	if (dwcbSize > 0)
	{
		ReleaseMemory();
		m_dwcbSize = dwcbSize;
		m_pBuffer = malloc(m_dwcbSize);
		memset(m_pBuffer, 0, m_dwcbSize);
	}
}

void * RegValueCtx::GetBuffer() const
{
	if (!IsVaildMemory())
	{
		return NULL;
	}
	else {
		return m_pBuffer;
	}
}

void RegValueCtx::ShowMemoryContent()
{
#ifndef PRINTFBUFFER
#define PRINTFBUFFER(szValueType)		   \
	TCHAR *szString = (TCHAR*)m_pBuffer;   \
	OutputDebugString((szValueType));	   \
	OutputDebugString(_T("��"));		   \
	OutputDebugString(szString);

#endif

	if (IsVaildMemory())
	{
		LSTATUS lStatus = ERROR_SUCCESS;
		switch (m_dwVarType)
		{
		case REG_SZ:
		{
			PRINTFBUFFER(_T("REG_SZ"));
		}; break;
		case REG_EXPAND_SZ:
		{
			PRINTFBUFFER(_T("REG_EXPAND_SZ"));
		}; break;
		case REG_MULTI_SZ:
		{
			PRINTFBUFFER(_T("REG_MULTI_SZ"));
		}; break;
		case REG_DWORD:
		{
			OutputDebugString(_T("REG_DWORD��"));
			DWORD dwDword = *(DWORD*)m_pBuffer;
			TCHAR szTemp[MAX_PATH] = { 0 };
			_stprintf(szTemp, _T("%lu"), dwDword);
			OutputDebugString(szTemp);
		}; break;
		case REG_QWORD:
		{
			OutputDebugString(_T("REG_QWORD��"));
			DWORD64 dwQDword = *(DWORD64*)m_pBuffer;
			TCHAR szTemp[MAX_PATH] = { 0 };
			_stprintf(szTemp, _T("%llu"), dwQDword);
			OutputDebugString(szTemp);
		}; break;
		case REG_BINARY:
		{
			OutputDebugString(_T("REG_BINARY��"));
			char *pBlock = (char*)m_pBuffer;
			DWORD dwTemp = m_dwcbSize;
			int nSwitchFlag = 0;
			for (DWORD dw = 0; dw < dwTemp; dw++)
			{
				if (nSwitchFlag == 8)
				{
					nSwitchFlag = 0;
					OutputDebugString(_T("\n"));
				}
				TCHAR szChar[MAX_PATH] = { 0 };
				char ch = (char)pBlock[dw];
				_stprintf(szChar, ch == '\0' ? _T("0X0%-10X") : _T("0X%-10X"), (unsigned char)ch);
				OutputDebugString(szChar);
				nSwitchFlag++;
			}
		}; break;
		default:
			break;
		}
		OutputDebugString(_T("\n"));
	}
	else {
		OutputDebugString(_T("�ڴ�������Ч\n"));
	}
#ifdef PRINTFBUFFER
#undef PRINTFBUFFER
#endif
}

bool RegValueCtx::GetDWORD(unsigned long & dwValue)
{
	if (IsVaildMemory())
	{
		if (REG_DWORD == m_dwVarType)
		{
			dwValue = *(DWORD*)m_pBuffer;
			return true;
		}
	}
	return false;
}

int nError = 0;
void RegValueCtx::SetDWORD(unsigned long dwNewValue)
{
	nError++;
	printf("��%d��\n",nError);
	ResetMemorySize(sizeof(DWORD));
	memcpy(m_pBuffer, &dwNewValue, m_dwcbSize);
}

bool RegValueCtx::GetQWORD(unsigned long long & qwValue)
{
	if (IsVaildMemory())
	{
		if (REG_QWORD == m_dwVarType)
		{
			qwValue = *(DWORD64*)m_pBuffer;
			return true;
		}
	}
	return false;
}

void RegValueCtx::SetQWORD(unsigned long long qwNewValue)
{
	ResetMemorySize(sizeof(DWORD64));
	memcpy(m_pBuffer, &qwNewValue, m_dwcbSize);
}

bool RegValueCtx::GetSTRING(tstring & szBuffer)
{
	if (IsVaildMemory())
	{
		switch (m_dwVarType)
		{
		case REG_SZ:
		case REG_EXPAND_SZ:
		case REG_MULTI_SZ:
		{
			szBuffer.clear();
			szBuffer.resize(m_dwcbSize);
			memcpy(&szBuffer[0], m_pBuffer, m_dwcbSize);
			return true;
		}; break;
		}
	}
	return false;
}

void RegValueCtx::SetSTRING(tstring szNewValue)
{
	//��Ҫע����ǣ������ֵ����ΪREG_SZ�ַ������͵�ʱ��
	//���ջ�õ�m_dwcbSize����ֵ���������ֹ��----'\0'
	ResetMemorySize((unsigned long)(szNewValue.length()+1)*sizeof(TCHAR));
	memcpy(m_pBuffer, &szNewValue[0], m_dwcbSize);
}

bool RegValueCtx::GetBINARY(void * lpBuffer, unsigned long & dwcbSize, bool IsAllocMemory)
{
	if (IsVaildMemory())
	{
		if (REG_BINARY == m_dwVarType)
		{
			if (IsAllocMemory)
			{
				lpBuffer = malloc(m_dwcbSize);
				memset(lpBuffer, 0, m_dwcbSize);
				memcpy(lpBuffer, m_pBuffer, m_dwcbSize);
			}
			else {
				lpBuffer = m_pBuffer;
			}
			dwcbSize = m_dwcbSize;
			return true;
		}
	}
	return false;
}

void RegValueCtx::SetBINARY(void * lpBuffer, unsigned long dwcbSize)
{
	ResetMemorySize(dwcbSize);
	memcpy(m_pBuffer, lpBuffer, m_dwcbSize);
}

tstring RegValueCtx::GetHexStrForBuffer(tstring szSpliteFlag,bool bIsXOREncode)
{
	tstring szHexStr;
	char *pBlock = (char*)GetBuffer();
	unsigned long dwcbSize = GetBufferSize();
	for (unsigned long n = 0; n < dwcbSize; n++)
	{
		TCHAR szTemp[3] = { 0 };
		char ch = pBlock[n];
		_stprintf(szTemp, ch == _T('\0') ? _T("0%X") : _T("%X"), (unsigned char)ch);
		szHexStr += szSpliteFlag + tstring(szTemp);
	}

	if (bIsXOREncode)
	{
		size_t Keys[] = { 1,5,9,8,7,4,2,3,87,41,36,98,17,26,47,124 };
		return XOREncodeEx(szHexStr, szSpliteFlag, Keys,sizeof(Keys)/sizeof(size_t));
	}
	return szHexStr;
}

std::vector<tstring> RegValueCtx::GetArrBySpliteStringEx(tstring szSource, 
	tstring szFirstFlag, tstring szFinalFlag)
{
	size_t ROffset = 0;
	std::vector<tstring> SpliteResult;
	while (szSource.find(szFirstFlag, ROffset) >= 0 
		&& szSource.find(szFirstFlag, ROffset) <= szSource.length())
	{
		//�õ���ʼ���De����index
		size_t findex = szSource.find(szFirstFlag, ROffset);
		//�õ�ĩβ��ǵ�����index
		size_t sindex = szSource.find(szFinalFlag, findex + szFirstFlag.length());
		if (szSource.find(szFinalFlag, findex + szFirstFlag.length()) >= 0 
			&& szSource.find(szFinalFlag, findex + szFirstFlag.length()) <= szSource.length())
		{
			SpliteResult.push_back(szSource.substr(findex + szFirstFlag.length(),
				sindex - findex - szFirstFlag.length()));
		}
		ROffset = sindex;
	}
	return SpliteResult;
}

/** @brief: У�鵱ǰ�����Ƿ����ʮ��������ʽ
* @param[in] tstring&  szHex  ����֤��ʮ����������
* @return  ����Ϊtrue����˵�����ݺϷ��������쳣
* @mark  
*/
bool CheckHexData(const TCHAR *szHex)
{
	bool bIsVaild = true;
	size_t nLen = _tcslen(szHex);
	for (size_t n = 0; n < nLen; n++)
	{
		TCHAR ch = szHex[n];
		if (!(_istdigit(ch) || (_T('a') <= ch&&_T('f') >= ch) || (_T('A') <= ch&&_T('F') >= ch)))
		{
			bIsVaild = false;
			break;
		}
	}
	return bIsVaild;
}

//��ʮ�������ַ���ת����ʮ��������
size_t RegValueCtx::HexToInt(tstring szHex)
{
	assert((_T("У�鵱ǰ������ֵ�����Ϸ�"), CheckHexData(&szHex[0])));
	int nInt = 0;
	_stscanf(&szHex[0], _T("%x"), &nInt);
	return nInt;
}

tstring RegValueCtx::XOREncodeEx(tstring szHexStr, tstring szSpliteFlag, size_t *XORKeys, size_t nKeyCount)
{
	tstring XORData;
	szHexStr += szSpliteFlag;
	std::vector<tstring> vcHexStrs = GetArrBySpliteStringEx(szHexStr,
		szSpliteFlag, szSpliteFlag);
	if (vcHexStrs.size() < 1)
		return XORData;
	unsigned long dwcbSize = (unsigned long)vcHexStrs.size();
	for (size_t n = 0; n < dwcbSize; n++)
	{
		TCHAR szTemp[MAX_PATH] = { 0 };
		size_t XORC = HexToInt(vcHexStrs[n])^ XORKeys[(n) % nKeyCount];
		_stprintf(szTemp, XORC == _T('\0') ? _T("0%X") : _T("%X"), (unsigned char)XORC);
		XORData += szSpliteFlag + tstring(szTemp);
	}
	return XORData;
}

void RegValueCtx::SetBufferByHexStr(tstring szHexStr, tstring szSpliteFlag, bool bIsXOREncode)
{
	if (szHexStr.length() < 1)
		return;
	if (szSpliteFlag.length() < 1)
		return;

	//������
	size_t Keys[] = { 1,5,9,8,7,4,2,3,87,41,36,98,17,26,47,124 };
	tstring szSource = (bIsXOREncode ? XOREncodeEx(szHexStr, szSpliteFlag, Keys, sizeof(Keys) / sizeof(size_t)) : szHexStr)
		+ szSpliteFlag;
	std::vector<tstring> vcHexStrs = GetArrBySpliteStringEx(szSource,
		szSpliteFlag, szSpliteFlag);
	unsigned long dwcbSize = (unsigned long)vcHexStrs.size();
	//assert(("��ǰʮ���������ݸ�ʽ��ȡ�쳣", dwcbSize > 0));
	if (dwcbSize < 1)
		return;

	//��Ϊ����������Ҫʹ��sprintf()����ʼ���ڴ����ݣ�
	//���������ԣ�����������յ��ַ������������һ��'\0'
	//������Ҫ�ڶ�̬�����ڴ��ʱ���1��Ҳ����dwcbSize+1��
	//�����ս����ڴ����ݿ�����ע����ֵ��ʱ�򣬽�����ֱ������
	//'\0'
	char *pBlock = (char*)malloc(dwcbSize + 1);
	memset(pBlock, 0, dwcbSize + 1);
	char *pTemp = pBlock;
	for (unsigned long n = 0; n < dwcbSize; n++)
	{
		sprintf(pTemp, "%c", (unsigned char)HexToInt(vcHexStrs[n]));
		pTemp++;
	}
	SetBINARY(pBlock,dwcbSize);
	free(pBlock);
	pBlock = NULL;
}

bool IRegEditor::IsOSX64()
{
#if	_WIN64
	return true;
#else
	BOOL bIsWow64 = FALSE;
	IsWow64Process(GetCurrentProcess(), &bIsWow64);
	return bIsWow64 ? true : false;
#endif
}

bool IRegEditor::AnalyzeKeyPathWithRoot(const TCHAR * szKeyPathWithRoot, HKEY & hKey,
	TCHAR * szKeyPath)
{
	if (NULL == szKeyPathWithRoot)
		return false;
	size_t nChar = _tcslen(szKeyPathWithRoot);
	if (nChar < 1)
		return false;
	if (NULL == szKeyPath)
		return false;

	TCHAR szKeyRoot[MAX_PATH] = { 0 };

	TCHAR *pSplitePos = NULL;
	pSplitePos = _tcsstr((TCHAR*)szKeyPathWithRoot, _T("\\"));
	if (pSplitePos)
	{
		size_t nStart = pSplitePos - szKeyPathWithRoot;
		_tcsncpy(szKeyRoot, szKeyPathWithRoot, nStart);
		_tcsncpy(szKeyPath, ++pSplitePos, nChar - nStart);
	}
	else {
		return false;
	}

	if (0 == _tcscmp(szKeyRoot, _T("HKEY_CLASSES_ROOT"))) 
	{ hKey = HKEY_CLASSES_ROOT; }
	else if (0 == _tcscmp(szKeyRoot, _T("HKEY_CURRENT_USER"))) 
	{ hKey = HKEY_CURRENT_USER; }
	else if (0 == _tcscmp(szKeyRoot, _T("HKEY_LOCAL_MACHINE"))) 
	{ hKey = HKEY_LOCAL_MACHINE; }
	else if (0 == _tcscmp(szKeyRoot, _T("HKEY_USERS"))) 
	{ hKey = HKEY_USERS; }
	else if (0 == _tcscmp(szKeyRoot, _T("HKEY_PERFORMANCE_DATA")))
	{ hKey = HKEY_PERFORMANCE_DATA; }
	else if (0 == _tcscmp(szKeyRoot, _T("HKEY_PERFORMANCE_TEXT"))) 
	{ hKey = HKEY_PERFORMANCE_TEXT; }
	else if (0 == _tcscmp(szKeyRoot, _T("HKEY_PERFORMANCE_NLSTEXT")))
	{ hKey = HKEY_PERFORMANCE_NLSTEXT; }
	return true;
}

LSTATUS IRegEditor::RegOpenKeyUtil(HKEY hKey, const TCHAR * szKeyPath, HKEY & hSubKey)
{
	LSTATUS lStatus = ERROR_SUCCESS;
	if (IsOSX64())
	{
		//��ǰ���е�ϵͳ��64bit�ģ�
#ifndef _WIN64
		//��������32bit��
		lStatus = RegOpenKeyEx(hKey, szKeyPath, 0, KEY_CREATE_LINK | KEY_WRITE | 
			KEY_READ | KEY_NOTIFY | KEY_WOW64_64KEY, &hSubKey);
#else
		lStatus = RegOpenKeyEx(hKey, szKeyPath, 0, KEY_CREATE_LINK | KEY_WRITE |
			KEY_READ | KEY_NOTIFY, &hSubKey);
#endif
	}
	else
	{
		lStatus = RegOpenKeyEx(hKey, szKeyPath, 0, KEY_CREATE_LINK | KEY_WRITE |
			KEY_READ | KEY_NOTIFY, &hSubKey);
	}
	return lStatus;
}

LSTATUS IRegEditor::RegDeleteKeyUtil(HKEY hKeyRoot, TCHAR * szSubKey)
{
	LSTATUS lStatus = ERROR_SUCCESS;
	if (IsOSX64())
	{
#ifndef _WIN64
		lStatus = RegDeleteKeyEx(hKeyRoot, szSubKey, KEY_READ | KEY_WOW64_64KEY, NULL);
#else
		lStatus = RegDeleteKey(hKeyRoot, szSubKey);
#endif
	}
	else {
		lStatus = RegDeleteKey(hKeyRoot, szSubKey);
	}
	return lStatus;
}

bool IRegEditor::RegDelNodeRecurse(HKEY hKeyRoot, TCHAR * szKeyPath)
{
	TCHAR* lpEnd = NULL;
	LSTATUS lStatus = ERROR_SUCCESS;
	DWORD dwSize = 0;
	TCHAR szName[MAX_PATH] = { 0 };
	HKEY hSubKey = NULL;
	FILETIME ftWrite;

	//�����״ο��Գɹ�ɾ�������Ӽ��Ļ�����ô��˵���˼�û���Ӽ�������ֱ���˳�
	lStatus = RegDeleteKeyUtil(hKeyRoot, szKeyPath);

	if (lStatus == ERROR_SUCCESS)
		return true;

	//���RegDeleteKey��RegDeleteKeyExִ��ʧ�ܣ���ô��˵���˼���
	//�����Ӽ����ڣ���ʱ�򿪴˼���ע����������Ӽ�
	lStatus = RegOpenKeyUtil(hKeyRoot, szKeyPath, hSubKey);

	if (lStatus != ERROR_SUCCESS)
	{
		if (lStatus == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
			return true;
		}
		else {
			printf("Error opening key.\n");
			return false;
		}
	}
	// Check for an ending slash and add one if it is missing.
	lpEnd = szKeyPath + _tcslen(szKeyPath);
	if (*(lpEnd - 1) != _T('\\'))
	{
		*lpEnd = _T('\\');
		lpEnd++;
		*lpEnd = _T('\0');
	}
	//ö�������Ӽ����������µĵݹ�
	dwSize = MAX_PATH;
	lStatus = RegEnumKeyEx(hSubKey, 0, szName, &dwSize, NULL,
		NULL, NULL, &ftWrite);
	if (lStatus == ERROR_SUCCESS)
	{
		do {
			_tcscpy(lpEnd, szName);
			if (!RegDelNodeRecurse(hKeyRoot, szKeyPath)) {
				break;
			}
			dwSize = MAX_PATH;
			lStatus = RegEnumKeyEx(hSubKey, 0, szName, &dwSize, NULL,
				NULL, NULL, &ftWrite);
		} while (lStatus == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = _T('\0');

	RegCloseKey(hSubKey);

	// ���˼��µ������Ӽ���ɾ��֮���ٴγ���ɾ���˼�
	lStatus = RegDeleteKeyUtil(hKeyRoot, szKeyPath);

	if (lStatus == ERROR_SUCCESS)
		return true;
	return false;
}

bool IRegEditor::GetRegValueEx(HKEY hKey,const TCHAR * szKeyPath,
	const TCHAR * szKeyName, RegValueCtx & KeyValue)
{
	bool bResult = false;

	if (NULL == hKey)
		return bResult;
	if (NULL == szKeyPath || _tcslen(szKeyPath) < 1)
		return bResult;
	//��ΪĬ�ϼ�ֵ��ʱ����������RegEditor����ʾΪ@��ʵ����������Ϊ�յģ�
	//�����szKeyName����ֵ��֤����
	if (NULL == szKeyName)
		return bResult;

	HKEY hSubKey = NULL;
	LSTATUS lStatus = RegOpenKeyUtil(hKey, szKeyPath, hSubKey);
	if (ERROR_SUCCESS == lStatus)
	{
		DWORD dwValueType = REG_NONE;
		DWORD cbData = 0;
		lStatus = RegQueryValueEx(hSubKey, szKeyName, NULL, &dwValueType, NULL, &cbData);
		if (lStatus == ERROR_SUCCESS)
		{
			//���赱ǰ��ֵ�������ڵ�ʱ�򣬲���KeyValue��ֵ
			if (REG_NONE != dwValueType)
			{
				DWORD cbNeeded = cbData;
				KeyValue.SetValueType(dwValueType);
				KeyValue.ResetMemorySize(cbData);
				lStatus = RegQueryValueEx(hSubKey, szKeyName, NULL, &dwValueType, 
					(BYTE*)KeyValue.GetBuffer(), &cbData);
				if (ERROR_SUCCESS == lStatus)
				{
					if (cbData == cbNeeded)
					{
						bResult = true;
					}
				}
			}
		}
	}
	RegCloseKey(hKey);
	return bResult;
}

bool IRegEditor::GetRegValueEx(const TCHAR * szKeyPathWithRoot, 
	const TCHAR * szKeyName, RegValueCtx & KeyValue)
{
	bool bResult = false;
	HKEY hKey = NULL;
	TCHAR szKeyPath[MAX_PATH] = { 0 };
	AnalyzeKeyPathWithRoot(szKeyPathWithRoot, hKey, szKeyPath);
	bResult = GetRegValueEx(hKey, szKeyPath, szKeyName, KeyValue);
	return bResult;
}

bool IRegEditor::SetRegValueEx(HKEY hKey,const TCHAR * szKeyPath,
	const TCHAR * szKeyName, RegValueCtx KeyValue)
{
	bool bResult = false;
	HKEY hSubKey = NULL;
	LSTATUS lStatus = S_FALSE;
	lStatus = RegOpenKeyUtil(hKey, szKeyPath, hSubKey);
	if (lStatus != ERROR_SUCCESS)
	{
		//���費���ڣ��򴴽�����Ӽ�
		lStatus = RegCreateKey(hKey, szKeyPath, &hSubKey);
	}

	if (KeyValue.IsVaildMemory())
	{
		lStatus = RegSetValueEx(hSubKey,szKeyName,0,KeyValue.GetValueType(),
			(CONST BYTE*)KeyValue.GetBuffer(),KeyValue.GetBufferSize());
		if (ERROR_SUCCESS == lStatus)
		{
			bResult = true;
		}
	}

	RegCloseKey(hKey);
	hSubKey = NULL;
	return bResult;
}

bool IRegEditor::SetRegValueEx(const TCHAR * szKeyPathWithRoot, 
	const TCHAR * szKeyName, RegValueCtx KeyValue)
{
	HKEY hKey = NULL;
	TCHAR szKeyPath[MAX_PATH] = { 0 };
	AnalyzeKeyPathWithRoot(szKeyPathWithRoot, hKey, szKeyPath);
	return SetRegValueEx(hKey, szKeyPath, szKeyName, KeyValue);
}


//ʹ�õݹ�ķ�ʽ��ɾ����ǰ·���µ�������Ŀ¼
bool IRegEditor::DeleteRegSubKeyByPath(const TCHAR * szKeyPathWithRoot)
{
	HKEY hKey = NULL;
	TCHAR szKeyPath[2048] = { 0 };
	AnalyzeKeyPathWithRoot(szKeyPathWithRoot, hKey, szKeyPath);
	return DeleteRegSubKeyByPath(hKey, szKeyPath);
}

bool IRegEditor::DeleteRegSubKeyByPath(HKEY hKeyRoot, const TCHAR* szKeyPath)
{
	//Ҫȷ���Ӽ����ַ����㹻��
	TCHAR szDelKey[MAX_PATH * 2] = { 0 };
	_tcscpy(szDelKey, szKeyPath);
	return RegDelNodeRecurse(hKeyRoot, szDelKey);
}

bool IRegEditor::DeleteRegValueByPath(HKEY hKey,
	const TCHAR * szKeyPath, const TCHAR * szKeyName)
{
	bool bResult = false;
	HKEY  hSubKey = NULL;
	LSTATUS lStatus = RegOpenKeyUtil(hKey, szKeyPath, hSubKey);
	if (lStatus == ERROR_SUCCESS)
	{
		lStatus = RegDeleteValue(hSubKey, szKeyName);
		if (ERROR_SUCCESS == lStatus)
			bResult = true;
	}
	RegCloseKey(hSubKey);
	hSubKey = NULL;
	return bResult;
}

bool IRegEditor::DeleteRegValueByPath(const TCHAR * szKeyPathWithRoot,
	const TCHAR * szKeyName)
{
	HKEY hKey = NULL;
	TCHAR szKeyPath[2048] = { 0 };
	AnalyzeKeyPathWithRoot(szKeyPathWithRoot, hKey, szKeyPath);
	return DeleteRegValueByPath(hKey, szKeyPath, szKeyName);
}

bool IRegEditor::EnumRegSubKeys(HKEY hKey, const TCHAR * szKeyPath,
	OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam)
{
	bool bResult = false;
	DWORD dwIndex = 0;
	HKEY  hSubKey = NULL;
	LSTATUS lStatus = RegOpenKeyUtil(hKey, szKeyPath, hSubKey);
	if (lStatus == ERROR_SUCCESS)
	{
		for (dwIndex = 0; ERROR_SUCCESS == lStatus; dwIndex++)
		{
			DWORD dwcbSize = MAX_PATH;
			TCHAR szSubKeyName[MAX_PATH] = { 0 };
			lStatus = RegEnumKeyEx(hSubKey, dwIndex, szSubKeyName, &dwcbSize,
				NULL, NULL, NULL, NULL);
			if (ERROR_SUCCESS == lStatus)
			{
				TCHAR szSubKeyPath[MAX_PATH] = { 0 };
				_stprintf(szSubKeyPath, _T("%s\\%s"), szKeyPath, szSubKeyName);

				//�ҵ���Ӧ����Ŀ¼������·��
				if (NULL != EnumRegSubKeyProc)
				{
					EnumRegSubKeyProc(hKey, szSubKeyPath, pParam);
				}
			}
		}
		bResult = true;
	}
	RegCloseKey(hKey);
	hSubKey = NULL;
	return bResult;
}

bool IRegEditor::EnumRegSubKeys(const TCHAR * szKeyPathWithRoot,
	OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam)
{
	HKEY hKey = NULL;
	TCHAR szKeyPath[2048] = { 0 };
	AnalyzeKeyPathWithRoot(szKeyPathWithRoot, hKey, szKeyPath);
	return EnumRegSubKeys(hKey, szKeyPath, EnumRegSubKeyProc, pParam);
}

bool IRegEditor::EnumRegValues(HKEY hKey, const TCHAR * szKeyPath, 
	OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam)
{
	bool bResult = false;
	DWORD   dwIndex = 0;
	HKEY    hSubKey = NULL;

	LSTATUS lStatus = RegOpenKeyUtil(hKey, szKeyPath, hSubKey);

	//ע�� KEY_WOW64_64KEY Ȩ��ֵ����ʹ�÷��򽫲��Ҳ���ָ����·����
	if (ERROR_SUCCESS == lStatus)
	{
		for (dwIndex = 0; lStatus == ERROR_SUCCESS; dwIndex++)
		{
			DWORD cchValueName = MAX_PATH;
			DWORD cbData = 0;
			TCHAR  szKeyName[MAX_PATH] = { 0 };
			DWORD   dwValueType = REG_NONE;
			lStatus = RegEnumValue(hSubKey, dwIndex, szKeyName,
				&cchValueName, NULL, &dwValueType, NULL, &cbData);

			if (lStatus == ERROR_SUCCESS)
			{
				DWORD cbNeeded = cbData;
				RegValueCtx KeyValue;
				KeyValue.SetValueType(dwValueType);
				KeyValue.ResetMemorySize(cbData);
				lStatus = RegQueryValueEx(hSubKey, szKeyName, NULL,
					&dwValueType, (BYTE*)KeyValue.GetBuffer(), &cbData);
				if (ERROR_SUCCESS == lStatus)
				{
					if (cbData == cbNeeded)
					{
						//��Ӧ�ص�����
						if (NULL != EnumRegKeyValueProc)
						{
							EnumRegKeyValueProc(szKeyName, KeyValue, pParam);
						}
					}
				}
			}
		}
		bResult = true;
	}
	RegCloseKey(hKey);
	hSubKey = NULL;
	return bResult;
}

bool IRegEditor::EnumRegValues(const TCHAR * szKeyPathWithRoot,
	OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam)
{
	HKEY hKey = NULL;
	TCHAR szKeyPath[2048] = { 0 };
	AnalyzeKeyPathWithRoot(szKeyPathWithRoot, hKey, szKeyPath);
	return EnumRegValues(hKey, szKeyPath, EnumRegKeyValueProc, pParam);
}

bool IRegEditor::BatchGetRegValueEx(HKEY hKeyRoot, const TCHAR * szKeyPath,
	std::map<tstring, RegValueCtx>& mapValues)
{
	if (NULL == szKeyPath || _tcslen(szKeyPath) < 1)
		return false;
	if (mapValues.size() == 0)
		return false;

	for (std::map<tstring, RegValueCtx>::iterator it = mapValues.begin(); 
	it != mapValues.end(); it++)
	{
		GetRegValueEx(hKeyRoot, szKeyPath, &it->first[0], it->second);
	}
	return true;
}

bool IRegEditor::BatchGetRegValueEx(const TCHAR * szKeyPathWithRoot, 
	std::map<tstring, RegValueCtx>& mapValues)
{
	HKEY hKey = NULL;
	TCHAR szKeyPath[2048] = { 0 };
	AnalyzeKeyPathWithRoot(szKeyPathWithRoot, hKey, szKeyPath);
	BatchGetRegValueEx(hKey, szKeyPath, mapValues);
	return true;
}

bool IRegEditor::GetRegValueEx(RegKeyCtx ConditionObj, RegValueCtx & ResultObj)
{
	if (!ConditionObj.IsVaildCondition())
		return false;
	return GetRegValueEx(ConditionObj.GetMainKeyEx(), ConditionObj.GetKeyPath(),
		ConditionObj.GetKeyName(), ResultObj);
}

bool IRegEditor::SetRegValueEx(RegKeyCtx ConditionObj, RegValueCtx ResultObj)
{
	//�ж��Ӽ�·���Ƿ���Ч
	if (!ConditionObj.IsVaildCondition())
		return false;

	//�жϼ�ֵ�Ƿ���Ч
	if (!ResultObj.IsVaildMemory())
	{
		//�������ֵ�ļ�ֵ������Ч�����Ҵ˼�ֵ�Ѿ���ע����д���
		//��ֱ�ӽ��˼�ֵɾ��
		DeleteRegValueByPath(ConditionObj);
		return false;
	}
	return SetRegValueEx(ConditionObj.GetMainKeyEx(), ConditionObj.GetKeyPath(),
		ConditionObj.GetKeyName(), ResultObj);
}

bool IRegEditor::DeleteRegSubKeyByPath(RegKeyCtx ConditionObj)
{
	if (!ConditionObj.IsVaildCondition())
		return false;
	return DeleteRegSubKeyByPath(ConditionObj.GetMainKeyEx(), 
		ConditionObj.GetKeyPath());
}

bool IRegEditor::DeleteRegValueByPath(RegKeyCtx ConditionObj)
{
	if (!ConditionObj.IsVaildCondition())
		return false;
	return DeleteRegValueByPath(ConditionObj.GetMainKeyEx(), 
		ConditionObj.GetKeyPath(), ConditionObj.GetKeyName());
}

bool IRegEditor::EnumRegSubKeys(RegKeyCtx ConditionObj, 
	OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam)
{
	if (!ConditionObj.IsVaildCondition())
		return false;
	return EnumRegSubKeys(ConditionObj.GetMainKeyEx(), 
		ConditionObj.GetKeyPath(), EnumRegSubKeyProc, pParam);
}

bool IRegEditor::EnumRegValues(RegKeyCtx ConditionObj, 
	OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam)
{
	if (!ConditionObj.IsVaildCondition())
		return false;
	return EnumRegValues(ConditionObj.GetMainKeyEx(), 
		ConditionObj.GetKeyPath(), EnumRegKeyValueProc, pParam);
}

bool IRegEditor::BatchGetRegValueEx(RegKeyCtx ConditionObj, 
	std::map<RegKeyCtx, RegValueCtx> &mapValues)
{
	if (!ConditionObj.IsVaildCondition())
		return false;
	for (std::map<RegKeyCtx, RegValueCtx>::iterator it = mapValues.begin();
	it != mapValues.end(); it++)
	{
		GetRegValueEx(ConditionObj.GetMainKeyEx(), ConditionObj.GetKeyPath(),
			ConditionObj.GetKeyName(), it->second);
	}
	return true;
}

bool IRegEditor::BatchSetRegValueEx(std::map<RegKeyCtx, RegValueCtx> mapValues)
{
	bool bResult = true;
	for (std::map<RegKeyCtx, RegValueCtx>::iterator it = mapValues.begin();
	it != mapValues.end(); it++)
	{
		if (!SetRegValueEx(it->first, it->second))
		{
			if(!bResult)
				bResult = false;
		}
	}
	return bResult;
}
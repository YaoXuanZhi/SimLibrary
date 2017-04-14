#ifndef __IREGEDITOR_H
#define __IREGEDITOR_H

#include <map>
#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>

#ifndef tstring
#ifndef _UNICODE
#define  tstring std::string
#else
#define  tstring std::wstring
#endif
#endif

/** @brief: ����ע����ѯ���������ݽṹ
* @mark Ҫ���ѯĳ����ֵ�������Ҫ����֪�����Ӽ�·���ͼ���
*		Ȼ��������������ѯ��Ӧ��ע�����Ϣ����������˼·��
*		����������������ݽṹ����Ų�ѯ��ֵ��������
* @not  ע�⣬��������Ctx��ԭ��Ϊcontext
*/
class RegKeyCtx 
{
public:
	/** @brief: ����Ĭ�Ϲ��캯��
	* @mark
	*/
	RegKeyCtx();

	/** @brief: ֱ��ͨ���������Ӽ�·���Լ���������ʼ��ʵ��
	* @param[in] HKEY  hNewKey  ������Ϣ
	* @param[in] tstring  szNewKeyPath  �Ӽ�·����Ϣ
	* @param[in] tstring  szNewKeyName  ������Ϣ
	* @mark  
	*/
	RegKeyCtx(HKEY hNewKey, tstring szNewKeyPath, tstring szNewKeyName);

	/** @brief: ֱ��ͨ���������Ӽ�·���Լ���������ʼ��ʵ��
	* @param[in] tstring  szKeyPathWithRoot  ������������Ϣ���Ӽ�·��
	* @param[in] tstring  szNewKeyName  ������Ϣ
	* @mark
	*/
	RegKeyCtx(tstring szKeyPathWithRoot, tstring szKeyName);

	/** @brief: ���ظ��ƹ��캯��
	* @mark
	*/
	RegKeyCtx(const RegKeyCtx &RegKeyObj);

private:
	HKEY m_hKey;					  //����ѯ������
	tstring m_szKeyPathWithoutRoot;	  //����ѯ���Ӽ�·��
	tstring m_szKeyName;			  //����ѯ�ļ���

public:
	/** @brief: ����<�����
	* @mark  Ϊ���ô�����֧��STL������㷨�⣬��Ҫ
	*        ��������Ϊ������Ա����
	*/
	bool operator<(const RegKeyCtx &dstObj) const;

	/** @brief: ����==�����
	* @mark  
	*/
	bool operator==(const RegKeyCtx & dstObj) const;

public:
	/** @brief: ���ô���ѯ��ע�������
	* @mark
	*/
	bool SetMainKeyEx(HKEY hMainKey);

	/** @brief: ��ȡ����ѯ��ע�������
	* @mark
	*/
	HKEY GetMainKeyEx() const;

	/** @brief: ���ô���ѯ��ע���·��
	* @mark
	*/
	void SetKeyPathEx(tstring szKeyPath);

	/** @brief: ��ô���ѯ��ע���·��
	* @mark
	*/
	tstring GetKeyPathEx() const;

	/** @brief: ��ô���ѯ��ע���·��
	* @mark
	*/
	const TCHAR* GetKeyPath() const;

	/** @brief: ��ô���ѯ��ע���·��������������Ϣ��
	* @mark
	*/
	tstring GetKeyPathWithRoot() const;

	/** @brief: ���ô���ѯ�ļ���
	* @mark
	*/
	void SetKeyNameEx(tstring szKeyName);

	/** @brief: ��ô���ѯ�ļ���
	* @mark
	*/
	tstring GetKeyNameEx() const;

	/** @brief: ��ô���ѯ�ļ���
	* @mark
	*/
	const TCHAR* GetKeyName() const;

	/** @brief: �жϵ�ǰ��ֵ�Ĳ�ѯ�����Ƿ���Ч
	* @mark
	*/
	bool IsVaildCondition();

protected:
	/** @brief: �ж�ע��������Ƿ���Ч
	* @mark
	*/
	bool IsVaildForMainKey(HKEY hKey) const;

	/** @brief: ���ַ�������Сд��ĸת��Ϊ��д��ĸ
	* @mark
	*/
	static void ToUpperForString(tstring &szSrc);

	/** @brief: ������ת��Ϊ�ַ�����ʽ
	* @mark
	*/
	static tstring CtxObj2String(RegKeyCtx KeyCtxObj);

	/** @brief: �滻std::string����ı�����
	* @param[in|out] tstring&  srcStr  ��������ַ���
	* @param[in] tstring  oldStr  ���滻���ַ���
	* @param[in] tstring  newStr  �滻����ַ���
	* @param[in] int  nMatchCount  ƥ���������ΪС��1��ʱ�򣬽����ƥ�䵽���ַ���ȫ���滻
	* @return  ��
	* @mark
	*/
	static void ReplaceForString(tstring& srcStr, tstring oldStr, tstring newStr, int nMatchCount = 1);
};

/** @brief: �����ֵ���ݽṹ
* @mark ��ֵ�ж������ͣ���REG_SZ��REG_DWORD�������ڲ�ѯע����ʱ��
*		��Ҫִ�����β�ѯ�ſ���׼ȷ�������Ҫ�ļ�ֵ����һ��Ŀ����Ϊ�˻�ö�Ӧ��
*		��ֵ�������ͣ����ڶ������Ǹ��ݼ�ֵ��������������õļ�ֵ����ת���ɶ�Ӧ
*		�Ŀɲ��������ݸ�ʽ����������˼·������������������ݽṹ����ż�ֵ����
* @not  ע�⣬��������Ctx��ԭ��Ϊcontext
*/
class RegValueCtx 
{
public:
	RegValueCtx();
	~RegValueCtx();

	/** @brief: ���ؿ������캯��
	* @mark  
	*/
	RegValueCtx(const RegValueCtx &KeyValueObj);

	/** @brief: ��ʼ���ַ�����ֵ����
	* @mark  dwValueType����ΪREG_SZ��REG_EXPAND_SZ��REG_MULTI_SZ
	*/
	RegValueCtx(const TCHAR* szBuffer, unsigned long dwValueType = REG_SZ);

	/** @brief: ��ʼ��REG_QWORD��ֵ����
	* @mark  ��Ϊunsigned long lon��unsigned long���ݣ�������Ҫ����
	*        ָ������ֵ���ͣ����磺RegValueCtx((DWORD64)0x00000001)
	*/
	RegValueCtx(unsigned long long dw64Value);

	/** @brief: ��ʼ��REG_DWORD��ֵ����
	* @mark  ��Ϊunsigned long lon��unsigned long���ݣ�������Ҫ����
	*        ָ������ֵ���ͣ����磺RegValueCtx((DWORD)0x00000001)
	*/
	RegValueCtx(unsigned long dwValue);

	/** @brief: ��ʼ��REG_BINARY��ֵ����
	* @mark  
	*/
	RegValueCtx(void *pBuffer, unsigned long dwcbSize);

	//////////////////////���������//////////////////////
	/** @brief: ����==�����
	* @mark
	*/
	bool operator==(const RegValueCtx &dstObj) const;

	/** @brief: ����!=�����
	* @mark
	*/
	bool operator!=(const RegValueCtx &dstObj) const;

	/** @brief: ����<�����
	* @mark  Ϊ���ô���֧��STL������㷨�⣬������Ҫ
	*		 ��������Ϊ������Ա����
	*/
	bool operator<(const RegValueCtx &dstObj) const;

	/** @brief: ����=�����
	* @mark
	*/
	RegValueCtx& operator=(const RegValueCtx&dstObj);

public:
	/** @brief: ���ü�ֵ����
	* @mark
	*/
	void SetValueType(unsigned long dwNewType);

	/** @brief: ��õ�ǰ��ֵ������Ϣ
	* @mark
	*/
	unsigned long GetValueType() const;

	/** @brief: ��ü�ֵ����ռ�õ��ڴ��С
	* @mark
	*/
	unsigned long GetBufferSize() const;

private:
	/** @brief: ������ĳ�Ա����ȫ������
	* @mark
	*/
	void InitBufferDatas();

	/** @brief: �ͷ�ע����ֵ�Զ�����ָ��ָ����ڴ�
	* @mark
	*/
	void ReleaseMemory();

public:
	void *m_pBuffer;			 //ע����ֵ��Ӧ���ڴ�ָ��

private:
	unsigned long m_dwcbSize;	 //ע����ֵ������ڴ�����
	unsigned long m_dwVarType;	 //ע����ֵ���������ͣ�ͨ����REG_SZ��REG_DOWRD����Ϊ����

public:
	/** @brief: ���·���ע����ֵָ����ڴ��С������������
	* @param[in] unsigned long  dwcbSize  �����·�����ڴ��С
	* @mark  ͨ��ִ�д˺���֮�󣬽���ִ��GetBuffer()����
	*/
	void ResetMemorySize(unsigned long dwcbSize);

	/** @brief: ��ô��ע����ֵ������ָ��
	* @return  void* ���������ָ�벢û�б���ʼ��������ô�᷵��NULL
	* @mark  ͨ����ִ�д˺���֮ǰ��Ҫִ��ResetMemorySize()����
	*/
	void *GetBuffer() const;

	/** @brief: �жϵ�ǰע����ֵ�������Ƿ���Ч
	* @return  ����Ϊtrue����ô������Ч�ģ���֮����ʾ��Ч
	* @mark
	*/
	bool IsVaildMemory() const;

public:
	/** @brief: ��ӡ�ڴ���Ϣ����������ϻ���ʹ��DebugView����׽��Щ����
	* @mark
	*/
	void ShowMemoryContent();

public:
	/** @brief: ���DWORD��ֵ
	* @param[out] unsigned long&  dwValue   ����ע����REG_DWORD������
	* @return  �������Ϊtrue����˵��ȡֵ�ɹ���������ζ��ʧ��
	* @mark
	*/
	bool GetDWORD(unsigned long &dwValue);

	/** @brief: ����REG_DWORD����
	* @param[in] unsigned long&  dwNewValue  ��Ҫ���õ�RET_DWORD����
	* @mark ע����ִ�д˺���֮ǰҪִ��SetValueType()����
	*/
	void SetDWORD(unsigned long dwNewValue);

	/** @brief: ���DWORD64��ֵ
	* @param[out] unsigned long&  dwValue   ����ע����REG_DWORD������
	* @return  �������Ϊtrue����˵��ȡֵ�ɹ���������ζ��ʧ��
	* @mark
	*/
	bool GetQWORD(unsigned long long &qwValue);

	/** @brief: ����REG_DWORD����
	* @param[in] unsigned long&  dwNewValue  ��Ҫ���õ�RET_DWORD����
	* @mark ע����ִ�д˺���֮ǰҪִ��SetValueType()����
	*/
	void SetQWORD(unsigned long long qwNewValue);

	/** @brief: ���REG_SZ���ַ������͵�����
	* @param[out] tstring&  szBuffer  ��ŵ�ǰע����ֵ��Ӧ���ַ�������
	* @return  �������Ϊtrue����˵��ȡֵ�ɹ���������ζ��ʧ��
	* @mark 
	*/
	bool GetSTRING(tstring &szBuffer);

	/** @brief: ����REG_SZ���ַ������͵�����
	* @param[out] tstring  szBuffer  ��Ҫ���õ��ַ�������
	* @mark ע����ִ�д˺���֮ǰҪִ��SetValueType()����
	*/
	void  SetSTRING(tstring szNewValue);

	/** @brief: ���REG_BINARY��Ӧ����ֵ
	* @param[out] void*  lpBuffer  ��Ŵ����ݵ��ڴ�ָ�룬ע�⣬�����潫����ܵ���ĳЩ����ĳ���
	* @param[out] unsigned long&  dwcbSize  ��ǰ��������Ҫ���ڴ��С
	* @param[in] bool IsAllocMemory �Ƿ�̬ΪlpBuffer�����ڴ棬Ĭ���ǲ������
	* @return  �������Ϊtrue����˵��ȡֵ�ɹ���������ζ��ʧ��
	* @mark
	*/
	bool GetBINARY(void *lpBuffer, unsigned long& dwcbSize, bool IsAllocMemory = false);

	/** @brief: ��REG_BINARY������䵽��Ա������
	* @param[in] void*  lpBuffer  ��Ҫ��ֵ������������ڴ�ָ��
	* @param[in] unsigned long  dwcbSize  ��Ҫ��̬������ڴ��С
	* @mark ע����ִ�д˺���֮ǰҪִ��SetValueType()����
	*/
	void SetBINARY(void *lpBuffer, unsigned long dwcbSize);

	//////////////////////��չ��ֵ����//////////////////////
	/** @brief: ��ö��������ݵ�ʮ�������ַ���
	* @param[in] tstring  szSpliteFlag  ʮ�������ַ����ķָ���
	* @return  tstring  ���ش˶��������ݵ�ʮ�������ַ���
	* @mark  
	*/
	tstring GetHexStrForBuffer(tstring szSpliteFlag = _T("0x"),bool bIsXOREncode=false);

	/** @brief: ͨ��ʮ�������ַ�������ʼ��ע����ֵ����
	* @param[in] tstring  szHexStr  ͨ��ʮ��������������ʼ����ֵ����
	* @param[in] tstring  szSpliteFlag  ʮ����������֮��ķָ��
	* @return  ��
	* @mark  �˺�����SetBufferByHexStr()��һ��
	*/
	void SetBufferByHexStr(tstring szHexStr, tstring szSpliteFlag = _T("0x"), bool bIsXOREncode=false);

	/** @brief: ���ݷָ������ַ����ָ��һ���ַ�������
	* @param[in] tstring  SourceStr  ���ָ���ַ���
	* @param[in] tstring  FirstFlag  ��ʼ�ָ��
	* @param[in] tstring  FinalFlag  ��ֹ�ָ��
	* @return  std::vector<tstring>  �ָ����ַ�������
	* @mark  �˺�����GetHexStrForBuffer()��һ��
	*/
	static std::vector<tstring> GetArrBySpliteStringEx(tstring szSource,
		tstring szFirstFlag, tstring szFinalFlag);

	/** @brief: ��ʮ��������ֵ���ַ�����ʽ��ת����ʮ������ֵ��������ʽ��
	* @param[in] tstring  szHexStrForByte  ʮ���������ݵ��ַ�����ʽ
	* @return  int  ��Ӧ��ʮ���Ƶ���������
	* @mark 
	*/
	static size_t HexToInt(tstring szHex);

	/** @brief: ��ʮ���������ݽ���������
	* @param[in] tstring  szHexStr  ԭ��������
	* @param[in] size_t*  XORKeys  ��������
	* @param[in] size_t  nKeyCount  �������
	* @return  ���ܺ��ʮ�������ַ�������
	* @mark  ��һ������Ǽ��ܣ��ڶ������ǽ���
	*/
	static tstring XOREncodeEx(tstring szHexStr, tstring szSpliteFlag, size_t *XORKeys, size_t nKeyCount);
};

/** @brief: ö��ע�����ָ��·���µ������Ӽ�
* @param[in] HKEY  hKey  ע���ĸ�Ŀ¼
* @param[in] TCHAR*  szSubKeyPath  ��������Ŀ¼��ע���·��
* @param[out] void*  pParam  ���뵽�˻ص��������ⲿ����
* @return  ��
* @mark  ���һ������һ�㶼�ǽ���ָ�룬�����������ɱ���ֵ��������
*/
typedef  void(*OnCallBackEnumRegKey)(HKEY hKey, TCHAR *szSubKeyPath, void *pParam);

/** @brief: ö��ע�����ָ��·���µ����м�ֵ
* @param[in] TCHAR*  szValueName  ��ǰ�������õļ���
* @param[in] IKeyValueEx  KeyValueObj  ��õļ�ֵ����
* @param[out] void*  pParam  ���뵽�˻ص��������ⲿ����
* @return  ��
* @mark  ���һ������һ�㶼�ǽ���ָ�룬�����������ɱ���ֵ��������
*/
typedef  void(*OnCallBackEnumRegValue)(TCHAR *szValueName, RegValueCtx KeyValueObj, void *pParam);

namespace IRegEditor 
{
	//////////////////////������������//////////////////////
	/** @brief: �жϵ�ǰϵͳ�Ƿ�Ϊx64bitϵͳ
	* @return  ����Ϊtrue��˵����ǰ����ϵͳ��x64bit������Ϊx86bit
	* @mark
	*/
	bool IsOSX64();

	/** @brief: ���ַ����ָ�Ϊ��Ӧ��ע����Ŀ¼�Ͷ�Ӧ��ע���·��
	* @param[in] TCHAR*  szKeyPathWithRoot  ���ָ���ַ���
	* @param[out] HKEY&  hKey  �������õ�ע����Ŀ¼
	* @param[out] TCHAR*  szKeyPath  ������õ���ע���·��
	* @return  ����Ϊtrue�������������������򣬴���ʧ��
	* @mark
	*/
	bool AnalyzeKeyPathWithRoot(const TCHAR *szKeyPathWithRoot, HKEY &hKey, TCHAR *szKeyPath);

	//////////////////////ע����д��������//////////////////////
	/** @brief: ����x86��x64ϵͳ��ע���򿪷�ʽ
	* @param[in] HKEY  hKey  ��Ҫ�򿪵�ע������
	* @param[in] TCHAR*  szKeyPath  ��Ҫ�򿪵�ע���·��
	* @param[out] HKEY&  hSubKey  �ѱ��򿪵��Ӽ�
	* @return  �������ΪERROR_SUCCESS��ɹ���������Ҫ��ѯ�쳣����
	* @mark
	*/
	LSTATUS RegOpenKeyUtil(HKEY hKey, const TCHAR *szKeyPath, HKEY &hSubKey);

	/** @brief: ����x86��x64ϵͳ��ɾ��ָ��ע���Ŀ¼�ķ�ʽ
	* @param[in] HKEY  hKey  ��Ҫ�򿪵�ע������
	* @param[in] TCHAR*  szSubKey  ��Ҫ��ɾ����ע����·����Ϣ
	* @return  �������ΪERROR_SUCCESS��ɹ���������Ҫ��ѯ�쳣����
	* @mark  MSDNԭ�ģ�Deletes a subkey and its values. Note that key names are not case sensitive.
    *        64-bit Windows:On WOW64, 32-bit applications view a registry tree that is separate from 
	*		 the registry tree that 64-bit applications view. To enable an application to delete an entry 
	*		 in the alternate registry view, use the RegDeleteKeyEx function.
	*        �����֮��x32bit���������x64bitϵͳ��ע�������ʹ��RegDeleteKeyEx()����������
	*/
	LSTATUS RegDeleteKeyUtil(HKEY hKeyRoot, TCHAR *szSubKey);

	/** @brief: ͨ���ݹ�ķ�ʽ������ǰע���·���µ�������Ŀ¼
	* @param[in] HKEY  hKeyRoot  ע���·�����ڵĸ�Ŀ¼
	* @param[in] TCHAR*  lpSubKey  ��ɾ����ע���·������������Ŀ¼
	* @return  ����Ϊtrue�������������������򣬴���ʧ��
	* @mark
	*/
	bool RegDelNodeRecurse(HKEY hKeyRoot, TCHAR* szKeyPath);

	//////////////////////ע����ֵ��������//////////////////////

	/** @brief: ��ѯָ��ע��·���ļ�ֵ
	* @param[in] HKEY  hKey  ע���·�����ڵĸ�Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ����ѯ��ע���·������������Ŀ¼
	* @param[in] TCHAR*  szValueName  ����ѯ�ļ���
	* @param[out] IKeyValueEx&  KeyValue  ��õļ�ֵ����
	* @return  �������Ϊtrue�����ȡ�ɹ��������ȡʧ��
	* @mark
	*/
	bool GetRegValueEx(HKEY hKey, const TCHAR * szKeyPath, const TCHAR * szKeyName, RegValueCtx &KeyValue);

	/** @brief: ��ѯָ��ע��·���ļ�ֵ
	* @param[in] TCHAR*  szKeyPathWithRoot  �����˸�Ŀ¼��Ϣ��ע���·��
	* @param[in] TCHAR*  szKeyName  ����ѯ�ļ���
	* @param[out] IKeyValueEx&  KeyValue  ��õļ�ֵ����
	* @return  �������Ϊtrue�����ȡ�ɹ��������ȡʧ��
	* @mark
	*/
	bool GetRegValueEx(const TCHAR *szKeyPathWithRoot, const TCHAR * szKeyName, RegValueCtx & KeyValue);

	/** @brief: ����ָ����ע���·���ļ�ֵ
	* @param[in] HKEY  hKey  ע���·�����ڵĸ�Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ����ֵ��ע���·������������Ŀ¼
	* @param[in] TCHAR*  szValueName  ����ֵ�ļ���
	* @param[out] IKeyValueEx&  KeyValue  ����ֵ�ļ�ֵ����
	* @return  �������Ϊtrue�����ȡ�ɹ��������ȡʧ��
	* @mark  ֮�����Դ����KEYVALUESTRUCT&��Ϊ��ֵ��������Ϊ����ֵΪREG_BINARY��ʱ��
	*		 ������ΪpBinary��̬�����ڴ棬��ʱ�����ַ�ʽ���룬�����������ݳ��ֶ���ͷ�
	*/
	bool SetRegValueEx(HKEY hKey, const TCHAR *szKeyPath, const TCHAR *szKeyName, RegValueCtx KeyValue);

	/** @brief: ����ָ����ע���·���ļ�ֵ
	* @param[in] TCHAR*  szKeyPathWithRoot  ����ֵ��ע���·����������Ŀ¼
	* @param[in] TCHAR*  szValueName  ����ֵ�ļ���
	* @param[out] IKeyValueEx&  KeyValue  ����ֵ�ļ�ֵ����
	* @return  �������Ϊtrue�������óɹ�����������ʧ��
	* @mark  ֮�����Դ����KEYVALUESTRUCT&��Ϊ��ֵ��������Ϊ����ֵΪREG_BINARY��ʱ��
	*		 ������ΪpBinary��̬�����ڴ棬��ʱ�����ַ�ʽ���룬�����������ݳ��ֶ���ͷ�
	*/
	bool SetRegValueEx(const TCHAR *szKeyPathWithRoot, const TCHAR *szKeyName, RegValueCtx KeyValue);

	/** @brief: ɾ��ָ��ע���Ŀ¼
	* @param[in] HKEY  hKeyRoot  ��ɾ����ע���·�������ڸ�Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ��ɾ����ע���·������������Ŀ¼
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool DeleteRegSubKeyByPath(HKEY hKeyRoot, const TCHAR* szKeyPath);

	/** @brief: ɾ��ָ��ע���Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ��ɾ����ע���·�����������Ŀ¼
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool DeleteRegSubKeyByPath(const TCHAR *szKeyPathWithRoot);

	/** @brief: ɾ��ָ����������ֵ
	* @param[in] HKEY  hKey  ��ɾ����ע����ֵ�����ڸ�Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ��ɾ���ļ�ֵ������·������������Ŀ¼
	* @return  �������Ϊtrue���ɹ�ɾ��ָ����ֵ������ʧ��
	* @mark
	*/
	bool DeleteRegValueByPath(HKEY hKey, const TCHAR *szKeyPath, const TCHAR *szKeyName);

	/** @brief: ɾ��ָ����������ֵ
	* @param[in] TCHAR*  szKeyPath  ��ɾ���ļ�ֵ������·����������Ŀ¼
	* @param[in] TCHAR*  szKeyName  ��ɾ���ļ�ֵ�ļ���
	* @return  �������Ϊtrue���ɹ�ɾ��ָ����ֵ������ʧ��
	* @mark
	*/
	bool DeleteRegValueByPath(const TCHAR *szKeyPathWithRoot, const TCHAR *szKeyName);

	//////////////////////����ע���//////////////////////
	/** @brief: ����ָ��·���µ�������Ŀ¼
	* @param[in] HKEY  hKey  ע���·�����ڵĸ�Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ��������ע���·������������Ŀ¼
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool EnumRegSubKeys(HKEY hKey, const TCHAR * szKeyPath, OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam);

	/** @brief: ����ָ��·���µ�������Ŀ¼
	* @param[in] TCHAR*  szKeyPathWithRoot  ����ֵ��ע���·����������Ŀ¼
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool EnumRegSubKeys(const TCHAR * szKeyPathWithRoot, OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam);

	/** @brief: ����ָ��·���µ�������Ŀ¼
	* @param[in] HKEY  hKey  ע���·�����ڵĸ�Ŀ¼����������Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ��������ע���·��
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool EnumRegValues(HKEY hKey, const TCHAR * szKeyPath, OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam);

	/** @brief: ����ָ��·���µ�������Ŀ¼
	* @param[in] TCHAR*  szKeyPathWithRoot  ����ֵ��ע���·��
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool EnumRegValues(const TCHAR * szKeyPathWithRoot, OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam);

	//////////////////////��չ����//////////////////////
	/** @brief: ������ѯͬһע���·���µļ�ֵ��Ϣ
	* @param[in] HKEY  hKeyRoot  ����ѯ��ע���ĸ�Ŀ¼
	* @param[in] TCHAR*  szKeyPath  ��������Ŀ¼�Ĵ���ѯ��ע���·��
	* @param[in|out] std::map<tstring,IKeyValueEx>&  mapValues  ����ѯ�ļ����Լ���ѯ��Ľ��
	* @return  ����Ϊtruef����˵�������ҵ�������һ�������򽫴����ѯʧ��
	* @mark  mapValues�ĵ�һ����ֵ����Ϊ��ѯ�ļ������ڶ�����ֵ���Ǵ�Ų�ѯ���õĽ��
	*/
	bool BatchGetRegValueEx(HKEY hKeyRoot, const TCHAR* szKeyPath, std::map<tstring, RegValueCtx> &mapValues);

	/** @brief: ������ѯͬһע���·���µļ�ֵ��Ϣ
	* @param[in] TCHAR*  szKeyPathWithRoot  ������Ŀ¼�Ĵ���ѯ��ע���·��
	* @param[in|out] std::map<tstring,IKeyValueEx>&  mapValues  ����ѯ�ļ����Լ���ѯ��Ľ��
	* @return  ����Ϊtruef����˵�������ҵ�������һ�������򽫴����ѯʧ��
	* @mark  mapValues�ĵ�һ����ֵ����Ϊ��ѯ�ļ������ڶ�����ֵ���Ǵ�Ų�ѯ���õĽ��
	*/
	bool BatchGetRegValueEx(const TCHAR *szKeyPathWithRoot, std::map<tstring, RegValueCtx> &mapValues);

	//////////////////////////////////////IRegEditor Wrapper//////////////////////////////////////
	/** @brief: ��ѯָ��ע��·���ļ�ֵ
	* @param[in] RegKeyCtx  ConditionObj  ��ѯ����
	* @param[out] RegValueCtx&  ResultObj  ��õļ�ֵ����
	* @return  �������Ϊtrue�����ȡ�ɹ��������ȡʧ��
	* @mark
	*/
	bool GetRegValueEx(RegKeyCtx ConditionObj, RegValueCtx& ResultObj);

	/** @brief: ����ָ����ע���·���ļ�ֵ
	* @param[in] RegKeyCtx  ConditionObj  ��ѯ����
	* @param[out] RegValueCtx&  ResultObj  ��õļ�ֵ����
	* @return  �������Ϊtrue�������óɹ�����������ʧ��
	* @mark  ֮�����Դ����KEYVALUESTRUCT&��Ϊ��ֵ��������Ϊ����ֵΪREG_BINARY��ʱ��
	*		 ������ΪpBinary��̬�����ڴ棬��ʱ�����ַ�ʽ���룬�����������ݳ��ֶ���ͷ�
	*/
	bool SetRegValueEx(RegKeyCtx ConditionObj, RegValueCtx ResultObj);

	/** @brief: ɾ��ָ��ע���Ŀ¼
	* @param[in] RegKeyCtx  ConditionObj  ��ѯ����
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool DeleteRegSubKeyByPath(RegKeyCtx ConditionObj);

	/** @brief: ɾ��ָ����������ֵ
	* @param[in] RegKeyCtx  ConditionObj  ��ѯ����
	* @param[in] TCHAR*  szKeyName  ��ɾ���ļ�ֵ�ļ���
	* @return  �������Ϊtrue���ɹ�ɾ��ָ����ֵ������ʧ��
	* @mark
	*/
	bool DeleteRegValueByPath(RegKeyCtx ConditionObj);

	/** @brief: ����ָ��·���µ�������Ŀ¼
	* @param[in] RegKeyCtx  ConditionObj  ��ѯ����
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool EnumRegSubKeys(RegKeyCtx ConditionObj, OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam);

	/** @brief: ����ָ��·���µ�������Ŀ¼
	* @param[in] RegKeyCtx  ConditionObj  ��ѯ����
	* @return  �������Ϊtrue��������ɹ����������ʧ��
	* @mark
	*/
	bool EnumRegValues(RegKeyCtx ConditionObj, OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam);

	/** @brief: ������ѯͬһע���·���µļ�ֵ��Ϣ
	* @param[in] RegKeyCtx  ConditionObj  ��ѯ����
	* @param[in|out] std::map<RegKeyCtx,IKeyValueEx>&  mapValues  ����ѯ�ļ����Լ���ѯ��Ľ��
	* @return  ����Ϊtrue����˵�������ҵ�������һ�������򽫴����ѯʧ��
	* @mark  mapValues�ĵ�һ����ֵ����Ϊ��ѯ�ļ������ڶ�����ֵ���Ǵ�Ų�ѯ���õĽ��
	*/
	bool BatchGetRegValueEx(RegKeyCtx ConditionObj, std::map<RegKeyCtx, RegValueCtx> &mapValues);

	/** @brief: �������ü�ֵ��Ϣ
	* @param[in] std::map<RegKeyCtx,IKeyValueEx>&  mapValues  ����ѯ�ļ����Լ���ѯ��Ľ��
	* @return  ����Ϊfalse����˵��������һ����ֵʧ�ܣ������ǵ�ǰ����ֵ�ļ�ֵ��ϢΪ��
	* @mark  mapValues�ĵ�һ����ֵ����Ϊ��ѯ�ļ������ڶ�����ֵ���Ǵ�Ų�ѯ���õĽ��
	*/
	bool BatchSetRegValueEx(std::map<RegKeyCtx, RegValueCtx> mapValues);

}

#endif

/*********************************������ʾ************************************
void main()
{
	
	std::map<tstring, RegValueCtx> mapValues;
	mapValues[_T("Default User ID")] = RegValueCtx();
	mapValues[_T("Identity Login")] = RegValueCtx();
	mapValues[_T("Identity Ordinal")] = RegValueCtx();
	mapValues[_T("Last User ID")] = RegValueCtx();
	mapValues[_T("Last Username")] = RegValueCtx();
	mapValues[_T("Migrated7")] = RegValueCtx();
	IRegEditor::BatchGetRegValueEx(_T("HKEY_CURRENT_USER\\Identities"), mapValues);
	for (std::map<tstring, RegValueCtx>::iterator it = mapValues.begin(); it != mapValues.end(); it++)
	{
		OutputDebugString(it->first.c_str());
		OutputDebugString(_T("\n"));
		it->second.ShowMemoryContent();
	}
}                                                               
*****************************************************************************/
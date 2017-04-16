#ifndef __REGEDITOR_H
#define __REGEDITOR_H

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

/**
* @class RegKeyCtx
* @brief 定义注册表查询条件的数据结构                                                                                         
* @note 
* 要想查询某个键值，则必须要事先知道其子键路径和键名
* 然后根据其键名来查询对应的注册表信息。根据上述思路，
* 定义了下面这个数据结构来存放查询键值的条件。 另外，
* Ctx的原意为context
*/
class RegKeyCtx 
{
public:
	RegKeyCtx();

	/** 
	* @brief: 直接通过主键、子键路径以及键名来初始化实例
	* @param[in] HKEY  hNewKey  主键信息
	* @param[in] tstring  szNewKeyPath  子键路径信息
	* @param[in] tstring  szNewKeyName  键名信息
	* @note
	*
	*/
	RegKeyCtx(HKEY hNewKey, tstring szNewKeyPath, tstring szNewKeyName);

	/** 
	* @brief: 直接通过主键、子键路径以及键名来初始化实例
	* @param[in] tstring  szKeyPathWithRoot  包含了主键信息的子键路径
	* @param[in] tstring  szNewKeyName  键名信息
	* @note
	*
	*/
	RegKeyCtx(tstring szKeyPathWithRoot, tstring szKeyName);

	RegKeyCtx(const RegKeyCtx &RegKeyObj);

private:
	HKEY m_hKey;					  /**< 待查询的主键 */
	tstring m_szKeyPathWithoutRoot;	  /**< 待查询的子键路径 */
	tstring m_szKeyName;			  /**< 待查询的键名 */

public:
	bool operator<(const RegKeyCtx &dstObj) const;
	bool operator==(const RegKeyCtx & dstObj) const;

public:
	/** 
	* @brief: 设置待查询的注册表主键
	* @param[in] 参数类型  参数名  参数说明
	* @return bool 返回值为true，则设置成功
	* @note   
	* 
	*/
	bool SetMainKeyEx(HKEY hMainKey);

	/** 
	* @brief: 获取待查询的注册表根主键
	* @return HKEY 返回主根键
	* @note
	*
	*/
	HKEY GetMainKeyEx() const;

	/** 
	* @brief: 设置待查询的注册表路径
	* @return 无
	* @note
	*
	*/
	void SetKeyPathEx(tstring szKeyPath);

	/** 
	* @brief: 获得待查询的注册表路径
	* @return tstring 返回注册表路径
	* @note
	*
	*/
	tstring GetKeyPathEx() const;

	/** 
	*@brief: 获得待查询的注册表路径
	* @return const TCHAR*  返回注册表路径
	* @note
	*
	*/
	const TCHAR* GetKeyPath() const;

	/** 
	* @brief: 获得待查询的注册表路径（包含主键信息）
	* @return tstring  返回路径信息
	* @note
	*
	*/
	tstring GetKeyPathWithRoot() const;

	/** 
	* @brief: 设置待查询的键名
	* @return 无
	* @note
	*
	*/
	void SetKeyNameEx(tstring szKeyName);

	/** @brief: 获得待查询的键名
	* @return tstring  返回键名
	* @note
	*
	*/
	tstring GetKeyNameEx() const;

	/** @brief: 获得待查询的键名
	* @return const TCHAR*  返回键名
	* @note
	*
	*/
	const TCHAR* GetKeyName() const;

	/** @brief: 判断当前键值的查询条件是否有效
	* @return bool 返回值为true，则查询条件有效
	* @note
	*
	*/
	bool IsVaildCondition();

protected:
	/** @brief: 判断注册表根主键是否有效
	* @return bool 返回值为true，则根主键有效
	* @note
	*
	*/
	bool IsVaildForMainKey(HKEY hKey) const;

	/** @brief: 将字符串所有小写字母转换为大写字母
	* @return 无
	* @note
	*
	*/
	static void ToUpperForString(tstring &szSrc);

	/** @brief: 将本类转换为字符串形式
	* @param[in] RegKeyCtx  KeyCtxObj  待转换的数据
	* @return tstring 返回字符串数据
	* @note
	*
	*/
	static tstring CtxObj2String(RegKeyCtx KeyCtxObj);

	/** @brief: 替换std::string里的文本内容
	* @param[in|out] tstring&  srcStr  待处理的字符串
	* @param[in] tstring  oldStr  待替换的字符串
	* @param[in] tstring  newStr  替换后的字符串
	* @param[in] int  nMatchCount  匹配次数，设为小于1的时候，将会把匹配到的字符串全部替换
	* @return  无
	* @note
	*
	*/
	static void ReplaceForString(tstring& srcStr, tstring oldStr, tstring newStr, int nMatchCount = 1);
};

/**
* @class RegValueCtx
* @brief 定义键值数据结构                                                                                         
* @note 
* 键值有多种类型，如REG_SZ、REG_DWORD，并且在查询注册表的时候，
* 需要执行两次查询才可以准确获得所需要的键值，第一次目的是为了获得对应的
* 键值数据类型，而第二次则是根据键值数据类型来将获得的键值数据转换成对应
* 的可操作的数据格式。根据上述思路，定义了下面这个数据结构来存放键值数据
*/
class RegValueCtx 
{
public:
	RegValueCtx();
	~RegValueCtx();

	/** 
	* @brief: 重载拷贝构造函数
	* @note
	*/
	RegValueCtx(const RegValueCtx &KeyValueObj);

	/** 
	* @brief: 初始化字符串键值类型
	* @note
	* dwValueType可能为REG_SZ、REG_EXPAND_SZ、REG_MULTI_SZ
	*/
	RegValueCtx(const TCHAR* szBuffer, unsigned long dwValueType = REG_SZ);

	/** 
	* @brief: 初始化REG_QWORD键值类型
	* @note
	* 因为unsigned long lon和unsigned long兼容，所以需要特意
	* 指定其数值类型，比如：RegValueCtx((DWORD64)0x00000001)
	*/
	RegValueCtx(unsigned long long dw64Value);

	/** @brief: 初始化REG_DWORD键值类型
	* @note  
	* 因为unsigned long lon和unsigned long兼容，所以需要特意
	* 指定其数值类型，比如：RegValueCtx((DWORD)0x00000001)
	*/
	RegValueCtx(unsigned long dwValue);

	/** 
	* @brief: 初始化REG_BINARY键值类型
	* @note
	*
	*/
	RegValueCtx(void *pBuffer, unsigned long dwcbSize);

	bool operator==(const RegValueCtx &dstObj) const;
	bool operator!=(const RegValueCtx &dstObj) const;
	bool operator<(const RegValueCtx &dstObj) const;
	RegValueCtx& operator=(const RegValueCtx&dstObj);

public:
	
	/** 
	* @brief: 设置键值类型
	* @note
	*
	*/
	void SetValueType(unsigned long dwNewType);

	/** 
	* @brief: 获得当前键值类型信息
	* @note`
	*
	*/
	unsigned long GetValueType() const;

	/**
	* @brief: 获得键值数据占用的内存大小
	* @note
	*
	*/
	unsigned long GetBufferSize() const;

private:
	/** 
	* @brief: 将此类的成员变量全部置零
	* @note
	*
	*/
	void InitBufferDatas();

	/** 
	* @brief: 释放注册表键值对额数据指针指向的内存
	* @note
	*
	*/
	void ReleaseMemory();

public:
	void *m_pBuffer;			 /**< 注册表键值对应的内存指针 */

private:
	unsigned long m_dwcbSize;	 /**< 注册表键值所需的内存数据 */
	unsigned long m_dwVarType;	 /**< 注册表键值的数据类型，通常以REG_SZ、REG_DOWRD来作为区分 */

public:
	/** @brief: 重新分配注册表键值指针的内存大小，并将其置零
	* @param[in] unsigned long  dwcbSize  待重新分配的内存大小
	* @return 无
	* @note  
	* 通常执行此函数之后，将会执行GetBuffer()函数
	*/
	void ResetMemorySize(unsigned long dwcbSize);

	/** 
	* @brief: 获得存放注册表键值的数据指针
	* @return  void* 假设此数据指针并没有被初始化过，那么会返回NULL
	* @note  
	* 通常在执行此函数之前需要执行ResetMemorySize()函数
	*/
	void *GetBuffer() const;

	/** 
	* @brief: 判断当前注册表键值的数据是否有效
	* @return  返回为true，那么则是有效的，反之，表示无效
	* @note  
	* 
	*/
	bool IsVaildMemory() const;

public:
	/** 
	* @brief: 打印内存信息到输出窗口上或者使用DebugView来捕捉这些数据
	* @return  无
	* @note  
	* 
	*/
	void ShowMemoryContent();

public:
	/** 
	* @brief: 获得DWORD数值
	* @param[out] unsigned long&  dwValue   保存注册表的REG_DWORD的数据
	* @return  如果返回为true，则说明取值成功，否则意味着失败
	* @note
	*
	*/
	bool GetDWORD(unsigned long &dwValue);

	/** 
	* @brief: 设置REG_DWORD数据
	* @param[in] unsigned long&  dwNewValue  需要设置的RET_DWORD数据
	* @note 
	* 注意在执行此函数之前要执行SetValueType()函数
	*/
	void SetDWORD(unsigned long dwNewValue);

	/** 
	* @brief: 获得DWORD64数值
	* @param[out] unsigned long&  dwValue   保存注册表的REG_DWORD的数据
	* @return  如果返回为true，则说明取值成功，否则意味着失败
	* @note
	*
	*/
	bool GetQWORD(unsigned long long &qwValue);

	/** 
	* @brief: 设置REG_DWORD数据
	* @param[in] unsigned long&  dwNewValue  需要设置的RET_DWORD数据
	* @return  无
	* @note 
	* 注意在执行此函数之前要执行SetValueType()函数
	*/
	void SetQWORD(unsigned long long qwNewValue);

	/** 
	* @brief: 获得REG_SZ等字符串类型的数据
	* @param[out] tstring&  szBuffer  存放当前注册表键值对应的字符串数据
	* @return  如果返回为true，则说明取值成功，否则意味着失败
	* @note
	*
	*/
	bool GetSTRING(tstring &szBuffer);

	/** 
	* @brief: 设置REG_SZ等字符串类型的数据
	* @param[out] tstring  szBuffer  需要设置的字符串数据
	* @note
	* 注意在执行此函数之前要执行SetValueType()函数
	*/
	void  SetSTRING(tstring szNewValue);

	/** 
	* @brief: 获得REG_BINARY对应的数值
	* @param[out] void*  lpBuffer  存放此数据的内存指针，注意，这里面将会可能导致某些情况的出现
	* @param[out] unsigned long&  dwcbSize  当前数据所需要的内存大小
	* @param[in] bool IsAllocMemory 是否动态为lpBuffer分配内存，默认是不分配的
	* @return  如果返回为true，则说明取值成功，否则意味着失败
	* @note
	*
	*/
	bool GetBINARY(void *lpBuffer, unsigned long& dwcbSize, bool IsAllocMemory = false);

	/** 
	* @brief: 将REG_BINARY数据填充到成员变量里
	* @param[in] void*  lpBuffer  需要赋值到此类里面的内存指针
	* @param[in] unsigned long  dwcbSize  需要动态分配的内存大小
	* @note
	* 注意在执行此函数之前要执行SetValueType()函数
	*/
	void SetBINARY(void *lpBuffer, unsigned long dwcbSize);

	//////////////////////拓展键值操作//////////////////////
	/** 
	* @brief: 获得二进制数据的十六进制字符串
	* @param[in] tstring  szSpliteFlag  十六进制字符串的分隔符
	* @return  tstring  返回此二进制数据的十六进制字符串
	* @note
	*
	*/
	tstring GetHexStrForBuffer(tstring szSpliteFlag = _T("0x"),bool bIsXOREncode=false);

	/** 
	* @brief: 通过十六进制字符串来初始化注册表键值数据
	* @param[in] tstring  szHexStr  通过十六进制数据来初始化键值数据
	* @param[in] tstring  szSpliteFlag  十六进制数据之间的分割符
	* @return  无
	* @note
	* 此函数和SetBufferByHexStr()是一对
	*/
	void SetBufferByHexStr(tstring szHexStr, tstring szSpliteFlag = _T("0x"), bool bIsXOREncode=false);

	/** 
	* @brief: 根据分隔符将字符串分割成一个字符串数组
	* @param[in] tstring  SourceStr  待分割的字符串
	* @param[in] tstring  FirstFlag  起始分割符
	* @param[in] tstring  FinalFlag  终止分割符
	* @return  std::vector<tstring>  分割后的字符串数组
	* @note
	* 此函数和GetHexStrForBuffer()是一对
	*/
	static std::vector<tstring> GetArrBySpliteStringEx(tstring szSource,
		tstring szFirstFlag, tstring szFinalFlag);

	/** 
	* @brief: 将十六进制数值（字符串形式）转换成十进制数值（整型形式）
	* @param[in] tstring  szHexStrForByte  十六进制数据的字符串形式
	* @return  int  对应的十进制的整型数据
	* @note
	*
	*/
	static size_t HexToInt(tstring szHex);

	/** 
	* @brief: 对十六进制数据进行异或加密
	* @param[in] tstring  szHexStr  原本的数据
	* @param[in] size_t*  XORKeys  密码数组
	* @param[in] size_t  nKeyCount  密码个数
	* @return  加密后的十六进制字符串数据
	* @note
	* 第一次异或是加密，第二次则是解密
	*/
	static tstring XOREncodeEx(tstring szHexStr, tstring szSpliteFlag, size_t *XORKeys, size_t nKeyCount);
};

/** 
* @brief: 枚举注册表内指定路径下的所有子键
* @param[in] HKEY  hKey  注册表的根目录
* @param[in] TCHAR*  szSubKeyPath  不包含根目录的注册表路径
* @param[out] void*  pParam  传入到此回调函数的外部参数
* @return  无
* @note
* 最后一个参数一般都是将类指针，或者是其它可变数值传进来的
*/
typedef  void(*OnCallBackEnumRegKey)(HKEY hKey, TCHAR *szSubKeyPath, void *pParam);

/** 
* @brief: 枚举注册表内指定路径下的所有键值
* @param[in] TCHAR*  szValueName  当前遍历所得的键名
* @param[in] IKeyValueEx  KeyValueObj  获得的键值数据
* @param[out] void*  pParam  传入到此回调函数的外部参数
* @return  无
* @note
* 最后一个参数一般都是将类指针，或者是其它可变数值传进来的
*/
typedef  void(*OnCallBackEnumRegValue)(TCHAR *szValueName, RegValueCtx KeyValueObj, void *pParam);

namespace RegEditor 
{
	//////////////////////公共函数部分//////////////////////
	/** 
	* @brief: 判断当前系统是否为x64bit系统
	* @return  返回为true，说明当前操作系统是x64bit，否则为x86bit
	* @note
	*
	*/
	bool IsOSX64();

	/** 
	* @brief: 将字符串分割为对应的注册表根目录和对应的注册表路径
	* @param[in] TCHAR*  szKeyPathWithRoot  待分割的字符串
	* @param[out] HKEY&  hKey  分析后获得的注册表根目录
	* @param[out] TCHAR*  szKeyPath  分析后得到的注册表路径
	* @return  返回为true，表明分析正常，否则，代表失败
	* @note
	*
	*/
	bool AnalyzeKeyPathWithRoot(const TCHAR *szKeyPathWithRoot, HKEY &hKey, TCHAR *szKeyPath);

	//////////////////////注册表读写公共函数//////////////////////
	/** 
	* @brief: 兼容x86和x64系统的注册表打开方式
	* @param[in] HKEY  hKey  将要打开的注册表根键
	* @param[in] TCHAR*  szKeyPath  将要打开的注册表路径
	* @param[out] HKEY&  hSubKey  已被打开的子键
	* @return  如果返回为ERROR_SUCCESS则成功，否则需要查询异常代码
	* @note
	*
	*/
	LSTATUS RegOpenKeyUtil(HKEY hKey, const TCHAR *szKeyPath, HKEY &hSubKey);

	/** 
	* @brief: 兼容x86和x64系统的删除指定注册表目录的方式
	* @param[in] HKEY  hKey  将要打开的注册表根键
	* @param[in] TCHAR*  szSubKey  将要被删除的注册表的路径信息
	* @return  如果返回为ERROR_SUCCESS则成功，否则需要查询异常代码
	* @note
	* MSDN原文，Deletes a subkey and its values. Note that key names are not case sensitive.
    * 64-bit Windows:On WOW64, 32-bit applications view a registry tree that is separate from 
	* the registry tree that 64-bit applications view. To enable an application to delete an entry 
	* in the alternate registry view, use the RegDeleteKeyEx function.
	* 简而言之，x32bit程序想操作x64bit系统的注册表，建议使用RegDeleteKeyEx()函数来处理
	*/
	LSTATUS RegDeleteKeyUtil(HKEY hKeyRoot, TCHAR *szSubKey);

	/** 
	* @brief: 通过递归的方式来处理当前注册表路径下的所有子目录
	* @param[in] HKEY  hKeyRoot  注册表路径所在的根目录
	* @param[in] TCHAR*  lpSubKey  待删除的注册表路径，不包含根目录
	* @return  返回为true，表明分析正常，否则，代表失败
	* @note
	*
	*/
	bool RegDelNodeRecurse(HKEY hKeyRoot, TCHAR* szKeyPath);

	//////////////////////注册表键值操作部分//////////////////////

	/** 
	* @brief: 查询指定注册路径的键值
	* @param[in] HKEY  hKey  注册表路径所在的根目录
	* @param[in] TCHAR*  szKeyPath  待查询的注册表路径，不包含根目录
	* @param[in] TCHAR*  szValueName  待查询的键名
	* @param[out] IKeyValueEx&  KeyValue  获得的键值数据
	* @return  如果返回为true，则获取成功，否则获取失败
	* @note
	*
	*/
	bool GetRegValueEx(HKEY hKey, const TCHAR * szKeyPath, const TCHAR * szKeyName, RegValueCtx &KeyValue);

	/** 
	* @brief: 查询指定注册路径的键值
	* @param[in] TCHAR*  szKeyPathWithRoot  包含了根目录信息的注册表路径
	* @param[in] TCHAR*  szKeyName  待查询的键名
	* @param[out] IKeyValueEx&  KeyValue  获得的键值数据
	* @return  如果返回为true，则获取成功，否则获取失败
	* @note
	*
	*/
	bool GetRegValueEx(const TCHAR *szKeyPathWithRoot, const TCHAR * szKeyName, RegValueCtx & KeyValue);

	/** 
	* @brief: 设置指定的注册表路径的键值
	* @param[in] HKEY  hKey  注册表路径所在的根目录
	* @param[in] TCHAR*  szKeyPath  待赋值的注册表路径，不包含根目录
	* @param[in] TCHAR*  szValueName  待赋值的键名
	* @param[out] IKeyValueEx&  KeyValue  待赋值的键值数据
	* @return bool 如果返回为true，则获取成功，否则获取失败
	* @note  
	* 之所以以传入的KEYVALUESTRUCT&作为键值，这是因为当键值为REG_BINARY的时候，
	* 往往会为pBinary动态分配内存，此时以这种方式传入，将不会让数据出现多次释放
	*/
	bool SetRegValueEx(HKEY hKey, const TCHAR *szKeyPath, const TCHAR *szKeyName, RegValueCtx KeyValue);

	/** 
	* @brief: 设置指定的注册表路径的键值
	* @param[in] TCHAR*  szKeyPathWithRoot  待赋值的注册表路径，包含根目录
	* @param[in] TCHAR*  szValueName  待赋值的键名
	* @param[out] IKeyValueEx&  KeyValue  待赋值的键值数据
	* @return bool 如果返回为true，则设置成功，否则设置失败
	* @note  
	* 之所以以传入的KEYVALUESTRUCT&作为键值，这是因为当键值为REG_BINARY的时候，
	* 往往会为pBinary动态分配内存，此时以这种方式传入，将不会让数据出现多次释放
	*/
	bool SetRegValueEx(const TCHAR *szKeyPathWithRoot, const TCHAR *szKeyName, RegValueCtx KeyValue);

	/** 
	* @brief: 删除指定注册表目录
	* @param[in] HKEY  hKeyRoot  待删除的注册表路径的所在根目录
	* @param[in] TCHAR*  szKeyPath  待删除的注册表路径，不包含根目录
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool DeleteRegSubKeyByPath(HKEY hKeyRoot, const TCHAR* szKeyPath);

	/** 
	* @brief: 删除指定注册表目录
	* @param[in] TCHAR*  szKeyPath  待删除的注册表路径，包含其根目录
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool DeleteRegSubKeyByPath(const TCHAR *szKeyPathWithRoot);

	/** 
	* @brief: 删除指定键名、键值
	* @param[in] HKEY  hKey  待删除的注册表键值的所在根目录
	* @param[in] TCHAR*  szKeyPath  待删除的键值的所在路径，不包含根目录
	* @return bool 如果返回为true，成功删除指定键值，否则失败
	* @note
	*
	*/
	bool DeleteRegValueByPath(HKEY hKey, const TCHAR *szKeyPath, const TCHAR *szKeyName);

	/** 
	* @brief: 删除指定键名、键值
	* @param[in] TCHAR*  szKeyPath  待删除的键值的所在路径，包含根目录
	* @param[in] TCHAR*  szKeyName  待删除的键值的键名
	* @return bool 如果返回为true，成功删除指定键值，否则失败
	* @note
	*
	*/
	bool DeleteRegValueByPath(const TCHAR *szKeyPathWithRoot, const TCHAR *szKeyName);

	//////////////////////遍历注册表//////////////////////
	/** 
	* @brief: 遍历指定路径下的所有子目录
	* @param[in] HKEY  hKey  注册表路径所在的根目录
	* @param[in] TCHAR*  szKeyPath  待遍历的注册表路径，不包含根目录
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool EnumRegSubKeys(HKEY hKey, const TCHAR * szKeyPath, OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam);

	/** 
	* @brief: 遍历指定路径下的所有子目录
	* @param[in] TCHAR*  szKeyPathWithRoot  待赋值的注册表路径，包含根目录
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool EnumRegSubKeys(const TCHAR * szKeyPathWithRoot, OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam);

	/** 
	* @brief: 遍历指定路径下的所有子目录
	* @param[in] HKEY  hKey  注册表路径所在的根目录，不包含根目录
	* @param[in] TCHAR*  szKeyPath  待遍历的注册表路径
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool EnumRegValues(HKEY hKey, const TCHAR * szKeyPath, OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam);

	/** 
	* @brief: 遍历指定路径下的所有子目录
	* @param[in] TCHAR*  szKeyPathWithRoot  待赋值的注册表路径
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool EnumRegValues(const TCHAR * szKeyPathWithRoot, OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam);

	//////////////////////拓展操作//////////////////////
	/** 
	* @brief: 批量查询同一注册表路径下的键值信息
	* @param[in] HKEY  hKeyRoot  待查询的注册表的根目录
	* @param[in] TCHAR*  szKeyPath  不包含根目录的待查询的注册表路径
	* @param[in|out] std::map<tstring,IKeyValueEx>&  mapValues  待查询的键名以及查询后的结果
	* @return bool 返回为truef，则说明起码找到了其中一个，否则将代表查询失败
	* @note
	* mapValues的第一个数值将作为查询的键名，第二个数值则是存放查询所得的结果
	*/
	bool BatchGetRegValueEx(HKEY hKeyRoot, const TCHAR* szKeyPath, std::map<tstring, RegValueCtx> &mapValues);

	/** 
	* @brief: 批量查询同一注册表路径下的键值信息
	* @param[in] TCHAR*  szKeyPathWithRoot  包含根目录的待查询的注册表路径
	* @param[in|out] std::map<tstring,IKeyValueEx>&  mapValues  待查询的键名以及查询后的结果
	* @return bool 返回为truef，则说明起码找到了其中一个，否则将代表查询失败
	* @note  
	* mapValues的第一个数值将作为查询的键名，第二个数值则是存放查询所得的结果
	*/
	bool BatchGetRegValueEx(const TCHAR *szKeyPathWithRoot, std::map<tstring, RegValueCtx> &mapValues);

	//////////////////////////////////////IRegEditor Wrapper//////////////////////////////////////
	/** 
	* @brief: 查询指定注册路径的键值
	* @param[in] RegKeyCtx  ConditionObj  查询条件
	* @param[out] RegValueCtx&  ResultObj  获得的键值数据
	* @return bool 如果返回为true，则获取成功，否则获取失败
	* @note
	*
	*/
	bool GetRegValueEx(RegKeyCtx ConditionObj, RegValueCtx& ResultObj);

	/** 
	* @brief: 设置指定的注册表路径的键值
	* @param[in] RegKeyCtx  ConditionObj  查询条件
	* @param[out] RegValueCtx&  ResultObj  获得的键值数据
	* @return bool 如果返回为true，则设置成功，否则设置失败
	* @note
	* 之所以以传入的KEYVALUESTRUCT&作为键值，这是因为当键值为REG_BINARY的时候，
	* 往往会为pBinary动态分配内存，此时以这种方式传入，将不会让数据出现多次释放
	*/
	bool SetRegValueEx(RegKeyCtx ConditionObj, RegValueCtx ResultObj);

	/** 
	* @brief: 删除指定注册表目录
	* @param[in] RegKeyCtx  ConditionObj  查询条件
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool DeleteRegSubKeyByPath(RegKeyCtx ConditionObj);

	/** 
	* @brief: 删除指定键名、键值
	* @param[in] RegKeyCtx  ConditionObj  查询条件
	* @param[in] TCHAR*  szKeyName  待删除的键值的键名
	* @return bool 如果返回为true，成功删除指定键值，否则失败
	* @note
	*
	*/
	bool DeleteRegValueByPath(RegKeyCtx ConditionObj);

	/** 
	* @brief: 遍历指定路径下的所有子目录
	* @param[in] RegKeyCtx  ConditionObj  查询条件
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool EnumRegSubKeys(RegKeyCtx ConditionObj, OnCallBackEnumRegKey EnumRegSubKeyProc, void * pParam);

	/** 
	* @brief: 遍历指定路径下的所有子目录
	* @param[in] RegKeyCtx  ConditionObj  查询条件
	* @return bool 如果返回为true，则遍历成功，否则遍历失败
	* @note
	*
	*/
	bool EnumRegValues(RegKeyCtx ConditionObj, OnCallBackEnumRegValue EnumRegKeyValueProc, void * pParam);

	/** 
	* @brief: 批量查询同一注册表路径下的键值信息
	* @param[in] RegKeyCtx  ConditionObj  查询条件
	* @param[in|out] std::map<RegKeyCtx,IKeyValueEx>&  mapValues  待查询的键名以及查询后的结果
	* @return bool 返回为true，则说明起码找到了其中一个，否则将代表查询失败
	* @note
	* mapValues的第一个数值将作为查询的键名，第二个数值则是存放查询所得的结果
	*/
	bool BatchGetRegValueEx(RegKeyCtx ConditionObj, std::map<RegKeyCtx, RegValueCtx> &mapValues);

	/** 
	* @brief: 批量设置键值信息
	* @param[in] std::map<RegKeyCtx,IKeyValueEx>&  mapValues  待查询的键名以及查询后的结果
	* @return bool 返回为false，则说明起码有一个赋值失败，或者是当前待赋值的键值信息为空
	* @note
	* mapValues的第一个数值将作为查询的键名，第二个数值则是存放查询所得的结果
	*/
	bool BatchSetRegValueEx(std::map<RegKeyCtx, RegValueCtx> mapValues);

}

#endif

/*********************************调用演示************************************
void main()
{
	std::map<tstring, RegValueCtx> mapValues;
	mapValues[_T("Default User ID")] = RegValueCtx();
	mapValues[_T("Identity Login")] = RegValueCtx();
	mapValues[_T("Identity Ordinal")] = RegValueCtx();
	mapValues[_T("Last User ID")] = RegValueCtx();
	mapValues[_T("Last Username")] = RegValueCtx();
	mapValues[_T("Migrated7")] = RegValueCtx();
	RegEditor::BatchGetRegValueEx(_T("HKEY_CURRENT_USER\\Identities"), mapValues);
	for (std::map<tstring, RegValueCtx>::iterator it = mapValues.begin(); it != mapValues.end(); it++)
	{
		OutputDebugString(it->first.c_str());
		OutputDebugString(_T("\n"));
		it->second.ShowMemoryContent();
	}
}                                                               
*****************************************************************************/
#ifndef __CAUTOREGISTERFACTORY_H
#define __CAUTOREGISTERFACTORY_H

#include <map>
#include <windows.h>

/**
* @class IForwarder
* @brief ת��������                                                                                         
* @note   
* �ѹ���ģʽ�µ����ֳ��õ��÷�ʽ���г��󣬽������
* �����ɸ���[CloneClass()]/ֱ�ӵ���[InvokeClass()]
*/
template<typename TBase>
class IForwarder
{
public:
	virtual ~IForwarder() {}

public:
	/** 
	* @brief: ͨ���˽ӿ�������һ����ָ��ġ���ס�
	* @return TBase* ���ػ���ָ��ĸ���
	* @note   
	* 
	*/
	virtual TBase *CloneClass() = 0;

	/**
	* @brief: ͨ���˽ӿ���ֱ�ӵ�����ָ��
	* @return TBase* ���ػ���ָ��
	* @note
	*
	*/
	virtual TBase* InvokeClass() = 0;
};

template<typename TSub,typename TBase>
class TplClassTemplate :public IForwarder<TBase>
{
private:
	TSub m_Obj;

public:
	virtual ~TplClassTemplate() {}

public:
	virtual TBase* CloneClass() { return new TSub; }
	virtual TBase* InvokeClass() { return &m_Obj; }
};

/**
* @class CAutoRegisterFactory
* @brief һ��֧���Զ�ע��Ĺ�����ģ��                                                                                         
* @note   
* ע�⣬���TKey���Զ����������ͣ���ô���������ϵ�����Ŷ��
* ���忴std::map��������ͨ������Ҫ�����������<���������뿴
* https://msdn.microsoft.com/ZH-CN/library/s44w4h2s(v=VS.100,d=hv.2).aspx
*/
template<typename TKey,typename TBase>
class CAutoRegisterFactory
{
public:
	CAutoRegisterFactory() {}
	virtual ~CAutoRegisterFactory() 
	{
		std::map<TKey, IForwarder<TBase>*>::iterator it = m_mapClasses.begin();
		for (; it != m_mapClasses.end(); it++)
		{
			IForwarder<TBase>* pTemp = it->second;
			if (NULL != pTemp)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
	}

	/** 
	* @brief: �����ע��Ļ���ָ��ĸ���
	* @param[in] TKey  Key  ��������ָ�������
	* @return TBase* ���ػ���ָ�룬����Ҳ�������ôֱ�ӷ���NULL
	* @note   
	* ע�⣬���ô˺���֮�󣬻���Ҫ��֤һ������ָ���Ƿ���Ч
	*/
	TBase* CloneClass(TKey Key)
	{
		if (IsRegistered(Key))
		{
			return m_mapClasses[Key]->CloneClass();
		}
		OutputDebugStringA("���ؿ�ָ�룬���ಢû�б�ע�ᣡ����\n");
		return NULL;
	}

	/** 
	* @brief: �����ע��Ļ���ָ��
	* @param[in] TKey  Key  ��������ָ�������
	* @return TBase* ���ػ���ָ�룬����Ҳ�������ôֱ�ӷ���NULL
	* @note   
	* ע�⣬���ô˺���֮�󣬻���Ҫ��֤һ������ָ���Ƿ���Ч
	*/
	TBase* InvokeClass(TKey Key)
	{
		if (IsRegistered(Key))
		{
			return m_mapClasses[Key]->InvokeClass();
		}
		OutputDebugStringA("���ؿ�ָ�룬���ಢû�б�ע�ᣡ����\n");
		return NULL;
	}

	/** 
	* @brief: �ѹ�����ʵ��֮�е�ĳ����з�ע��
	* @param[in] TKey  Key  ��������ָ�������
	* @return ��
	* @note   
	* 
	*/
	void UnregisterFactory(TKey Key)
	{
		if(!IsRegistered(Key))
			return;
		IForwarder<TBase> *pTemp = m_mapClasses[Key];
		if (NULL != pTemp)
		{
			delete pTemp;
			pTemp = NULL;
			m_mapClasses.erase(Key);
		}
	}

	/** 
	* @brief: ��ĳ��ע�ᵽ������ʵ��֮��
	* @param[in] TKey  Key  ĳ�����ⲿ����ʱ�����õ���������ͨ�������ַ�����ö�ٵ�����
	* @param[in|out] IForwarder<TBase>*  pObj  ��ע�ᵽ������ʵ���Ļ���ָ�룬�Զ�������ָ��
	* @param[in] bool  bIsOverWrite  ���Ϊtrue��������ע����࣬����ֱ���ͷŴ�ע��Ļ���ָ��
	* @return ��
	* @note   
	* ������Զ�����������Ҫע�ᵽ���๤���Ļ���ָ�룬�����Ҫ���ظ�ע�����ָ���Զ������ͷ�
	*/
	void RegisterFactory(TKey Key, IForwarder<TBase> *pObj, bool bIsOverWrite = false)
	{
		if(NULL==pObj)
		{
			OutputDebugStringA("�ǲ������ˮ���㣬��ע��Ļ���ָ�뾹ȻΪ�գ�����\n");
			return;
		}

		if (IsRegistered(Key))
		{
			if (!bIsOverWrite)
			{
				OutputDebugStringA("�Ѿ��������ͷ��ظ�ע�����ָ���ˣ���Ҫ�ظ��ͷ�Ŷ��\n");
				delete pObj;
				pObj = NULL;
				return;
			}
			UnregisterFactory(Key);
		}
		m_mapClasses[Key] = pObj;
	}

private:
	std::map<TKey, IForwarder<TBase>*> m_mapClasses;

protected:
	bool IsRegistered(TKey Key) { return m_mapClasses.find(Key) != m_mapClasses.end()? true:false; }

};

#endif

/*********************************������ʾ************************************
#include <string>
#include <tchar.h>
#include <windows.h>
#pragma warning(disable:4996)

#ifndef tstring
#ifndef _UNICODE
#define tstring std::string
#else
#define tstring std::wstring
#endif
#endif

void DebugStringEx(TCHAR* str, ...)
{
	va_list args;
	TCHAR szText[1024] = { 0 };
	va_start(args, str);
	_vstprintf(szText, str, args);
	OutputDebugString(szText);
	va_end(args);
}

class IObject {
public:
	IObject() { m_szName = _T("IObject"); }
	virtual ~IObject() {}
	virtual const TCHAR * GetName() { return &m_szName[0]; }
	virtual void Print() { DebugStringEx(_T("%s:%s\n"), _T("��IObject��"), &m_szName[0]); }
protected:
	tstring m_szName;
};

class IObject1st :public IObject
{
public:
	IObject1st() { m_szName = _T("IObject1st"); }
	virtual ~IObject1st() {}
	virtual void Print() { DebugStringEx(_T("%s:%s\n"), _T("��IObject1st��"), &m_szName[0]); }
};

class IObject2nd :public IObject1st
{
public:
	IObject2nd() { m_szName = _T("IObject2nd"); }
	virtual ~IObject2nd() {}
	virtual void Print() { DebugStringEx(_T("%s:%s\n"), _T("��IObject2nd��"), &m_szName[0]); }
};

template<typename TBase>
class TplClassOfIObject
{
public:
	TplClassOfIObject() :m_pTemp(NULL)
	{
		m_pTemp = new TplClassTemplate<TBase, IObject>;
	}

	operator IForwarder<IObject>*()
	{
		return m_pTemp;
	}
private:
	IForwarder<IObject> *m_pTemp;
};

class CIObjectFactory
{
public:
	void RegisterIObject(IForwarder<IObject> *pObj, bool bIsOverWrite = false)
	{
		if (NULL == pObj)
			return;
		m_IObjectFactory.RegisterFactory(pObj->InvokeClass()->GetName(), pObj, bIsOverWrite);
	}

	void UnregisterIObject(tstring szName) { m_IObjectFactory.UnregisterFactory(szName); }

	IObject *InvokeIObject(tstring szName) { return m_IObjectFactory.InvokeClass(szName); };

private:
	CAutoRegisterFactory<tstring, IObject> m_IObjectFactory;
};

//ԭʼ��ע�᷽ʽ
void main1st()
{
	CAutoRegisterFactory<tstring, IObject> tempfactory;
	TplClassTemplate<IObject2nd, IObject>* pNew = new TplClassTemplate<IObject2nd, IObject>;
	tempfactory.RegisterFactory(pNew->InvokeClass()->GetName(), pNew);

	IObject *pTemp = tempfactory.InvokeClass(pNew->InvokeClass()->GetName());
	if (NULL != pTemp)
	{
		pTemp->Print();
	}
}

//�Ƽ���ע�᷽ʽ
void main2nd()
{
	CIObjectFactory tempfactory;
	tempfactory.RegisterIObject(TplClassOfIObject<IObject1st>());
	tempfactory.RegisterIObject(TplClassOfIObject<IObject2nd>());
	IObject *pTemp = tempfactory.InvokeIObject(_T("IObject1st"));
	if (NULL != pTemp)
	{
		pTemp->Print();
	}
}
*********************************������ʾ************************************/

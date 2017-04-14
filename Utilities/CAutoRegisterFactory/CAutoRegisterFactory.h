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
	* @brief: ͨ���˽ӿ�������һ����ָ��ġ����ס�
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
#ifndef __CAUTOREGISTERFACTORY_H
#define __CAUTOREGISTERFACTORY_H

#include <map>
#include <windows.h>

/**
* @class IForwarder
* @brief 转发抽象类                                                                                         
* @note   
* 把工厂模式下的两种常用调用方式进行抽象，结果如下
* ：生成副本[CloneClass()]/直接调用[InvokeClass()]
*/
template<typename TBase>
class IForwarder
{
public:
	virtual ~IForwarder() {}

public:
	/** 
	* @brief: 通过此接口来拷贝一份类指针的“马甲”
	* @return TBase* 返回基类指针的副本
	* @note   
	* 
	*/
	virtual TBase *CloneClass() = 0;

	/**
	* @brief: 通过此接口来直接调用类指针
	* @return TBase* 返回基类指针
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
* @brief 一个支持自动注册的工厂类模板                                                                                         
* @note   
* 注意，如果TKey是自定义数据类型，那么请重载其关系运算符哦，
* 具体看std::map的条件，通常仅需要重载运算符“<”，详情请看
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
	* @brief: 获得已注册的基类指针的副本
	* @param[in] TKey  Key  索引基类指针的条件
	* @return TBase* 返回基类指针，如果找不到，那么直接返回NULL
	* @note   
	* 注意，调用此函数之后，还需要验证一下所得指针是否有效
	*/
	TBase* CloneClass(TKey Key)
	{
		if (IsRegistered(Key))
		{
			return m_mapClasses[Key]->CloneClass();
		}
		OutputDebugStringA("返回空指针，此类并没有被注册！！！\n");
		return NULL;
	}

	/** 
	* @brief: 获得已注册的基类指针
	* @param[in] TKey  Key  索引基类指针的条件
	* @return TBase* 返回基类指针，如果找不到，那么直接返回NULL
	* @note   
	* 注意，调用此函数之后，还需要验证一下所得指针是否有效
	*/
	TBase* InvokeClass(TKey Key)
	{
		if (IsRegistered(Key))
		{
			return m_mapClasses[Key]->InvokeClass();
		}
		OutputDebugStringA("返回空指针，此类并没有被注册！！！\n");
		return NULL;
	}

	/** 
	* @brief: 把工厂类实例之中的某类进行反注册
	* @param[in] TKey  Key  索引基类指针的条件
	* @return 无
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
	* @brief: 将某类注册到工厂类实例之中
	* @param[in] TKey  Key  某类在外部检索时的所用到的条件，通常采用字符串或枚举等类型
	* @param[in|out] IForwarder<TBase>*  pObj  待注册到工厂类实例的基类指针，自动跳过空指针
	* @param[in] bool  bIsOverWrite  如果为true，则重新注册此类，否则，直接释放待注册的基类指针
	* @return 无
	* @note   
	* 本类会自动管理所有想要注册到本类工厂的基类指针，因此需要对重复注册的类指针自动进行释放
	*/
	void RegisterFactory(TKey Key, IForwarder<TBase> *pObj, bool bIsOverWrite = false)
	{
		if(NULL==pObj)
		{
			OutputDebugStringA("是不是想浑水摸鱼，待注册的基类指针竟然为空！！！\n");
			return;
		}

		if (IsRegistered(Key))
		{
			if (!bIsOverWrite)
			{
				OutputDebugStringA("已经在这里释放重复注册的类指针了，不要重复释放哦！\n");
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
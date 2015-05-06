/************************************************************************
* FileName : SRongClassFactory.cpp
* Autor    : xihu
* Info     : 2012-01
************************************************************************/
#include "stdafx.h"
#include "ClassFactory.h"


/***********************************************************************************
* 以下是实例接口创建的函数
***********************************************************************************/
/***********************************************************************************
* name		 : CreateInstance
* description: 创建实例的接口
* input	     : typeinfo:实例的类型;
* output	 : ppInterface:实例的接口;
* return	 : NA
* remark  	 : NA
***********************************************************************************/
void CSRongClassFactory::CreateInstance( const type_info& typeinfo ,void** ppInstance )
{
	// 根据不同的类型创建不同的实例
	map<type_info*,void*>::iterator pItor;
	pItor = m_InstanceList.find((type_info*)&typeinfo);
	if (pItor != m_InstanceList.end())
	{
		*ppInstance = pItor->second; 
	}
	else
	{
		*ppInstance = NULL; 
	}

	return ;
}

void CSRongClassFactory::RegType(const type_info& typeinfo,void* pInstance )
{
	m_InstanceList.insert(pair<type_info*,void*>((type_info*)&typeinfo,pInstance));
}
//////////////////////////////////////////////////////////////////////////end
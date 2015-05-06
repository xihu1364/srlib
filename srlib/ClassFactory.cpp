/************************************************************************
* FileName : SRongClassFactory.cpp
* Autor    : xihu
* Info     : 2012-01
************************************************************************/
#include "stdafx.h"
#include "ClassFactory.h"


/***********************************************************************************
* ������ʵ���ӿڴ����ĺ���
***********************************************************************************/
/***********************************************************************************
* name		 : CreateInstance
* description: ����ʵ���Ľӿ�
* input	     : typeinfo:ʵ��������;
* output	 : ppInterface:ʵ���Ľӿ�;
* return	 : NA
* remark  	 : NA
***********************************************************************************/
void CSRongClassFactory::CreateInstance( const type_info& typeinfo ,void** ppInstance )
{
	// ���ݲ�ͬ�����ʹ�����ͬ��ʵ��
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
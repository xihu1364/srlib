/************************************************************************
* FileName : SRongClassFactory.h
* Autor    : xihu
* Info     : 2012-01
************************************************************************/

#ifndef __SRONG_CLASSFACTORY_H__
#define __SRONG_CLASSFACTORY_H__

#include "Singleton.h"
#include <map>

using namespace std;
class CSRongClassFactory
{
public:
	/***********************************************************************************
		* ʵ���ӿڴ����ĺ���
		***********************************************************************************/
		void CreateInstance( const type_info& typeinfo ,void** ppInstance );

		void RegType(const type_info& typeinfo,void* pInstance );
	private:
		/***********************************************************************************
		* ���켰��������
		***********************************************************************************/
		CSRongClassFactory() {};
		virtual ~CSRongClassFactory() {};

		map<type_info*,void*> m_InstanceList;

	//< ������Ϊ����ģʽ 
	DECLARE_SINGLETON_CLASS( CSRongClassFactory )
};

/***********************************************************************************
* ������ȫ�ֵĹ���ӿں���
***********************************************************************************/
/***********************************************************************************
* name       : CreateInstance
* description: ����ʵ������
* input      : NA
* output     : NA
* return     : T*����ʵ������
* remark     : TΪ���ض����ʵ������
***********************************************************************************/
template <typename T>
inline T* CreateInstance()
{
	T* pInstance = NULL;
	CSRongClassFactory* pClassFactory = CSRongSingleton<CSRongClassFactory>::CreateInstance();
	if (NULL != pClassFactory)
	{
 		pClassFactory->CreateInstance( typeid( T ), (void**)&pInstance );
	}
	
	return pInstance;
}



#endif 

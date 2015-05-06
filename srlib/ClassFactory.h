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
		* 实例接口创建的函数
		***********************************************************************************/
		void CreateInstance( const type_info& typeinfo ,void** ppInstance );

		void RegType(const type_info& typeinfo,void* pInstance );
	private:
		/***********************************************************************************
		* 构造及析构函数
		***********************************************************************************/
		CSRongClassFactory() {};
		virtual ~CSRongClassFactory() {};

		map<type_info*,void*> m_InstanceList;

	//< 声明类为单件模式 
	DECLARE_SINGLETON_CLASS( CSRongClassFactory )
};

/***********************************************************************************
* 以下是全局的构造接口函数
***********************************************************************************/
/***********************************************************************************
* name       : CreateInstance
* description: 创建实例对象
* input      : NA
* output     : NA
* return     : T*返回实例对象
* remark     : T为返回对象的实例类型
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

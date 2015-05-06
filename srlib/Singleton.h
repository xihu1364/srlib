/******************************************************************
*  FileName £ºSRongSingleton.h
*  Autor    £ºxihu
*  Info	    £ºNA
*  Data     £ºNA
*******************************************************************/
#ifndef  __SRongSingleton_H__
#define	 __SRongSingleton_H__


template <typename T>
class CSRongSingleton
{
public:
	static inline T* CreateInstance();
protected:
	CSRongSingleton(){};
	~CSRongSingleton(){};
	CSRongSingleton(const CSRongSingleton& src){};
	CSRongSingleton& operator=(const CSRongSingleton&){};

	static T* m_pInstance;
};

template <typename T>
	T* CSRongSingleton<T>::m_pInstance;


template <typename T>
	T* CSRongSingleton<T>::CreateInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new T;
		//m_pInstance = (T*)::operator new(sizeof(T));
	}
	return m_pInstance;
}

#define DECLARE_SINGLETON_CLASS(class_name)  friend class CSRongSingleton<class_name>;


#endif
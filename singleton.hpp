/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file sigleton.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ʮһ�� 2018
 * 
 * ��ʵ��ģ����
 * �ṩ���̰߳�ȫ�ĵ�ʵ������
 *
 */

#ifndef __DAXIA_SINGLETON_HPP
#define __DAXIA_SINGLETON_HPP
#include <mutex>

namespace daxia
{
	template<class T>
	class Singleton
	{
	protected:
		Singleton() = delete;
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
	public:
		~Singleton(){}
	public:
		static T& Instance()
		{
			std::call_once(flag_, [&]()
			{
				static T v;
				instance_ = &v;
			});

			return *instance_;
		}
	private:
		static T* instance_;
		static std::once_flag flag_;
	};

	template<class T>
	T* Singleton<T>::instance_;

	template<class T>
	std::once_flag Singleton<T>::flag_;
}

#endif // !__DAXIA_SINGLETON_HPP

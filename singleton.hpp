/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file sigleton.hpp
 * \author 漓江里的大虾
 * \date 十一月 2018
 * 
 * 单实例模板类
 * 提供多线程安全的单实例功能
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

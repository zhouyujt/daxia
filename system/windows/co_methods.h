/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file co_methods.h
* \author �콭��Ĵ�Ϻ
* \date ʮ�� 2021
*
* Э���ڲ�ʹ�õĿɱ����ѵ�һЩ������
* ��Щ�������ú󣬻ᱻ��������ʱ����֮����������ض�ʱ���ٴλָ�֮
*
*/
#ifdef _WIN32
#ifndef __DAXIA_SYSTEM_WINDOWS_COMETHODS_H
#define __DAXIA_SYSTEM_WINDOWS_COMETHODS_H
#include <functional>
#include <future>

#define FUTURE_CAST(x) reinterpret_cast<std::future<void>*>(&x)

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class Coroutine;

			class CoMethods
			{
				friend Coroutine;
			protected:
				CoMethods(Coroutine* coroutine);
				~CoMethods();
			public:
				// ˯��ָ����ʱ�䣨��λ:���룩
				void CoSleep(size_t milliseconds);
				// ������ǰʱ��Ƭ
				void CoYield();
				// ����,��future״̬Ϊstd::future_status::readyʱ�ָ�
				void CoWait(const std::future<void>* future);
			private:
				Coroutine* couroutine_;
			};
		}
	}
}

#endif // !__DAXIA_COROUTINE_COMETHODS_H
#endif // !_WIN32
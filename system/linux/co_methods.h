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
#ifdef __linux__
#ifndef __DAXIA_SYSTEM_LINUX_COMETHODS_H
#define __DAXIA_SYSTEM_LINUX_COMETHODS_H
#include <functional>
#include <future>

#define WAIT_FUTURE(future) [&](){return future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;}

namespace daxia
{
	namespace system
	{
		namespace linux
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
				// ����,������ָ������ʱ������
				void CoWait(std::function<bool()>&& wakeupCondition);
			private:
				Coroutine* couroutine_;
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_LINUX_COMETHODS_H
#endif // !__linux__
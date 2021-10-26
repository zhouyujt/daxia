/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file co_methods.h
* \author 漓江里的大虾
* \date 十月 2021
*
* 协程内部使用的可被唤醒的一些方法。
* 这些方法调用后，会被调度器暂时挂起，之后调度器在特定时机再次恢复之
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
				// 睡眠指定的时间（单位:毫秒）
				void CoSleep(size_t milliseconds);
				// 放弃当前时间片
				void CoYield();
				// 挂起,当满足指定条件时被唤醒
				void CoWait(std::function<bool()>&& wakeupCondition);
			private:
				Coroutine* couroutine_;
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_LINUX_COMETHODS_H
#endif // !__linux__
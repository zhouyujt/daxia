/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file this_coroutine.h
* \author 漓江里的大虾
* \date 十月 2021
*
* 协程内部使用的可被唤醒的一些方法。
* 这些方法调用后，当前协程会被调度器暂时挂起，之后调度器在特定时机再次恢复之
* 注意：只能在协程中使用这些方法，如果不是协程调用，则会出现未知结果
*/
#ifdef __linux__
#ifndef __DAXIA_SYSTEM_LINUX_THIS_COROUTINE_H
#define __DAXIA_SYSTEM_LINUX_THIS_COROUTINE_H
#include <functional>
#include <future>

#define WAIT_FUTURE(future) [&](){return future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;}

namespace daxia
{
	namespace system
	{
		namespace linux
		{
			namespace this_coroutine
			{
				// 睡眠指定的时间（单位:毫秒）
				void CoSleep(size_t milliseconds);
				// 放弃当前时间片
				void CoYield();
				// 挂起,当满足指定条件时被唤醒
				void CoWait(std::function<bool()>&& wakeupCondition);
			}
		}
	}
}

#endif // !__DAXIA_SYSTEM_LINUX_THIS_COROUTINE_H
#endif // !__linux__
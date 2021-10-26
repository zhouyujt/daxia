/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file coroutine.h
* \author 漓江里的大虾
* \date 十月 2021
*
* 协程对象
*
*/
#ifdef __linux__
#ifndef __DAXIA_SYSTEM_LINUX_COROUTINE_H
#define __DAXIA_SYSTEM_LINUX_COROUTINE_H
#include <functional>
#include <future>
#include <semaphore.h>
#include <ucontext.h>
#include "co_methods.h"
#include "../datetime.h"

namespace daxia
{
	namespace system
	{
		namespace linux
		{
			class CoScheduler;

			class Coroutine
			{
				friend CoScheduler;
				friend CoMethods;
			protected:
				Coroutine(std::function<void(CoMethods& coMethods)> fiber, long long id, ucontext_t* mainFiber);
			public:
				~Coroutine();
			public:
				// 获取协程ID
				long long Id() const { return id_; }
				// 等待协程执行完毕
				void Join();
				// 强制结束协程（当协程体内执行CoMethods相关方法时强制结束）
				void Terminate();
			private:
				// 协程ID
				long long id_;
				// 被唤醒（执行）的次数
				long long wakeupCount_;
				// 执行完毕标志
				bool complete_;
				// 执行完毕事件对象
				sem_t completeEvent_;
				// 协程体
				std::function<void()> fiberStartRoutine_;
				// 协程体内支持的方法
				CoMethods methods_;
				// 当前协程环境
				ucontext_t ctx_;
				// 主协程环境
				ucontext_t* mainCtx_;
			private:
				// 强制结束标志
				bool terminate_;
				// 睡眠时长
				size_t sleepMilliseconds_;
				// 睡眠时间戳
				daxia::system::DateTime sleepTimestamp_;
				// 挂起标志
				bool yield_;
				// 唤醒条件
				const std::future<void>* wakeupCondition_;
			private:
				std::shared_ptr<char> stack_;
				static void fiberStartRoutine(Coroutine* co);
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_LINUX_COROUTINE_H
#endif // !__linux__
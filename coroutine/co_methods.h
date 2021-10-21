/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file coroutine.h
* \author 漓江里的大虾
* \date 十月 2021
*
* 协程内部使用的可被唤醒的一些方法。
* 这些方法调用后，会被调度器暂时挂起，之后调度器在特定时机再次恢复之
*
*/
#ifndef __DAXIA_COROUTINE_COMETHODS_H
#define __DAXIA_COROUTINE_COMETHODS_H
#include <functional>
#include <setjmp.h>

namespace daxia
{
	namespace coroutine
	{
		class Coroutine;

		class CoMethods
		{
			friend Coroutine;
		protected:
			CoMethods(jmp_buf& root,jmp_buf& context);
			~CoMethods();
		public:
			// 睡眠指定的时间（单位:毫秒）
			void CoSleep(size_t milliseconds);
			// 挂起,当满足指定条件时被唤醒
			void CoYield(std::function<void()> wakeupCondition);
		private:
			jmp_buf& root_;
			jmp_buf& context_;
		};
	}
}

#endif // !__DAXIA_COROUTINE_COMETHODS_H
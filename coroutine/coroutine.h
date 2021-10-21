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
#ifndef __DAXIA_COROUTINE_COROUTINE_H
#define __DAXIA_COROUTINE_COROUTINE_H
#include <functional>
#include <setjmp.h>
#include "co_methods.h"
#include "../system/datetime.h"

namespace daxia
{
	namespace coroutine
	{
		class Scheduler;

		class Coroutine
		{
			friend Scheduler;
		protected:
			Coroutine(std::function<void(CoMethods& coMethods)> fiber, long long id, jmp_buf& root);
		public:
			~Coroutine();
		public:
			long long Id() const { return id_; }
			void Join();
			void Terminate();
		private:
			std::function<void(CoMethods& coMethods)> fiber_;
			long long id_;
			long long wakeupCount_;
			daxia::system::DateTime yieldTimestamp_;
			CoMethods methods_;
			jmp_buf context_;
		};
	}
}

#endif // !__DAXIA_COROUTINE_COROUTINE_H
/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file coroutine.h
* \author 漓江里的大虾
* \date 八月 2021
*
* 协程对象
*
*/
#ifndef __DAXIA_COROUTINE_COROUTINE_H
#define __DAXIA_COROUTINE_COROUTINE_H
#include <functional>

namespace daxia
{
	namespace coroutine
	{
		class Scheduler;
		class Coroutine
		{
			friend Scheduler;
		protected:
			Coroutine(std::function<void()> fiber,long long id);
			~Coroutine();
		public:
			long long Id() const { return id_; }
		public:
			void CoSleep();
			void CoYield();
			void CoResume();
			void CoJoin();
		private:
			std::function<void()> fiber_;
			long long id_;
		};
	}
}

#endif // !__DAXIA_COROUTINE_COROUTINE_H
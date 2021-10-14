/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file coroutine.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* Э�̶���
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
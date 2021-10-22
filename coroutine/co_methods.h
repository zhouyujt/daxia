/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file coroutine.h
* \author �콭��Ĵ�Ϻ
* \date ʮ�� 2021
*
* Э���ڲ�ʹ�õĿɱ����ѵ�һЩ������
* ��Щ�������ú󣬻ᱻ��������ʱ����֮����������ض�ʱ���ٴλָ�֮
*
*/
#ifndef __DAXIA_COROUTINE_COMETHODS_H
#define __DAXIA_COROUTINE_COMETHODS_H
#include <functional>

namespace daxia
{
	namespace coroutine
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
			void CoWait(std::function<bool()> wakeupCondition);
		private:
			Coroutine* couroutine_;
		};
	}
}

#endif // !__DAXIA_COROUTINE_COMETHODS_H
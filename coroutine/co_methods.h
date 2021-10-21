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
			// ˯��ָ����ʱ�䣨��λ:���룩
			void CoSleep(size_t milliseconds);
			// ����,������ָ������ʱ������
			void CoYield(std::function<void()> wakeupCondition);
		private:
			jmp_buf& root_;
			jmp_buf& context_;
		};
	}
}

#endif // !__DAXIA_COROUTINE_COMETHODS_H
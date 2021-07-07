/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file defer.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2017
 * 
 * ģ��ʵ��golang���defer���塣�����ں�������ǰִ��һЩ������
 *
 */

#ifndef __DAXIA_DEFER_HPP
#define __DAXIA_DEFER_HPP

#include <stack>
#include <functional>

#define deferinit() daxia::DeferStack deferStack;

#define defer deferStack<<[&]()

#define return(...)  while (true)\
	{\
		auto f = deferStack.Pop();\
		if(f)\
		{\
			f();\
		}\
		else\
		{\
			break;\
		}\
	}\
	return __VA_ARGS__;

namespace daxia
{
	class DeferStack
	{
	public:
		typedef std::function<void()> deferFun;
	public:
		DeferStack(){}
		~DeferStack(){}
	public:
		void Push(deferFun fun)
		{
			funstack_.push(fun);
		}

		deferFun Pop()
		{
			deferFun fun;
			if (!funstack_.empty())
			{
				fun = funstack_.top();
				funstack_.pop();
			}

			return fun;
		}

		void operator <<(deferFun fun)
		{
			Push(fun);
		}
	private:
		std::stack<deferFun> funstack_;
	};
}

#endif // !__DAXIA_DEFER_HPP
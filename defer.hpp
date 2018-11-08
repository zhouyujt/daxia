/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file defer.hpp
 * \author 漓江里的大虾
 * \date 八月 2017
 * 
 * 模仿实现golang里的defer语义。用来在函数返回前执行一些操作。
 *
 */

#ifndef __DAXIA_DEFER_HPP
#define __DAXIA_DEFER_HPP

#include <stack>
#include <functional>

#define deferinit() daxia::DeferStack deferStack;

#define defer deferStack<<

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
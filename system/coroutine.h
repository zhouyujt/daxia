/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file coroutine.h
* \author 漓江里的大虾
* \date 十月 2021
*
*  跨平台协程
*
*/
#ifndef __DAXIA_SYSTEM_COROUTINE_H
#define __DAXIA_SYSTEM_COROUTINE_H
#ifdef _WIN32
#include "windows/co_scheduler.h"
#include "windows/this_coroutine.h"
#else
#include "linux/co_scheduler.h"
#endif
namespace daxia
{
	namespace system
	{
#ifdef _WIN32
		using CoScheduler = daxia::system::windows::CoScheduler;
		using Coroutine = daxia::system::windows::Coroutine;
		namespace this_coroutine = daxia::system::windows::this_coroutine;
#else
		using CoScheduler = daxia::system::linux::CoScheduler;
		using Coroutine = daxia::system::linux::Coroutine;
		namespace this_coroutine = daxia::system::linux::this_coroutine;
#endif
	}
}

#endif // !__DAXIA_SYSTEM_COROUTINE_H
/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file coroutine.h
* \author �콭��Ĵ�Ϻ
* \date ʮ�� 2021
*
*  ��ƽ̨Э��
*
*/
#ifndef __DAXIA_SYSTEM_COROUTINE_H
#define __DAXIA_SYSTEM_COROUTINE_H
#include "windows/co_scheduler.h"
namespace daxia
{
	namespace system
	{
#ifdef _WIN32
		using CoScheduler = daxia::system::windows::CoScheduler;
		using CoMethods = daxia::system::windows::CoMethods;
		using Coroutine = daxia::system::windows::Coroutine;
#else
#endif
	}
}

#endif // !__DAXIA_SYSTEM_COROUTINE_H
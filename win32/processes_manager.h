/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file processes_manager.h
* \author 漓江里的大虾
* \date 六月 2021
*
* WIN32 进程相关的操作
*
*/
#ifndef _DAXIA_WIN32_PROCESSESMANAGER_H
#define _DAXIA_WIN32_PROCESSESMANAGER_H
#include <functional>
#include "process.h"

namespace daxia
{
	namespace win32
	{
		class Process;
		class ProcessesManager
		{
		public:
			ProcessesManager();
			~ProcessesManager();
		public:
			enum EnumType
			{
				EnumType_CreateToolhelp32Snapshot,
				EnumType_EnumProcesses,
				EnumType_WTSEnumerateProcess,
				EnumType_NtQuerySystemInfo
			};
		public:
			static void Enum(std::function<bool(std::shared_ptr<Process>)> fun, EnumType type = EnumType_CreateToolhelp32Snapshot);
		private:
			static void enumByCreateToolhelp32Snapsho(std::function<bool(std::shared_ptr<Process>)> fun);
			static void enumByEnumProcesses(std::function<bool(std::shared_ptr<Process>)> fun);
			static void enumByWTSEnumerateProcess(std::function<bool(std::shared_ptr<Process>)> fun);
			static void enumByNtQuerySystemInfo(std::function<bool(std::shared_ptr<Process>)> fun);
		};
	}
}
#endif // !_DAXIA_WIN32_PROCESSESMANAGER_H


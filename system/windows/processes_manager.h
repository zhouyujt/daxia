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
#ifdef _MSC_VER
#ifndef __DAXIA_SYSTEM_WINDOWS_PROCESSESMANAGER_H
#define __DAXIA_SYSTEM_WINDOWS_PROCESSESMANAGER_H
#include <functional>
#include "../../string.hpp"
#include "process.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class Process;
			class ProcessesManager
			{
			protected:
				ProcessesManager();
				~ProcessesManager();

				// STL风格遍历
			public:
				class iterator
				{
					friend ProcessesManager;
				public:
					iterator();
				private:
					iterator(std::shared_ptr<void> handle, std::shared_ptr<Process> process);
				public:
					iterator& operator++();
					bool operator==(const iterator& iter) const;
					bool operator!=(const iterator& iter) const;
					std::shared_ptr<Process> operator->();
					std::shared_ptr<Process> operator*();
					iterator& operator=(const iterator& iter);
				private:
					std::shared_ptr<void> handle_;
					std::shared_ptr<Process> process_;
				};
				static iterator begin();
				static iterator end();
				static iterator find(const daxia::tstring& name, const iterator& pos = end());
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
				static bool HasProcess(const daxia::tstring& name);
			private:
				static void enumByCreateToolhelp32Snapsho(std::function<bool(std::shared_ptr<Process>)> fun);
				static void enumByEnumProcesses(std::function<bool(std::shared_ptr<Process>)> fun);
				static void enumByWTSEnumerateProcess(std::function<bool(std::shared_ptr<Process>)> fun);
				static void enumByNtQuerySystemInfo(std::function<bool(std::shared_ptr<Process>)> fun);
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_PROCESSESMANAGER_H
#endif // !_MSC_VER


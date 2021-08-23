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
#ifndef __DAXIA_SYSTEM_WINDOWS_FIND_PROCESS_H
#define __DAXIA_SYSTEM_WINDOWS_FIND_PROCESS_H
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
			class FindProcess
			{
			public:
				enum Type
				{
					CreateToolhelp32Snapshot,
					EnumProcesses,
					WTSEnumerateProcess,
					NtQuerySystemInfo
				};
			public:
				FindProcess(Type type = CreateToolhelp32Snapshot);
				~FindProcess();

				// STL风格遍历
			public:
				class iterator
				{
					friend FindProcess;
				public:
					iterator();
				private:
					iterator(std::shared_ptr<void> handle, std::shared_ptr<Process> process);
				public:
					iterator& operator++();
					bool operator==(const iterator& iter) const;
					bool operator!=(const iterator& iter) const;
					const std::shared_ptr<Process> operator->() const;
					const std::shared_ptr<Process> operator*() const;
					std::shared_ptr<Process> operator->();
					std::shared_ptr<Process> operator*();
					iterator& operator=(const iterator& iter);
				private:
					std::shared_ptr<void> handle_;
					std::shared_ptr<Process> process_;
				};
				iterator begin();
				iterator end();
				iterator find(const char* name, const iterator* pos = nullptr);
				iterator find(const wchar_t* name, const iterator* pos = nullptr);
				iterator find(unsigned long pid);
			private:
				Type type_;
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_FIND_PROCESS_H
#endif // !_MSC_VER


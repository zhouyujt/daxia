#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include "processes_manager.h"
#include "process.h"
#include "../encode/strconv.h"

namespace daxia
{
	namespace win32
	{

		ProcessesManager::ProcessesManager()
		{

		}

		ProcessesManager::~ProcessesManager()
		{

		}

		void ProcessesManager::Enum(std::function<bool(std::shared_ptr<Process>)> fun, EnumType type /*= EnumType_CreateToolhelp32Snapshot*/)
		{
			switch (type)
			{
			case daxia::win32::ProcessesManager::EnumType_CreateToolhelp32Snapshot:
				enumByCreateToolhelp32Snapsho(fun);
				break;
			case daxia::win32::ProcessesManager::EnumType_EnumProcesses:
				enumByEnumProcesses(fun);
				break;
			case daxia::win32::ProcessesManager::EnumType_WTSEnumerateProcess:
				enumByWTSEnumerateProcess(fun);
				break;
			case daxia::win32::ProcessesManager::EnumType_NtQuerySystemInfo:
				enumByNtQuerySystemInfo(fun);
				break;
			default:
				break;
			}
		}

		bool ProcessesManager::HasProcess(const daxia::String& name)
		{
			bool has = false;

			HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hSnapshot == INVALID_HANDLE_VALUE) return has;

			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			BOOL bRet = ::Process32First(hSnapshot, &pe);
			while (bRet)
			{
				if (name.CompareNoCase(pe.szExeFile) == 0)
				{
					has = true;
					break;
				}

				bRet = ::Process32Next(hSnapshot, &pe);
			}

			::CloseHandle(hSnapshot);

			return has;
		}

		void ProcessesManager::enumByCreateToolhelp32Snapsho(std::function<bool(std::shared_ptr<Process>)> fun)
		{
			HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hSnapshot == INVALID_HANDLE_VALUE) return;

			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32); 
			BOOL bRet = ::Process32First(hSnapshot, &pe);
			while (bRet)
			{
				auto process = std::shared_ptr<Process>(new Process(pe.th32ProcessID));
				if (*process)
				{
					if (!fun(process))
					{
						break;
					}
				}

				bRet = ::Process32Next(hSnapshot, &pe);
			}

			::CloseHandle(hSnapshot);
		}

		void ProcessesManager::enumByEnumProcesses(std::function<bool(std::shared_ptr<Process>)> fun)
		{

		}

		void ProcessesManager::enumByWTSEnumerateProcess(std::function<bool(std::shared_ptr<Process>)> fun)
		{

		}

		void ProcessesManager::enumByNtQuerySystemInfo(std::function<bool(std::shared_ptr<Process>)> fun)
		{

		}

	}
}

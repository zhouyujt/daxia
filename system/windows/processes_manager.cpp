#ifdef _MSC_VER
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include "processes_manager.h"
#include "process.h"
#include "../../encode/strconv.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			ProcessesManager::ProcessesManager()
			{

			}

			ProcessesManager::~ProcessesManager()
			{

			}

			ProcessesManager::iterator ProcessesManager::begin()
			{
				HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if (hSnapshot == INVALID_HANDLE_VALUE) return end();

				PROCESSENTRY32 pe;
				pe.dwSize = sizeof(PROCESSENTRY32);
				if (::Process32First(hSnapshot, &pe))
				{
					auto handle = std::shared_ptr<void>(hSnapshot, [](void* handle)
					{
						::CloseHandle(handle);
					});
					auto process = std::shared_ptr<Process>(new Process(pe.th32ProcessID));
					auto iter = iterator(handle, process);
					while (!**iter)
					{
						++iter;
					}

					return iter;
				}

				return end();
			}

			ProcessesManager::iterator ProcessesManager::end()
			{
				return iterator();
			}

			ProcessesManager::iterator ProcessesManager::find(const char* name, const ProcessesManager::iterator& pos)
			{
				return find(daxia::string(name).ToUnicode().GetString());
			}

			ProcessesManager::iterator ProcessesManager::find(const wchar_t* name, const ProcessesManager::iterator& pos)
			{
				ProcessesManager::iterator result = end();

				auto iter = pos == end() ? begin() : pos;
				if (pos != end()) ++iter;
				for (; iter != end(); ++iter)
				{
					if (iter->GetName().CompareNoCase(name) == 0)
					{
						result = iter;
						break;
					}
				}

				return result;
			}

			ProcessesManager::iterator ProcessesManager::find(unsigned long pid)
			{
				ProcessesManager::iterator result = end();

				for (auto iter = begin(); iter != end(); ++iter)
				{
					if (iter->GetId() == pid)
					{
						result = iter;
						break;
					}
				}

				return result;
			}

			void ProcessesManager::Enum(std::function<bool(std::shared_ptr<Process>)> fun, EnumType type /*= EnumType_CreateToolhelp32Snapshot*/)
			{
				switch (type)
				{
				case ProcessesManager::EnumType_CreateToolhelp32Snapshot:
					enumByCreateToolhelp32Snapsho(fun);
					break;
				case ProcessesManager::EnumType_EnumProcesses:
					enumByEnumProcesses(fun);
					break;
				case ProcessesManager::EnumType_WTSEnumerateProcess:
					enumByWTSEnumerateProcess(fun);
					break;
				case ProcessesManager::EnumType_NtQuerySystemInfo:
					enumByNtQuerySystemInfo(fun);
					break;
				default:
					break;
				}
			}

			bool ProcessesManager::HasProcess(const daxia::tstring& name)
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

			ProcessesManager::iterator::iterator()
			{

			}

			ProcessesManager::iterator::iterator(std::shared_ptr<void> handle, std::shared_ptr<Process> process)
				: handle_(handle)
				, process_(process)
			{

			}

			ProcessesManager::iterator& ProcessesManager::iterator::operator++()
			{
				PROCESSENTRY32 pe;
				pe.dwSize = sizeof(PROCESSENTRY32);
				if (::Process32Next(handle_.get(), &pe))
				{
					process_ = std::shared_ptr<Process>(new Process(pe.th32ProcessID));
				}
				else
				{
					process_.reset();
				}

				return *this;
			}

			bool ProcessesManager::iterator::operator==(const iterator& iter) const
			{
				if ((!process_ && !iter.process_)
					|| (process_ && iter.process_ && process_->GetId() == iter.process_->GetId()))
				{
					return true;
				}

				return false;
			}

			bool ProcessesManager::iterator::operator!=(const iterator& iter) const
			{
				return !(*this == iter);
			}

			const std::shared_ptr<daxia::system::windows::Process> ProcessesManager::iterator::operator->() const
			{
				return process_;
			}

			const std::shared_ptr<daxia::system::windows::Process> ProcessesManager::iterator::operator*() const
			{
				return process_;
			}

			std::shared_ptr<daxia::system::windows::Process> ProcessesManager::iterator::operator->()
			{
				return process_;
			}

			std::shared_ptr<daxia::system::windows::Process> ProcessesManager::iterator::operator*()
			{
				return process_;
			}

			ProcessesManager::iterator& ProcessesManager::iterator::operator=(const iterator& iter)
			{
				handle_ = iter.handle_;
				process_ = iter.process_;
				return *this;
			}

		}
	}
}
#endif // !_MSC_VER


#ifdef _MSC_VER
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include "find_process.h"
#include "process.h"
#include "../../encode/strconv.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			FindProcess::FindProcess(Type type)
				: type_(type)
			{

			}

			FindProcess::~FindProcess()
			{

			}

			FindProcess::iterator FindProcess::begin()
			{
				if (type_ == CreateToolhelp32Snapshot)
				{
					HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
					if (hSnapshot == INVALID_HANDLE_VALUE) return end();

					PROCESSENTRY32W pe;
					pe.dwSize = sizeof(PROCESSENTRY32W);
					if (::Process32FirstW(hSnapshot, &pe))
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
				}
				else
				{
					// TODO: 完善其他方式
				}

				return end();
			}

			FindProcess::iterator FindProcess::end()
			{
				return iterator();
			}

			daxia::system::windows::FindProcess::iterator FindProcess::find(const char* name, const iterator* pos /*= nullptr*/)
			{
				return find(daxia::string(name).ToUnicode().GetString(), pos);
			}

			daxia::system::windows::FindProcess::iterator FindProcess::find(const wchar_t* name, const iterator* pos /*= nullptr*/)
			{
				FindProcess::iterator result = end();

				auto iter = pos == nullptr ? begin() : *pos;
				if (pos && *pos != end()) ++iter;
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

			FindProcess::iterator FindProcess::find(unsigned long pid)
			{
				FindProcess::iterator result = end();

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

			FindProcess::iterator::iterator()
			{

			}

			FindProcess::iterator::iterator(std::shared_ptr<void> handle, std::shared_ptr<Process> process)
				: handle_(handle)
				, process_(process)
			{

			}

			FindProcess::iterator& FindProcess::iterator::operator++()
			{
				PROCESSENTRY32W pe;
				pe.dwSize = sizeof(PROCESSENTRY32W);
				if (::Process32NextW(handle_.get(), &pe))
				{
					process_ = std::shared_ptr<Process>(new Process(pe.th32ProcessID));
				}
				else
				{
					process_.reset();
				}

				return *this;
			}

			bool FindProcess::iterator::operator==(const iterator& iter) const
			{
				if ((!process_ && !iter.process_)
					|| (process_ && iter.process_ && process_->GetId() == iter.process_->GetId()))
				{
					return true;
				}

				return false;
			}

			bool FindProcess::iterator::operator!=(const iterator& iter) const
			{
				return !(*this == iter);
			}

			const std::shared_ptr<daxia::system::windows::Process> FindProcess::iterator::operator->() const
			{
				return process_;
			}

			const std::shared_ptr<daxia::system::windows::Process> FindProcess::iterator::operator*() const
			{
				return process_;
			}

			std::shared_ptr<daxia::system::windows::Process> FindProcess::iterator::operator->()
			{
				return process_;
			}

			std::shared_ptr<daxia::system::windows::Process> FindProcess::iterator::operator*()
			{
				return process_;
			}

			FindProcess::iterator& FindProcess::iterator::operator=(const iterator& iter)
			{
				handle_ = iter.handle_;
				process_ = iter.process_;
				return *this;
			}

		}
	}
}
#endif // !_MSC_VER


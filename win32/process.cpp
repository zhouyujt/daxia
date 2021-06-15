#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include "process.h"
#include "path.h"

namespace daxia
{
	namespace win32
	{
		Process::Process()
		{
			handle_ = ::GetCurrentProcess();
		}

		Process::Process(unsigned long id)
		{
			handle_ = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);

			// ³¢ÊÔ¿ªÆôÈ¨ÏÞ
			if (handle_ == nullptr)
			{
				Process process;
				auto token = process.GetAccessToken();
				token->EnablePrivilege(SE_DEBUG_NAME, true);
				handle_ = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
			}
		}

		Process::~Process()
		{
			if (handle_ != nullptr)
			{
				::CloseHandle(handle_);
				handle_ = nullptr;
			}
		}

		bool Process::Kill()
		{
			if (::TerminateProcess(handle_, 0))
			{
				::CloseHandle(handle_);
				handle_ = nullptr;

				return true;
			}

			return false;
		}

		unsigned long Process::GetId() const
		{
			return ::GetProcessId(handle_);
		}

		void* Process::GetHandle() const
		{
			return handle_;
		}

		std::wstring Process::GetName() const
		{
			std::wstring name;

			HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetId());
			if (hSnapshot == INVALID_HANDLE_VALUE) return name;

			MODULEENTRY32 me;
			me.dwSize = sizeof(me);
			if (::Module32FirstW(hSnapshot, &me))
			{
				name = me.szModule;
			}

			::CloseHandle(hSnapshot);

			return name;
		}

		std::wstring Process::GetPath() const
		{
			std::wstring path;

			HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetId());
			if (hSnapshot == INVALID_HANDLE_VALUE) return path;

			MODULEENTRY32 me;
			me.dwSize = sizeof(me);
			if (::Module32FirstW(hSnapshot, &me))
			{
				path = me.szExePath;
			}

			::CloseHandle(hSnapshot);

			return path;
		}

		std::wstring Process::GetDirectory() const
		{
			std::wstring dir;
			dir.resize(MAX_PATH);
			auto copied = ::GetCurrentDirectoryW(MAX_PATH, const_cast<wchar_t*>(dir.c_str()));
			dir.resize(copied);

			return dir;
		}

		std::shared_ptr<AccessToken> Process::GetAccessToken()
		{
			if (!token_)
			{
				token_ = std::shared_ptr<AccessToken>(new AccessToken(handle_));
				if (!*token_)
				{
					token_.reset();
				}
			}

			return token_;
		}

		bool Process::RevertToSelf()
		{
			return ::RevertToSelf() != FALSE;
		}

	}
}
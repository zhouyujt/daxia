#include <Windows.h>
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
			std::wstring name = GetPath();
			name = Path::FindFileName(name.c_str());

			return name;
		}

		std::wstring Process::GetPath() const
		{
			std::wstring path;
			path.resize(MAX_PATH);
			auto copied = ::GetModuleFileNameW(FALSE, const_cast<wchar_t*>(path.c_str()), MAX_PATH);
			path.resize(copied);

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
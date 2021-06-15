#include <windows.h>
#include <memory>
#include "access_token.h"
#include "../encode/strconv.h"
namespace daxia
{
	namespace win32
	{
		AccessToken::AccessToken(void* process)
		{
			if (process)
			{
				::OpenProcessToken(process, MAXIMUM_ALLOWED, &token_);
			}
		}

		AccessToken::~AccessToken()
		{
			if (token_)
			{
				::CloseHandle(token_);
				token_ = nullptr;
			}
		}

		bool AccessToken::IsPrivilegeEnabled(const char* privilege) const
		{
			return IsPrivilegeEnabled(daxia::encode::Strconv::Ansi2Unicode(privilege).c_str());
		}

		bool AccessToken::IsPrivilegeEnabled(const wchar_t* privilege) const
		{
			if (token_ == nullptr) return false;

			LUID luid;
			if (!::LookupPrivilegeValueW(NULL, privilege, &luid)) return false;

			PRIVILEGE_SET privs;
			privs.PrivilegeCount = 1;
			privs.Control = PRIVILEGE_SET_ALL_NECESSARY;
			privs.Privilege[0].Luid = luid;
			privs.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;

			BOOL bSet = FALSE;
			::PrivilegeCheck(token_, &privs, &bSet);

			return bSet != FALSE;
		}

		bool AccessToken::EnablePrivilege(const char* privilege, bool enable) const
		{
			return EnablePrivilege(daxia::encode::Strconv::Ansi2Unicode(privilege).c_str(), enable);
		}

		bool AccessToken::EnablePrivilege(const wchar_t* privilege, bool enable) const
		{
			if (token_ == nullptr) return false;

			LUID luid;
			if (!::LookupPrivilegeValueW(NULL, privilege, &luid)) return false;

			TOKEN_PRIVILEGES tp;
			ZeroMemory(&tp, sizeof(tp));
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
			if (!::AdjustTokenPrivileges(token_, FALSE, &tp, 0, NULL, NULL)) return false;

			return true;
		}

		void* AccessToken::GetHandle() const
		{
			return token_;
		}

		std::wstring AccessToken::GetUser() const
		{
			std::wstring user;

			// 获取缓冲区大小
			DWORD copied = 0;
			::GetTokenInformation(token_, TokenUser, NULL, 0, &copied);
			std::shared_ptr<TOKEN_USER> buf(reinterpret_cast<TOKEN_USER*>(new char[copied]));

			// 获取信息
			if (::GetTokenInformation(token_, TokenUser, buf.get(), copied, &copied))
			{
				wchar_t userName[128] = {};
				wchar_t domainName[128] = {};
				DWORD userNameSize = _countof(userName);
				DWORD domainNameSize = _countof(domainName);
				SID_NAME_USE use = SidTypeUser;
				if (::LookupAccountSidW(NULL, buf->User.Sid, userName, &userNameSize, domainName, &domainNameSize, &use))
				{
					user += domainName;
					user += L"\\\\";
					user += userName;
				}
			}

			return user;
		}

		bool AccessToken::Impersonate() const
		{
			if (token_ == nullptr) return false;

			return ::ImpersonateLoggedOnUser(token_) != FALSE;
		}
	}
}

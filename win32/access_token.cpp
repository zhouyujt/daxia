#include <windows.h>
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

		bool AccessToken::Impersonate() const
		{
			if (token_ == nullptr) return false;

			return ::ImpersonateLoggedOnUser(token_) != FALSE;
		}
	}
}

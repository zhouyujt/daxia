#include <Windows.h>
#include <Shlwapi.h>
#include "path.h"

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Shell32.lib")

namespace daxia
{
	namespace win32
	{
		Path::~Path()
		{
		}

		bool Path::CreateDirectory(const wchar_t* dir)
		{
			bool created = false;

			std::wstring path(dir);
			std::wstring root = path.substr(0,3);
			path = path.substr(3, path.npos);

			while (!path.empty())
			{
				auto pos = path.find(L'\\');
				if (pos == path.npos)
				{
					root += path;
					path.clear();
				}
				else
				{
					std::wstring floder = path.substr(0, pos);
					path = path.substr(pos + 1, path.npos);
					root += floder;
				}

				if (!::CreateDirectoryW(root.c_str(), NULL))
				{
					if (::GetLastError() != ERROR_ALREADY_EXISTS)
					{
						break;
					}
				}

				root += L"\\";
			}

			return created;
		}

		std::wstring Path::GetSpecialPath(int csidl/*e.g. CSIDL_APPDATA*/, bool create /*= false*/)
		{
			wchar_t path[MAX_PATH] = {};
			::SHGetSpecialFolderPath(NULL, path, csidl, create ? TRUE : FALSE);
			return std::wstring(path);
		}

		std::wstring Path::FindFileName(const wchar_t* dir)
		{
			return std::wstring(::PathFindFileNameW(dir));
		}

		std::wstring Path::FindExtension(const wchar_t* dir)
		{
			return std::wstring(::PathFindExtensionW(dir));
		}

	}
}
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

		bool Path::CreateDirectory(const daxia::tstring& dir)
		{
			bool created = false;

			daxia::tstring path(dir);
			daxia::tstring root = path.Left(3);
			path = path.Mid(3, -1);

			while (!path.IsEmpty())
			{
				size_t pos = path.Find(L'\\');
				if (pos == -1)
				{
					root += path;
					path.Empty();
				}
				else
				{
					daxia::tstring floder = path.Left(pos);
					path = path.Mid(pos + 1, -1);
					root += floder;
				}

				if (!::CreateDirectory(root.GetString(), NULL))
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

		daxia::tstring Path::GetSpecialPath(int csidl/*АэИзЈє CSIDL_APPDATA*/, bool create /*= false*/)
		{
			daxia::tstring path;
			::SHGetSpecialFolderPath(NULL, path.GetBuffer(MAX_PATH), csidl, create ? TRUE : FALSE);
			path.ReleaseBuffer();

			return path;
		}

		daxia::tstring Path::FindFileName(const daxia::tstring& dir)
		{
			return daxia::tstring(::PathFindFileName(dir.GetString()));
		}

		daxia::tstring Path::FindExtension(const daxia::tstring& dir)
		{
			return daxia::tstring(::PathFindExtension(dir.GetString()));
		}

	}
}
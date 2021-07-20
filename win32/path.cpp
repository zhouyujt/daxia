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

		bool Path::CreateDirectory(const daxia::String& dir)
		{
			bool created = false;

			daxia::String path(dir);
			daxia::String root = path.Left(3);
			path = path.Mid(3, -1);

			while (!path.IsEmpty())
			{
				int pos = path.Find(L'\\');
				if (pos == -1)
				{
					root += path;
					path.Empty();
				}
				else
				{
					daxia::String floder = path.Left(pos);
					path = path.Mid(pos + 1, -1);
					root += floder;
				}

				if (!::CreateDirectory(root, NULL))
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

		daxia::String Path::GetSpecialPath(int csidl/*АэИзЈє CSIDL_APPDATA*/, bool create /*= false*/)
		{
			daxia::String path;
			::SHGetSpecialFolderPath(NULL, path.GetBuffer(MAX_PATH), csidl, create ? TRUE : FALSE);
			path.ReleaseBuffer();

			return path;
		}

		daxia::String Path::FindFileName(const daxia::String& dir)
		{
			return daxia::String(::PathFindFileName(dir));
		}

		daxia::String Path::FindExtension(const daxia::String& dir)
		{
			return daxia::String(::PathFindExtension(dir));
		}

	}
}
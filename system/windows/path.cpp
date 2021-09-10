#ifdef _WIN32
#include <Windows.h>
#include <Shlwapi.h>
#include "path.h"

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Shell32.lib")

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			Path::~Path()
			{
			}

			bool Path::CreateDirectory(const daxia::wstring& dir)
			{
				bool created = false;

				daxia::wstring path(dir);
				daxia::wstring root = path.Left(3);
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
						daxia::wstring floder = path.Left(pos);
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

			daxia::wstring Path::GetSpecialPath(int csidl/*АэИзЈє CSIDL_APPDATA*/, bool create /*= false*/)
			{
				daxia::wstring path;
				::SHGetSpecialFolderPathW(NULL, path.GetBuffer(MAX_PATH), csidl, create ? TRUE : FALSE);
				path.ReleaseBuffer();

				return path;
			}

			daxia::wstring Path::GetTempPath()
			{
				daxia::wstring path;
				::GetTempPathW(MAX_PATH, path.GetBuffer(MAX_PATH));
				path.ReleaseBuffer();

				return path;
			}

			daxia::wstring Path::GetTempFilePath()
			{
				daxia::wstring path;
				::GetTempFileNameW(GetTempPath().GetString(), NULL, 0, path.GetBuffer(MAX_PATH));
				path.ReleaseBuffer();
				
				return path;
			}

			daxia::wstring Path::FindFileName(const daxia::wstring& dir)
			{
				return daxia::wstring(::PathFindFileNameW(dir.GetString()));
			}

			daxia::wstring Path::FindExtension(const daxia::wstring& dir)
			{
				return daxia::wstring(::PathFindExtensionW(dir.GetString()));
			}
		}
	}
}
#endif // !_WIN32

#include <Shlwapi.h>
#include "file.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			File::File(const char* path, Type type /*= file*/)
				: type_(type)
				, size_(0)
			{
				if (path)
				{
					path_ = daxia::string(path).ToUnicode();
				}
				else
				{
					type_ = directory;
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'\\')
				{
					path_.Delete(path_.GetLength() - 1);
				}
			}

			File::File(const wchar_t* path, Type type /*= file*/)
				: type_(type)
				, size_(0)
			{
				if (path)
				{
					path_ = path;
				}
				else
				{
					type_ = directory;
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'\\')
				{
					path_.Delete(path_.GetLength() - 1);
				}
			}

			File::~File()
			{

			}

			bool File::IsExists() const
			{
				return ::PathFileExistsW(path_.GetString());
			}

			size_t File::Size() const
			{
				return size_;
			}

			const daxia::wstring File::Path() const
			{
				return path_;
			}

			daxia::wstring File::Name() const
			{
				return ::PathFindFileNameW(path_.GetString());
			}

			daxia::wstring File::Extension() const
			{
				return ::PathFindExtensionW(path_.GetString());
			}

		}
	}
}
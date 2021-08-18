#include <windows.h>
#include <fileapi.h>
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

			File::iterator::iterator()
			{

			}

			File::iterator::~iterator()
			{

			}

			File::iterator::iterator(std::shared_ptr<void> handle, std::shared_ptr<File> file)
				: handle_(handle)
				, file_(file)
			{

			}

			File::iterator& File::iterator::operator++()
			{
				if (!file_) return *this;

				if (handle_ == nullptr)
				{
					// 遍历盘符
					DWORD size = ::GetLogicalDriveStringsW(0, NULL);
					daxia::wstring drives;
					::GetLogicalDriveStringsW(size, drives.GetBuffer(size));
					
					// 排除已经遍历过的盘符
					std::vector<daxia::wstring> strings;
					drives.Split(L"\0", 1, strings);
					int index = -1;
					for (size_t i = 0; i < strings.size(); ++i)
					{
						if (file_->Path() + L'\\' == strings[i])
						{
							index = i + 1;
							break;
						}
					}

					if (index >= strings.size())
					{
						file_.reset();
					}
					else
					{
						file_ = std::shared_ptr<File>(new File(strings[index].GetString(), Type::directory));
					}
				}
				else
				{
					WIN32_FIND_DATAW data;
					if (::FindNextFileW(handle_.get(), &data))
					{
						daxia::wstring path = file_->Path();
						path.Replace(file_->Name().GetString(), L"");
						path += data.cFileName;
						file_ = std::shared_ptr<File>(new File(path.GetString(), data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? Type::directory : Type::file));
					}
					else
					{
						file_.reset();
					}
					
				}

				return *this;
			}

			const std::shared_ptr<daxia::system::windows::File> File::iterator::operator->() const
			{
				return file_;
			}

			std::shared_ptr<daxia::system::windows::File> File::iterator::operator->()
			{
				return file_;
			}

			const std::shared_ptr<daxia::system::windows::File> File::iterator::operator*() const
			{
				return file_;
			}

			std::shared_ptr<daxia::system::windows::File> File::iterator::operator*()
			{
				return file_;
			}

			File::iterator& File::iterator::operator=(const iterator& iter)
			{
				handle_ = iter.handle_;
				file_ = iter.file_;

				return *this;
			}

			bool File::iterator::operator==(const iterator& iter) const
			{
				if ((!file_ && !iter.file_)
					|| file_ && iter.file_ && file_->path_ == iter.file_->path_ )
				{
					return true;
				}

				return false;
			}

			bool File::iterator::operator!=(const iterator& iter) const
			{
				return !(*this == iter);
			}

			File::iterator File::begin()
			{
				if (type_ != directory) return end();
				WIN32_FIND_DATAW data;

				if (path_.IsEmpty())
				{
					// 遍历盘符
					DWORD size = ::GetLogicalDriveStringsW(0, NULL);
					daxia::wstring drives;
					::GetLogicalDriveStringsW(size, drives.GetBuffer(size));
					auto file = std::shared_ptr<File>(new File(drives.GetString(), Type::directory));
					return  File::iterator(nullptr, file);
				}
				else
				{
					daxia::wstring path = path_ + L"\\*.*";
					HANDLE handle = ::FindFirstFileW(path.GetString(), &data);
					if (INVALID_HANDLE_VALUE == handle)   return end();

					// 过滤当前文件夹以及上级文件夹
					while (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (wcscmp(data.cFileName, L".") == 0 || wcscmp(data.cFileName, L"..") == 0)
						{
							if (!FindNextFileW(handle, &data))  return end();
						}
						else
						{
							break;
						}
					}

					auto h = std::shared_ptr<void>(handle, [](void* handle)
					{
						::FindClose(handle);
					});
					auto file = std::shared_ptr<File>(new File((path_ + L"\\" + data.cFileName).GetString(), data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? Type::directory : Type::file));
					return  File::iterator(h, file);
				}
			}

			File::iterator File::end()
			{
				return File::iterator();
			}

			File::iterator File::find(const char* name)
			{
				throw 1;
			}

			File::iterator File::find(const wchar_t* name)
			{
				throw 1;
			}

			const daxia::wstring File::Path() const
			{
				return path_;
			}

			daxia::wstring File::Name()
			{
				return ::PathFindFileNameW(path_.GetString());
			}

			daxia::wstring File::Extension()
			{
				return ::PathFindExtension(path_.GetString());
			}

		}
	}
}
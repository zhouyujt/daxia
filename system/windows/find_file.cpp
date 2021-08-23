#ifdef _MSC_VER
#include <windows.h>
#include <fileapi.h>
#include <Shlwapi.h>
#include "find_file.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			FindFile::FindFile(const char* path, const char* name /*= "*.*"*/, bool recursion /*= false*/)
			{
				if (path)
				{
					path_ = daxia::string(path).ToUnicode();
				}

				if (name)
				{
					name_ = daxia::string(name).ToUnicode();
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'\\')
				{
					path_.Delete(path_.GetLength() - 1);
				}
			}

			FindFile::FindFile(const wchar_t* path, const wchar_t* name /*= L"*.*"*/, bool recursion /*= false*/)
			{
				if (path)
				{
					path_ = path;
				}
				
				if (name)
				{
					name_ = name;
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'\\')
				{
					path_.Delete(path_.GetLength() - 1);
				}
			}

			FindFile::~FindFile()
			{

			}

			FindFile::iterator::iterator()
			{

			}

			FindFile::iterator::~iterator()
			{

			}

			FindFile::iterator::iterator(std::shared_ptr<void> handle, std::shared_ptr<File> file)
				: handle_(handle)
				, file_(file)
			{

			}

			FindFile::iterator& FindFile::iterator::operator++()
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
						file_ = std::shared_ptr<File>(new File(strings[index].GetString(), File::directory));
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
						file_ = std::shared_ptr<File>(new File(path.GetString(), data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? File::directory : File::file));
						file_->size_ = (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow;
					}
					else
					{
						file_.reset();
					}
					
				}

				return *this;
			}

			const std::shared_ptr<daxia::system::windows::File> FindFile::iterator::operator->() const
			{
				return file_;
			}

			std::shared_ptr<daxia::system::windows::File> FindFile::iterator::operator->()
			{
				return file_;
			}

			const std::shared_ptr<daxia::system::windows::File> FindFile::iterator::operator*() const
			{
				return file_;
			}

			std::shared_ptr<daxia::system::windows::File> FindFile::iterator::operator*()
			{
				return file_;
			}

			FindFile::iterator& FindFile::iterator::operator=(const iterator& iter)
			{
				handle_ = iter.handle_;
				file_ = iter.file_;

				return *this;
			}

			bool FindFile::iterator::operator==(const iterator& iter) const
			{
				if ((!file_ && !iter.file_)
					|| file_ && iter.file_ && file_->Path() == iter.file_->Path() )
				{
					return true;
				}

				return false;
			}

			bool FindFile::iterator::operator!=(const iterator& iter) const
			{
				return !(*this == iter);
			}

			FindFile::iterator FindFile::begin()
			{
				WIN32_FIND_DATAW data;

				if (path_.IsEmpty())
				{
					// 遍历盘符
					DWORD size = ::GetLogicalDriveStringsW(0, NULL);
					daxia::wstring drives;
					::GetLogicalDriveStringsW(size, drives.GetBuffer(size));
					auto file = std::shared_ptr<File>(new File(drives.GetString(), File::directory));
					return  FindFile::iterator(nullptr, file);
				}
				else
				{
					daxia::wstring path = path_ + L"\\" +  name_;
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
					auto file = std::shared_ptr<File>(new File((path_ + L"\\" + data.cFileName).GetString(), data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? File::directory : File::file));
					file->size_ = (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow;
					return  FindFile::iterator(h, file);
				}
			}

			FindFile::iterator FindFile::end()
			{
				return FindFile::iterator();
			}

			FindFile::iterator FindFile::find(const char* name)
			{
				throw 1;
			}

			FindFile::iterator FindFile::find(const wchar_t* name)
			{
				throw 1;
			}
		}
	}
}
#endif // !_MSC_VER
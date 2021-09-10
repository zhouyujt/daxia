#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "find_file.h"
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			FindFile::FindFile(const char* path, const char* name /*= "*.*"*/, bool recursion /*= false*/)
			{
				if (path)
				{
					path_ = path;
				}
				else
				{
					path_ = "/";
				}


				if (name)
				{
					name_ = name;
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'/')
				{
					path_.Delete(path_.GetLength() - 1);
				}
			}

			FindFile::FindFile(const wchar_t* path, const wchar_t* name /*= L"*.*"*/, bool recursion /*= false*/)
			{
				if (path)
				{
					path_ = daxia::wstring(path).ToAnsi();
				}
				
				if (name)
				{
					name_ = daxia::wstring(name).ToAnsi();
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'/')
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

			FindFile::iterator::iterator(std::shared_ptr<DIR> handle, std::shared_ptr<File> file)
				: handle_(handle)
				, file_(file)
			{

			}

			FindFile::iterator& FindFile::iterator::operator++()
			{
				if (!file_) return *this;
				dirent* ent = readdir(handle_.get());
				if (ent)
				{
					daxia::string path = file_->Path();
					path.Replace(file_->Name().GetString(), "");
					path += ent->d_name;

					struct stat statbuf;
					stat(ent->d_name, &statbuf);
					file_ = std::shared_ptr<File>(new File(path.GetString(), S_ISDIR(statbuf.st_mode) ? File::directory : File::file));
					file->size_ = statbuf.st_size;
					//file->createTime_ = statbuf.st_ctime;
					file->accessTime_ = statbuf.st_atime;
					file->writeTime_ = statbuf.st_mtime;
				}
				else
				{
					file_.reset();
				}

				return *this;
			}

			const std::shared_ptr<daxia::system::linux::File> FindFile::iterator::operator->() const
			{
				return file_;
			}

			std::shared_ptr<daxia::system::linux::File> FindFile::iterator::operator->()
			{
				return file_;
			}

			const std::shared_ptr<daxia::system::linux::File> FindFile::iterator::operator*() const
			{
				return file_;
			}

			std::shared_ptr<daxia::system::linux::File> FindFile::iterator::operator*()
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
				DIR* handle;
				dirent* ent;

				handle = opendir(path_.GetString());
				if (handle == nullptr) return end();

				// 过滤当前文件夹以及上级文件夹
				while ((ent = readdir(handle)) != nullptr)
				{
					if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
					{
						continue;
					}
					else
					{
						break;
					}
				}

				auto h = std::shared_ptr<DIR>(handle, [](DIR* handle)
				{
					closedir(handle);
				});
				struct stat statbuf;
				stat(ent->d_name, &statbuf);
				auto file = std::shared_ptr<File>(new File((path_ + "/" + ent->d_name).GetString(), S_ISDIR(statbuf.st_mode) ? File::directory : File::file));
				file->size_ = statbuf.st_size;
				//file->createTime_ = statbuf.st_ctime;
				file->accessTime_ = statbuf.st_atime;
				file->writeTime_ = statbuf.st_mtime;
				return  FindFile::iterator(h, file);

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
#endif // !__linux__
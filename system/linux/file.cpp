#ifdef __linux__
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include "file.h"
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			File::File(const char* path, Type type /*= file*/)
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

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == '/')
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
					path_ = daxia::wstring(path).ToAnsi();
				}
				else
				{
					type_ = directory;
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == '/')
				{
					path_.Delete(path_.GetLength() - 1);
				}
			}

			File::~File()
			{

			}

			bool File::IsExists() const
			{
				if (type_ == file)
				{
					return isFileExists();
				}
				else
				{
					return isDirectoryExists();
				}
			}

			File::Type File::FileType() const
			{
				return type_;
			}

			size_t File::Size() const
			{
				return size_;
			}

			const daxia::string File::Path() const
			{
				return path_;
			}

			daxia::string File::Name() const
			{
				return path_;
			}

			daxia::string File::Extension() const
			{
				return path_;
			}

			const daxia::system::DateTime& File::CreateTime() const
			{
				return createTime_;
			}

			const daxia::system::DateTime& File::AccessTime() const
			{
				return accessTime_;
			}

			const daxia::system::DateTime& File::WriteTime() const
			{
				return writeTime_;
			}

			bool File::Move(const char* path) const
			{
				return !rename(path_.GetString(), path);
			}

			bool File::Move(const wchar_t* path) const
			{
				return Move(daxia::wstring(path).ToAnsi().GetString());
			}

			bool File::Copy(const char* path) const
			{
				if (!IsExists())
				{
					return false;
				}

				int fin = open(path_.GetString(), O_RDONLY, 0777);
				int fout = open(path, O_WRONLY | O_CREAT, 0777);

				char buff[1024];
				int len = 0;
				while ((len = read(fin, buff, sizeof(buff))) > 0)
				{
					write(fout, buff, len);
				}

				close(fin);
				close(fout);
			}

			bool File::Copy(const wchar_t* path) const
			{
				return Copy(daxia::wstring(path).ToAnsi().GetString());
			}

			bool File::Create() const
			{
				if (!IsExists())
				{
					if (type_ == directory)
					{
						bool created = true;

						daxia::string path(path_);
						daxia::string root = "/";

						while (!path.IsEmpty())
						{
							size_t pos = path.Find(L'/');
							if (pos == -1)
							{
								root += path;
								path.Empty();
							}
							else
							{
								daxia::string floder = path.Left(pos);
								path = path.Mid(pos + 1, -1);
								root += floder;
							}

							int ret;
							if (mkdir(root.GetString()) != 0)
							{
								if (errno != EEXIST)
								{
									created = false;
									break;
								}
							}

							root += "/";
						}

						return created;
					}
					else
					{
						std::ofstream ofs;
						ofs.open(path_);
						if (ofs.is_open())
						{
							ofs.close();
							return true;
						}

						return false;
					}
				}

				return false;
			}

			bool File::Delete() const
			{
				return !remove(path_.GetString());
			}

			bool File::Read(daxia::buffer& buffer, size_t pos /*= 0*/, size_t len /*= -1*/)
			{
				bool result = false;

				std::ifstream ifs;
				ifs.open(path_, std::ios::binary);
				if (ifs.is_open())
				{
					ifs.seekg(0, ifs.end);
					size_t datalen = static_cast<size_t>(ifs.tellg());
					ifs.seekg(0, ifs.beg);

					len = len < datalen - pos ? len : datalen - pos;

					ifs.seekg(pos, ifs.beg);
					if (ifs.read(buffer.GetBuffer(len), len))
					{
						result = true;
					}

					ifs.close();
				}

				return result;
			}

			bool File::Write(const daxia::buffer& buffer, bool truncate /*= true*/)
			{
				bool result = false;

				std::ofstream ofs;
				if (truncate)
				{
					ofs.open(path_, std::ios::binary | std::ios::trunc);
				}
				else
				{
					ofs.open(path_, std::ios::binary);
				}
				if (ofs.is_open())
				{
					if (ofs.write(buffer.GetString(), buffer.GetLength()))
					{
						result = true;
					}

					ofs.close();
				}

				return result;
			}

			bool File::isDirectoryExists() const
			{
				DIR* dir;
				dir = opendir(path_.GetString());
				if (dir == nullptr)
				{
					return false;
				}
				else
				{
					closedir(dir);
					return true;
				}
			}

			bool File::isFileExists() const
			{
				return !access(path_.GetString(), F_OK);
			}

		}
	}
}
#endif // !__linux__
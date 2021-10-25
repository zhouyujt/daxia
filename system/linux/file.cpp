#ifdef __linux__
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include "file.h"
#include "find_file.h"
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			File::File(const char* path)
				: size_(0)
			{
				if (path)
				{
					daxia::string newpath(path);
					newpath.Replace("\\","/");
					while (!newpath.IsEmpty() && newpath[newpath.GetLength() - 1] == '/')
					{
						newpath.Delete(newpath.GetLength() - 1);
					}

					struct stat statbuf;
					if(lstat(path,&statbuf) == 0)
					{
						path_ = std::move(newpath);

						type_ = S_ISDIR(statbuf.st_mode) ? File::directory : File::file;
						size_ = statbuf.st_size;
						//createTime_ = statbuf.st_ctime;
						accessTime_ = statbuf.st_atime;
						writeTime_ = statbuf.st_mtime;
					}
				}
			}

			File::File(const char* path, Type type)
				: type_(type)
				, size_(0)
			{
				if (path)
				{
					path_ = path;
				}

				while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == '/')
				{
					path_.Delete(path_.GetLength() - 1);
				}
			}

			File::~File()
			{

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
				daxia::string name;
				size_t pos = path_.Rfind("/");
				if (pos != (size_t)-1)
				{
					name = path_.Mid(pos + 1,-1);
				}

				return name;
			}

			daxia::string File::Extension() const
			{
				daxia::string ext;
				daxia::string name = Name();
				size_t pos = name.Rfind(".");
				if (pos != (size_t)-1)
				{
					ext = name.Mid(pos,-1);
				}

				return ext;
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

			bool File::Copy(const char* path) const
			{
				if (!IsExists(path))
				{
					return false;
				}

				int fin = open(path_.GetString(), O_RDONLY, 0777);
				int fout = open(path, O_WRONLY | O_CREAT, 0777);

				char buff[1024];
				size_t len = 0;
				while ((len = read(fin, buff, sizeof(buff))) > 0)
				{
					write(fout, buff, len);
				}

				close(fin);
				close(fout);

				return true;
			}

			bool File::Delete() const
			{
				if (path_.IsEmpty()) return false;

				if (type_ == file)
				{
					return !remove(path_.GetString());
				}
				else
				{
					// µÝ¹éÉ¾³ý
					FindFile finder(path_.GetString());
					for (auto iter = finder.begin(); iter != finder.end(); ++ iter)
					{
						iter->Delete();
					}
					return !remove(path_.GetString());
				}
			}

			bool File::IsExists(const char* path)
			{
				DIR* dir;
				dir = opendir(path);
				if (dir == nullptr)
				{
					return !access(path, F_OK);
				}
				else
				{
					closedir(dir);
					return true;
				}
			}

			File File::Create(const char* p, Type type)
			{
				if (!IsExists(p))
				{
					daxia::string path(p);
					path.Replace("\\","/");

					if (type == directory)
					{
						daxia::string root;

						while (!path.IsEmpty())
						{
							size_t pos = path.Find(L'/');
							if (pos == (size_t)-1)
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

							if (mkdir(root.GetString(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
							{
								if (errno != EEXIST)
								{
									break;
								}
							}

							root += "/";
						}

						return File(path.GetString());
					}
					else
					{
						std::ofstream ofs;
						ofs.open(p);
						if (ofs.is_open())
						{
							ofs.close();
							return File(path.GetString());
						}

						return File("");
					}
				}

				return File("");
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
					if (!ifs.read(buffer.GetBuffer(len), len))
					{
						buffer.ReSize(ifs.gcount());
					}

					result = true;
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
					ofs.open(path_, std::ios::binary | std::ios::app);
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
		}
	}
}
#endif // !__linux__
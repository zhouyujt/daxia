#ifdef _WIN32
#include <fstream>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
#include "file.h"
#include "find_file.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			File::File(const char* path)
				: size_(0)
			{
				if (path)
				{
					init(daxia::string(path).ToUnicode().GetString());
				}
			}

			File::File(const wchar_t* path)
				: size_(0)
			{
				if (path)
				{
					init(path);
				}
			}

			File::File(const char* path, Type type)
				: type_(type)
				, size_(0)
			{
				if (path)
				{
					path_ = daxia::string(path).ToUnicode();

					while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'\\')
					{
						path_.Delete(path_.GetLength() - 1);
					}
				}


			}

			File::File(const wchar_t* path, Type type)
				: type_(type)
				, size_(0)
			{
				if (path)
				{
					path_ = path;

					while (!path_.IsEmpty() && path_[path_.GetLength() - 1] == L'\\')
					{
						path_.Delete(path_.GetLength() - 1);
					}
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
				return Move(daxia::string(path).ToUnicode().GetString());
			}

			bool File::Move(const wchar_t* path) const
			{
				return ::MoveFile(path_.GetString(), path);
			}

			bool File::Copy(const char* path) const
			{
				return Copy(daxia::string(path).ToUnicode().GetString());
			}

			bool File::Copy(const wchar_t* path) const
			{
				return ::CopyFile(path_.GetString(), path, FALSE);
			}

			bool File::Delete() const
			{
				if (type_ == file)
				{
					return ::DeleteFileW(path_.GetString()) != 0;
				}
				else
				{
					// µÝ¹éÉ¾³ý
					FindFile finder(path_.GetString());
					for (auto iter = finder.begin(); iter != finder.end(); ++ iter)
					{
						iter->Delete();
					}
					return ::RemoveDirectory(path_.GetString()) != 0;
				}
			}

			bool File::IsExists(const char* path)
			{
				return ::PathFileExistsA(path);
			}

			bool File::IsExists(const wchar_t* path)
			{
				return ::PathFileExistsW(path);
			}

			File File::Create(const char* p, Type type)
			{
				return Create(daxia::string(p).ToUnicode().GetString(), type);
			}

			File File::Create(const wchar_t* p, Type type)
			{
				if (!IsExists(p))
				{
					daxia::wstring path(p);
					path.Replace(L'/', L'\\');

					if (type == directory)
					{
						daxia::wstring root;

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

						return File(path.GetString());
					}
					else
					{
						std::ofstream ofs;
#ifdef _MSC_VER
						ofs.open(path.GetString());
#else
						ofs.open(path.ToAnsi().GetString());
#endif
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
				HANDLE file = ::CreateFileW(path_.GetString(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file == INVALID_HANDLE_VALUE) return false;

				if (len == -1)
				{
					LARGE_INTEGER size;
					GetFileSizeEx(file, &size);
					len = static_cast<size_t>(size.QuadPart);
				}
			
				if (pos != 0)
				{
					LARGE_INTEGER temp;
					temp.QuadPart = pos;
					::SetFilePointerEx(file, temp, &temp, FILE_BEGIN);
				}

				DWORD dwRead = 0;
				if (::ReadFile(file, buffer.GetBuffer(len), len, &dwRead, NULL))
				{
					buffer.ReSize(dwRead);
					result = true;
				}

				::CloseHandle(file);
				return result;
			}

			bool File::Write(const daxia::buffer& buffer, bool truncate /*= true*/)
			{
				bool result = false;
				DWORD creationDisposition = truncate ? CREATE_ALWAYS : OPEN_ALWAYS;
				HANDLE file = ::CreateFileW(path_.GetString(), FILE_APPEND_DATA, 0, NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file == INVALID_HANDLE_VALUE) return false;

				DWORD dwWrite = 0;
				if (::WriteFile(file,buffer.GetString(),buffer.GetLength(),&dwWrite,NULL))
				{
					result = true;
				}

				::CloseHandle(file);
				return result;
			}

			void File::init(const wchar_t* path)
			{
				daxia::wstring newpath(path);
				newpath.Replace(L'/', L'\\');
				while (!newpath.IsEmpty() && newpath[newpath.GetLength() - 1] == L'\\')
				{
					newpath.Delete(newpath.GetLength() - 1);
				}

				WIN32_FILE_ATTRIBUTE_DATA data;
				BOOL ret = ::GetFileAttributesExW(newpath.GetString(), GetFileExInfoStandard, &data);

				if (ret != 0)
				{
					path_ = std::move(newpath);

					type_ = data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY ? File::directory : type_ = File::file;
					size_ = (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow;
					createTime_ = data.ftCreationTime;
					accessTime_ = data.ftLastAccessTime;
					writeTime_ = data.ftLastWriteTime;
				}
			}
		}
	}
}
#endif // !_WIN32
#ifdef _MSC_VER
#include <fstream>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
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

			bool File::Create() const
			{
				if (!IsExists())
				{
					if (type_ == directory)
					{
						bool created = false;

						daxia::wstring path(path_);
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
				return ::DeleteFileW(path_.GetString()) != 0;
			}

		}
	}
}
#endif // !_MSC_VER
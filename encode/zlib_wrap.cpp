#include "zlib_wrap.h"
#include "zlib/zlib/contrib/minizip/zip.h"
#include "zlib/zlib/contrib/minizip/unzip.h"
#include "../system/file.h"
#include "../system/find_file.h"
#include "../net/common/byte_order.hpp"

namespace daxia
{
	namespace encode
	{
		using daxia::system::File;

		Zlib::Zlib()
		{

		}

		Zlib::~Zlib()
		{

		}

		daxia::buffer Zlib::Marshal(const void* data, size_t size)
		{
			daxia::buffer buffer;
			if (data == nullptr || size == 0) return buffer;

			uLongf len = 0;
			if (Z_OK == compress(reinterpret_cast<Bytef*>(buffer.GetBuffer(size)), &len, reinterpret_cast<const Bytef*>(data), size))
			{
				buffer.ReSize(len);

				// 附加源数据长度到末尾
				size_t s = daxia::net::common::ByteOrder::hton(size);
				buffer.Append(reinterpret_cast<char*>(&s), sizeof(s));
			}
			else
			{
				buffer.Empty();
			}

			return buffer;
		}

		daxia::buffer Zlib::Marshal(const daxia::string& str)
		{
			return Marshal(str.GetString(), str.GetLength());
		}

		bool Zlib::MarshalFile(const daxia::string& source, const daxia::string& destination)
		{
			using daxia::system::File;

			if (File::IsExists(source.GetString()))
			{
				File f(source.GetString());
				daxia::string root;
				if (f.FileType() == File::directory)
				{
					size_t pos = source.Rfind('\\');
					if (pos != -1)
					{
						root = source.Mid(pos + 1, -1);
					}
					else
					{
						root = source;
					}
				}

				return marshalFile(source, destination, root, true);
			}
			else
			{
				return false;
			}
		}

		daxia::buffer Zlib::Unmarshal(const void* data, size_t size)
		{
			size_t oldsize = *reinterpret_cast<const size_t*>(static_cast<const char*>(data)+(size - sizeof(size_t)));
			oldsize = daxia::net::common::ByteOrder::ntoh(oldsize);
			daxia::buffer buffer;
			uLongf len = oldsize;
			if (Z_OK != uncompress(reinterpret_cast<Bytef*>(buffer.GetBuffer(oldsize)), &len, reinterpret_cast<const Bytef*>(data), size))
			{
				buffer.Empty();
			}
			
			return buffer;
		}

		daxia::buffer Zlib::Unmarshal(const daxia::string& str)
		{
			return Unmarshal(str.GetString(), str.GetLength());
		}

		bool Zlib::UnmarshalFile(const daxia::string& source, const daxia::string& destination)
		{
			using daxia::system::File;

			bool result = false;

			unzFile uf = unzOpen(source.GetString());
			if (uf == nullptr) return result;

			unz_global_info gi = { 0 };
			if (unzGetGlobalInfo(uf, &gi) == UNZ_OK)
			{
				for (int i = 0; i < gi.number_entry; ++i)
				{
					// 获取当前文件信息
					unz_file_info fi = { 0 };
					daxia::buffer filename;
					if (unzGetCurrentFileInfo(uf, &fi, filename.GetBuffer(256), 256, nullptr, 0, nullptr, 0) != UNZ_OK) break;
					filename.ReleaseBuffer();
					if (!File::IsExists(filename.GetString()))
					{
						File::Create(filename.GetString(), File::file);
					}

					// 读取当前文件
					if (unzOpenCurrentFile(uf) != UNZ_OK) break;

					unsigned int size = 1024 * 1024;
					int copied = 0;
					daxia::buffer data;
					while ((copied = unzReadCurrentFile(uf, data.GetBuffer(size), size) > 0))
					{
						data.ReSize(copied);
						File f(filename.GetString());
						f.Write(data);
					}
					
					unzCloseCurrentFile(uf);
					unzGoToNextFile(uf);
				}
			}

			unzClose(uf);

			return result;
		}

		bool Zlib::marshalFile(const daxia::string& source, const daxia::string& destination, const daxia::string& root, bool trunc)
		{
			using daxia::system::File;
			using daxia::system::FindFile;

			File f(source.GetString());
			if (f.FileType() == File::file)
			{
				zipFile zfile = nullptr;
				if (trunc || !File::IsExists(destination.GetString()))
				{
					zfile = zipOpen(destination.GetString(), APPEND_STATUS_CREATE);
				}
				else
				{
					zfile = zipOpen(destination.GetString(), APPEND_STATUS_ADDINZIP);
				}

				if (zfile == nullptr) return false;

				zip_fileinfo fileInfo = { 0 };
				if (zipOpenNewFileInZip(zfile, (root + "\\" + f.Name().ToAnsi()).GetString(), &fileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) == Z_OK)
				{

					size_t copied = 0;
					const size_t maxsize = 1024 * 1024 * 16;
					daxia::buffer data;
					while (copied < f.Size())
					{
						f.Read(data, copied, maxsize);
						if (!data.IsEmpty())
						{
							zipWriteInFileInZip(zfile, data.GetString(), data.GetLength());
						}
						else
						{
							break;
						}

						copied += data.GetLength();
					}

					zipCloseFileInZip(zfile);
				}

				zipClose(zfile, nullptr);
			}
			else
			{
				FindFile finder(source.GetString());
				for (auto iter = finder.begin(); iter != finder.end(); ++iter)
				{
					if (iter->FileType() == File::file)
					{
						marshalFile(iter->Path().ToAnsi().GetString(), destination, root, false);
					}
					else
					{
						marshalFile(iter->Path().ToAnsi().GetString(), destination, root + "\\" + iter->Name().ToAnsi(), false);
					}
				}
			}

			return true;
		}
	}
}
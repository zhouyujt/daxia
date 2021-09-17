#include "zlib/zlib/zlib.h"
#include "zlib.h"
#include "../system/file.h"
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
			if (data == nullptr || size == 0) return;

			daxia::buffer buffer;
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

		void Zlib::MarshalFile(const daxia::string& filename, const daxia::string& destination)
		{

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

		daxia::buffer Zlib::UnmarshalFile(const daxia::string& filename, const daxia::string& destination)
		{

		}

	}
}
#include <iostream>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include "md5.h"

namespace daxia
{
	namespace encode
	{

		Md5::Md5()
		{
			md5_ = std::shared_ptr<boost::uuids::detail::md5>(new boost::uuids::detail::md5);
		}

		daxia::string Md5::Update(const char* data, size_t size)
		{
			return update(data, size, md5_);
		}

		daxia::string Md5::Update(const std::string& data)
		{
			return update(data.c_str(), data.size(), md5_);
		}

		daxia::string Md5::Marshal(const char* data, size_t size)
		{
			return update(data, size, std::shared_ptr<boost::uuids::detail::md5>(new boost::uuids::detail::md5));
		}

		daxia::string Md5::Marshal(const std::string& data)
		{
			return update(data.c_str(), data.size(), std::shared_ptr<boost::uuids::detail::md5>(new boost::uuids::detail::md5));
		}

		daxia::string Md5::update(const char* data, size_t size, std::shared_ptr<boost::uuids::detail::md5> md5)
		{
			daxia::string ret;
			if (data == nullptr) return ret;
			md5->process_bytes(data, size);
			boost::uuids::detail::md5::digest_type digest;
			md5->get_digest(digest);

			// ×Ö½ÚÐòµ÷Õû
			boost::uuids::detail::md5::digest_type order;
			memcpy(&order, &digest, sizeof(order));
			for (size_t i = 0; i < sizeof(digest) / sizeof(int); ++i)
			{
				for (size_t j = 0; j < sizeof(int); ++j)
				{
					reinterpret_cast<char*>(&order[i])[j] = reinterpret_cast<char*>(&digest[i])[sizeof(int) - j - 1];
				}
			}

			const auto char_digest = reinterpret_cast<const char*>(&order);

			boost::algorithm::hex_lower(char_digest, char_digest + sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(const_cast<std::string&>(ret.operator const std::string&())));

			return ret;
		}

	}// namespace encode
}// namespace daxia

#include <iostream>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include "sha1.h"

namespace daxia
{
	namespace encode
	{

		Sha1::Sha1()
		{
			sha1_ = std::shared_ptr<boost::uuids::detail::sha1>(new boost::uuids::detail::sha1);
		}

		daxia::string Sha1::Update(const char* data, size_t size)
		{
			return update(data, size, sha1_);
		}

		daxia::string Sha1::Update(const std::string& data)
		{
			return update(data.c_str(), data.size(), sha1_);
		}

		daxia::string Sha1::Marshal(const void* data, size_t size)
		{
			return update(data, size, std::shared_ptr<boost::uuids::detail::sha1>(new boost::uuids::detail::sha1));
		}

		daxia::string Sha1::Marshal(const std::string& data)
		{
			return update(data.c_str(), data.size(), std::shared_ptr<boost::uuids::detail::sha1>(new boost::uuids::detail::sha1));
		}

		daxia::string Sha1::update(const void* data, size_t size, std::shared_ptr<boost::uuids::detail::sha1> sha1)
		{
			daxia::string ret;
			if (data == nullptr) return ret;
			sha1->process_bytes(data, size);
			boost::uuids::detail::sha1::digest_type digest;
			sha1->get_digest(digest);

			// ×Ö½ÚÐòµ÷Õû
			boost::uuids::detail::sha1::digest_type order;
			memcpy(&order, &digest, sizeof(order));
			for (size_t i = 0; i < sizeof(digest) / sizeof(int); ++i)
			{
				for (size_t j = 0; j < sizeof(int); ++j)
				{
					reinterpret_cast<char*>(&order[i])[j] = reinterpret_cast<char*>(&digest[i])[sizeof(int) - j - 1];
				}
			}

			const auto char_digest = reinterpret_cast<const char*>(&order);

			boost::algorithm::hex_lower(char_digest, char_digest + sizeof(boost::uuids::detail::sha1::digest_type), std::back_inserter(const_cast<std::string&>(ret.operator const std::string&())));

			return ret;
		}

	}// namespace encode
}// namespace daxia

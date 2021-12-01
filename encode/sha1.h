/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file sha1.h
 * \author 漓江里的大虾
 * \date 十二月 2021
 *
 * 封装boost库的sha1编码
 *
 */
#ifndef __DAXIA_ENCODE_MD5_H
#define __DAXIA_ENCODE_MD5_H
#include <string>
#include <memory>
#include "../string.hpp"

namespace boost
{
	namespace uuids
	{
		namespace detail
		{
			class sha1;
		}
	}
}

namespace daxia
{
	namespace encode
	{
		class Sha1
		{
		public:
			Sha1();
			~Sha1(){};
		public:
			daxia::string Update(const char* data, size_t size);
			daxia::string Update(const std::string& data);
		public:
			static daxia::string Marshal(const void* data, size_t size);
			static daxia::string Marshal(const std::string& data);
		private:
			static daxia::string update(const void* data, size_t size, std::shared_ptr<boost::uuids::detail::sha1> sha1);
			std::shared_ptr<boost::uuids::detail::sha1> sha1_;
		};// class Sha1
	}// namespace encode
}// namespace daxia

#endif // !__DAXIA_ENCODE_MD5_H

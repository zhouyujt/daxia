/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file base64.h
 * \author 漓江里的大虾
 * \date 七月 2021
 *
 * 封装boost库的md5编码
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
			class md5;
		}
	}
}

namespace daxia
{
	namespace encode
	{
		class Md5
		{
		public:
			Md5();
			~Md5(){};
		public:
			daxia::string Update(const char* data, size_t size);
			daxia::string Update(const std::string& data);
		public:
			static daxia::string Marshal(const void* data, size_t size);
			static daxia::string Marshal(const std::string& data);
		private:
			static daxia::string update(const void* data, size_t size, std::shared_ptr<boost::uuids::detail::md5> md5);
			std::shared_ptr<boost::uuids::detail::md5> md5_;
		};// class Base64
	}// namespace encode
}// namespace daxia

#endif // !__DAXIA_ENCODE_MD5_H

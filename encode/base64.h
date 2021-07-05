/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file base64.h
 * \author 漓江里的大虾
 * \date 七月 2021
 *
 * BASE64的编解码
 *
 */
#ifndef __DAXIA_ENCODE_BASE64_H
#define __DAXIA_ENCODE_BASE64_H

#include <string>

namespace daxia
{
	namespace encode
	{
		class Base64
		{
		public:
			Base64() = delete;
			~Base64(){};
		public:
			static std::string Encode(const char* str, unsigned int size);
			static std::string Decode(const char* str, unsigned int size);
		private:
			static const std::string _base64_table;
			static const char base64_pad;
		};// class Base64
	}// namespace encode
}// namespace daxia

#endif // !__DAXIA_ENCODE_BASE64_H

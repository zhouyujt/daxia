/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file strconv.hpp
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 * ascii、unicode、utf-8等字符串之间的编码相互转换
 *
 */
#ifndef __DAXIA_ENCODE_STRCONV_H
#define __DAXIA_ENCODE_STRCONV_H

#include <string>

namespace daxia
{
	namespace encode
	{
		class Strconv
		{
		public:
			Strconv() = delete;
			~Strconv(){};

		public:
			static std::string Unicode2Ansi(const wchar_t* str);
			static std::wstring Ansi2Unicode(const char* str);
			static std::wstring Utf82Unicode(const char* str);
			static std::string Utf82Ansi(const char* str);
			static std::string Unicode2Utf8(const wchar_t* str);
			static std::string Ansi2Utf8(const char* str);
		};// class strconv
	}// namespace encode
}// namespace daxia

#endif // !__DAXIA_ENCODE_STRCONV_H

/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file strconv.h
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
			static std::string Unicode2Ansi(const std::wstring& str);

			static std::wstring Ansi2Unicode(const char* str);
			static std::wstring Ansi2Unicode(const std::string& str);

			static std::wstring Utf82Unicode(const char* str);
			static std::wstring Utf82Unicode(const std::string& str);

			static std::string Utf82Ansi(const char* str);
			static std::string Utf82Ansi(const std::string& str);

			static std::string Unicode2Utf8(const wchar_t* str);
			static std::string Unicode2Utf8(const std::wstring& str);

			static std::string Ansi2Utf8(const char* str);
			static std::string Ansi2Utf8(const std::string& str);

			static std::string MakeLower(const char* str);
			static std::string MakeLower(const std::string& str);
			static std::wstring MakeLower(const wchar_t* str);
			static std::wstring MakeLower(const std::wstring& str);

			static std::string MakeUpper(const char* str);
			static std::string MakeUpper(const std::string& str);
			static std::wstring MakeUpper(const wchar_t* str);
			static std::wstring MakeUpper(const std::wstring& str);
		};// class Strconv
	}// namespace encode
}// namespace daxia

#endif // !__DAXIA_ENCODE_STRCONV_H

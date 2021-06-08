/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file strconv.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ʮ�� 2018
 *
 * ascii��unicode��utf-8���ַ���֮��ı����໥ת��
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

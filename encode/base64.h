/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file base64.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2021
 *
 * ��װboost���BASE64�ı����
 *
 */
#ifndef __DAXIA_ENCODE_BASE64_H
#define __DAXIA_ENCODE_BASE64_H
#include <string>
#include "../string.hpp"

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
			static daxia::string Marshal(const char* data, size_t size);
			static daxia::string Marshal(const std::string& str);
			static daxia::string Unmarshal(const char* str);
			static daxia::string Unmarshal(const std::string& str);
		};// class Base64
	}// namespace encode
}// namespace daxia

#endif // !__DAXIA_ENCODE_BASE64_H

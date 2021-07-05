/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file base64.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2021
 *
 * BASE64�ı����
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

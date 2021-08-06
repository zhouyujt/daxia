/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file hex.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ʮ�����Ʊ���
*
*/
#ifndef __DAXIA_ENCODE_HEX_H
#define  __DAXIA_ENCODE_HEX_H
#include "../string.hpp"
namespace daxia
{
	namespace encode
	{
		class Hex
		{
		protected:
			Hex();
			~Hex();
		public:
			static daxia::string ToString(const void* data, size_t size);
			static daxia::string FromString(const daxia::string& str);
		private:
			static daxia::string table_;
		};
	}
}

#endif	// !__DAXIA_ENCODE_HEX_H


/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file url.h
* \author 漓江里的大虾
* \date 八月 2021
*
* URL编解码
*
*/
#ifndef __DAXIA_ENCODE_URL_H
#define  __DAXIA_ENCODE_URL_H
#include <unordered_set>
#include "../string.hpp"
namespace daxia
{
	namespace encode
	{
		class Url
		{
		protected:
			Url();
			~Url();
		public:
			static daxia::string Marshal(const void* data, size_t size);
			static daxia::string Marshal(const daxia::string& str);
			static daxia::buffer Unmarshal(const void* data, size_t size);
			static daxia::buffer Unmarshal(const daxia::string& str);
		private:
			inline static bool isAlpha(char ch);
		};
	}
}

#endif	// !__DAXIA_ENCODE_URL_H


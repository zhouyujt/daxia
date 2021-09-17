/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file zlib.h
* \author 漓江里的大虾
* \date 九月 2021
*
* 封装zlib的一些功能
*
*/
#ifndef __DAXIA_ENCODE_ZLIB_H
#define  __DAXIA_ENCODE_ZLIB_H
#include "../string.hpp"
namespace daxia
{
	namespace encode
	{
		class Zlib
		{
		protected:
			Zlib();
			~Zlib();
		public:
			// 压缩内存数据
			static daxia::buffer Marshal(const void* data, size_t size);
			static daxia::buffer Marshal(const daxia::string& str);
			// 压缩文件/文件夹
			static void MarshalFile(const daxia::string& filename, const daxia::string& destination);
			// 解压内存数据
			static daxia::buffer Unmarshal(const void* data, size_t size);
			static daxia::buffer Unmarshal(const daxia::string& str);
			// 解压文件/文件夹
			static daxia::buffer UnmarshalFile(const daxia::string& filename, const daxia::string& destination);
		};
	}
}

#endif	// !__DAXIA_ENCODE_ZLIB_H
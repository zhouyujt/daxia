/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file zlib.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ��װzlib��һЩ����
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
			// ѹ���ڴ�����
			static daxia::buffer Marshal(const void* data, size_t size);
			static daxia::buffer Marshal(const daxia::string& str);
			// ѹ���ļ�/�ļ���
			static void MarshalFile(const daxia::string& filename, const daxia::string& destination);
			// ��ѹ�ڴ�����
			static daxia::buffer Unmarshal(const void* data, size_t size);
			static daxia::buffer Unmarshal(const daxia::string& str);
			// ��ѹ�ļ�/�ļ���
			static daxia::buffer UnmarshalFile(const daxia::string& filename, const daxia::string& destination);
		};
	}
}

#endif	// !__DAXIA_ENCODE_ZLIB_H
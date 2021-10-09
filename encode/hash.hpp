/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file hash.hpp
* \author 漓江里的大虾
* \date 十月 2021
*
* 获取哈希值
*
*/
#ifndef __DAXIA_ENCODE_HASH_H
#define  __DAXIA_ENCODE_HASH_H

namespace daxia
{
	namespace encode
	{
		class Hash
		{
		protected:
			Hash(){}
			~Hash(){}
		public:
			static unsigned int Marshal(const char* data, size_t len)
			{
				unsigned int seed = 131;
				unsigned int hash = 0;
				for (size_t i = 0; i < len; ++i)
				{
					hash = hash * seed + (*(data + i));
				}

				return (hash & 0x7fffffff);
			}

			template<typename T>
			static unsigned int Marshal(const T& v)
			{
				return Marshal(reinterpret_cast<const char*>(&v), sizeof(v));
			}
		};
	}
}

#endif	// !__DAXIA_ENCODE_HASH_H


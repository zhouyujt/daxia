/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file buffer.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 */

#ifndef __DAXIA_NET_COMMON_SHARED_BUFFER_H
#define __DAXIA_NET_COMMON_SHARED_BUFFER_H

#ifdef _MSC_VER
#include <sdkddkver.h>
#endif

#include <memory>
#include <boost/asio.hpp>
#include "define.h"

namespace daxia
{
	namespace net
	{
		namespace common
		{
#ifdef _MSC_VER
#define ATTRIBUTE_PACKED
#else
#define ATTRIBUTE_PACKED  __attribute__ ((__packed__))
#endif // _MSC_VER

#ifdef _MSC_VER
#pragma pack(1)
#endif
			// 分页信息（大数据需分页传送）
			struct ATTRIBUTE_PACKED PageInfo
			{
				unsigned int startPos{ 0 };	// 起始位置
				unsigned int endPos{ 0 };		// 结束位置
				unsigned int total{ 0 };		// 总大小

				// 获取总页数
				unsigned int Count() const
				{
					return (total + (common::MaxBufferSize - 1)) / common::MaxBufferSize;
				}

				// 获取当前页数基于0
				unsigned int Index() const
				{
					return endPos / common::MaxBufferSize;
				}
			};
#ifdef _MSC_VER
#pragma pack()
#endif

#undef ATTRIBUTE_PACKED

			// 缓冲区类
			class Buffer
			{
			public:
				Buffer();
				Buffer(size_t capacity);
				Buffer(const char data[], size_t size);
			public:
				void Clear();
				size_t Size() const;
				size_t Capacity() const;
				void Resize(size_t size);
				void Reserve(size_t capacity);
				bool IsEmpty() const;
				boost::asio::mutable_buffers_1 GetAsioBuffer(size_t offset = 0) const;
				operator char* () const
				{
					return buff_.get();
				}

				PageInfo& Page()
				{
					return pageInfo_;
				}

				const PageInfo& Page() const
				{
					return pageInfo_;
				}
			private:
				std::shared_ptr<char> buff_;
				size_t size_;
				size_t capacity_;
				PageInfo pageInfo_;
			};

		}// namespace common
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_COMMON_SHARED_BUFFER_H


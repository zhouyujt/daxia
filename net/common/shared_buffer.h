/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file shared_buffer.h
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
			// 缓冲区类
			class shared_buffer
			{
			public:
				shared_buffer();
				shared_buffer(size_t capacity);
				shared_buffer(const char data[], size_t size);
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
			private:
				std::shared_ptr<char> buff_;
				size_t size_;
				size_t capacity_;
			};

		}// namespace common
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_COMMON_SHARED_BUFFER_H


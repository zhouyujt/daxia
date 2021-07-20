/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file shared_buffer.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 */

#ifndef __DAXIA_DXG_COMMON_SHARED_BUFFER_H
#define __DAXIA_DXG_COMMON_SHARED_BUFFER_H

#ifdef _MSC_VER
#include <sdkddkver.h>
#endif

#include <memory>
#include <boost/asio.hpp>
#include "define.h"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			// ��������
			class shared_buffer
			{
			public:
				shared_buffer();
				shared_buffer(size_t capacity);
				shared_buffer(const byte data[], size_t size);
			public:
				void clear();
				size_t size() const;
				size_t capacity() const;
				void resize(size_t size);
				void reserve(size_t capacity);
				bool empty() const;
				byte* get() const;
				boost::asio::mutable_buffers_1 asio_buffer(size_t offset = 0) const;
			private:
				std::shared_ptr<byte> buff_;
				size_t size_;
				size_t capacity_;
			};

		}// namespace common
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_COMMON_SHARED_BUFFER_H


/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file shared_buffer.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
 * 
 */

#ifndef __DAXIA_DXG_COMMON_SHARED_BUFFER_HPP
#define __DAXIA_DXG_COMMON_SHARED_BUFFER_HPP

#include <memory>
#include <boost/asio.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			// 缓冲区类
			struct shared_buffer
			{
				std::shared_ptr<byte> buff_;
				size_t size_;
				size_t capacity_;

				shared_buffer()
					: size_(0)
					, capacity_(0)
				{
				}

				shared_buffer(size_t capacity)
					: buff_(new byte[capacity])
					, capacity_(capacity)
					, size_(0)
				{
				}

				shared_buffer(const byte data[], size_t size)
					: buff_(new byte[size])
					, size_(size)
					, capacity_(size)
				{
					memcpy(buff_.get(), data, size);
				}

				void clear()
				{
					size_ = 0;
				}

				size_t size() const
				{
					return size_;
				}

				size_t capacity() const
				{
					return capacity_;
				}

				void resize(size_t size)
				{
					if (size > capacity_)
					{
						size_t newCapacity = capacity_;
						if (newCapacity == 0)
						{
							newCapacity = 1;
						}

						while (newCapacity < size)
						{
							newCapacity *= 2;
						}

						std::shared_ptr<byte> temp(new byte[newCapacity]);

						if (buff_.get())
						{
							memcpy(temp.get(), buff_.get(), size);
						}

						capacity_ = newCapacity;
						buff_ = temp;
					}

					size_ = size;
				}

				void reserve(size_t capacity)
				{
					if (size_ > capacity)
					{
						size_ = capacity;
					}
					else if (size_ < capacity)
					{
						if (capacity > capacity)
						{
							std::shared_ptr<byte> temp(new byte[capacity]);
							memcpy(temp.get(), buff_.get(), size_);
							buff_ = temp;
						}
					}

					capacity = capacity;
				}

				bool empty() const
				{
					return size_ == 0;
				}

				byte* get() const
				{
					return buff_.get();
				}

				boost::asio::mutable_buffers_1 asio_buffer(size_t offset = 0) const
				{
					return boost::asio::buffer(buff_.get() + offset, capacity_ - offset);
				}
			};
		}// namespace common
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_COMMON_SHARED_BUFFER_HPP


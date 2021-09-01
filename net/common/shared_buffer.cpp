#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include "shared_buffer.h"
#include "define.h"

namespace daxia
{
	namespace net
	{
		namespace common
		{

			shared_buffer::shared_buffer()
				: size_(0)
				, capacity_(0)
			{
			}

			shared_buffer::shared_buffer(size_t capacity)
				: buff_(new char[capacity])
				, capacity_(capacity)
				, size_(0)
			{
			}

			shared_buffer::shared_buffer(const char data[], size_t size)
				: buff_(new char[size])
				, size_(size)
				, capacity_(size)
			{
				memcpy(buff_.get(), data, size);
			}

			void shared_buffer::Clear()
			{
				size_ = 0;
			}

			size_t shared_buffer::Size() const
			{
				return size_;
			}

			size_t shared_buffer::Capacity() const
			{
				return capacity_;
			}

			void shared_buffer::Resize(size_t size)
			{
				if (size > capacity_)
				{
					size_t newCapacity = capacity_;
					if (newCapacity == 0)
					{
						newCapacity = size;
					}

					// 1.5��������ֱ��1G��Ȼ��ÿ������1M
					while (newCapacity < size)
					{
						if (newCapacity < 1024 * 1024 * 1024)
						{
							newCapacity = newCapacity + newCapacity / 2;
						}
						else
						{
							newCapacity += 1024 * 1024;
						}
					}

					std::shared_ptr<char> temp(new char[newCapacity]);

					if (buff_.get())
					{
						memcpy(temp.get(), buff_.get(), size);
					}

					capacity_ = newCapacity;
					buff_ = temp;
				}

				size_ = size;
			}

			void shared_buffer::Reserve(size_t capacity)
			{
				if (size_ > capacity)
				{
					size_ = capacity;
				}
				else if (size_ < capacity)
				{
					if (capacity > capacity_)
					{
						std::shared_ptr<char> temp(new char[capacity]);
						memcpy(temp.get(), buff_.get(), size_);
						buff_ = temp;
					}
				}

				capacity_ = capacity;
			}

			bool shared_buffer::IsEmpty() const
			{
				return size_ == 0;
			}

			boost::asio::mutable_buffers_1 shared_buffer::GetAsioBuffer(size_t offset) const
			{
				return boost::asio::buffer(buff_.get() + offset, capacity_ - offset);
			}

		}// namespace common
	}// namespace net
}// namespace daxia

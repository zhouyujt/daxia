#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "buffer.h"
#include "define.h"

namespace daxia
{
	namespace net
	{
		namespace common
		{

			Buffer::Buffer()
				: size_(0)
				, capacity_(0)
			{
			}

			Buffer::Buffer(size_t capacity)
				: buff_(new char[capacity])
				, size_(0)
				, capacity_(capacity)
			{
			}

			Buffer::Buffer(const char data[], size_t size)
				: buff_(new char[size])
				, size_(size)
				, capacity_(size)
			{
				memcpy(buff_.get(), data, size);
			}

			void Buffer::Clear()
			{
				size_ = 0;
			}

			size_t Buffer::Size() const
			{
				return size_;
			}

			size_t Buffer::Capacity() const
			{
				return capacity_;
			}

			void Buffer::Resize(size_t size)
			{
				if (size > capacity_)
				{
					size_t newCapacity = capacity_;
					if (newCapacity == 0)
					{
						newCapacity = size;
					}

					// 1.5倍增长，直到1G，然后每次增长1M
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

			void Buffer::Reserve(size_t capacity)
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

			bool Buffer::IsEmpty() const
			{
				return size_ == 0;
			}

			boost::asio::mutable_buffers_1 Buffer::GetAsioBuffer(size_t offset) const
			{
				return boost::asio::buffer(buff_.get() + offset, capacity_ - offset);
			}

		}// namespace common
	}// namespace net
}// namespace daxia

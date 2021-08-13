/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2021 漓江里的大虾.
 * All rights reserved.
 *
 * \file byte_order.hpp
 * \author 漓江里的大虾
 * \date 七月 2021
 *
 */

#ifndef __DAXIA_NET_COMMON_BYTE_ORDER_HPP
#define __DAXIA_NET_COMMON_BYTE_ORDER_HPP

namespace daxia
{
	namespace net
	{
		namespace common
		{

			class ByteOrder
			{
			private:
				ByteOrder() = delete;
				~ByteOrder(){}
			public:
				static bool IsLittleEndian()
				{
					union
					{
						int i;
						char c;
					}test = { 1 };

					return test.c == 1;
				}

				static bool IsBigEndian()
				{
					return !IsLittleEndian();
				}

				template<typename ValueType>
				static ValueType hton(ValueType v)
				{
					if (IsBigEndian()) return v;

					ValueType result;

					unsigned char* pr = reinterpret_cast<unsigned char*>(&result);
					unsigned char* pv = reinterpret_cast<unsigned char*>(&v);
					for (int i = 0; i < sizeof(v); ++i)
					{
						pr[i] = pv[sizeof(v) - i - 1];
					}

					return result;
				}

				template<typename ValueType>
				static ValueType ntoh(ValueType v)
				{
					if (IsBigEndian()) return v;

					return hton(v);
				}
			};
		}
	}
}

#endif // !__DAXIA_NET_COMMON_BYTE_ORDER_HPP

/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file buffer.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
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
			// ��ҳ��Ϣ�����������ҳ���ͣ�
			struct PageInfo
			{
				int msgId{ common::DefMsgID_UnHandle };			// ��ϢID
				size_t startPos{ 0 };	// ��ʼλ��
				size_t endPos{ 0 };		// ����λ��
				size_t total{ 0 };		// �ܴ�С

				// ��ȡ��ҳ��
				size_t Count() const
				{
					return (total + (common::MaxBufferSize - 1)) / common::MaxBufferSize;
				}

				// ��ȡ��ǰҳ������0
				size_t Index() const
				{
					return endPos / common::MaxBufferSize;
				}
			};

			// ��������
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


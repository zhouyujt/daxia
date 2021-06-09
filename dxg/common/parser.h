/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file parser.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 * ��Ϣ�������ӿ���
 *
 */

#ifndef __DAXIA_DXG_COMMON_PARSER_H
#define __DAXIA_DXG_COMMON_PARSER_H

#include "define.h"
#include "shared_buffer.h"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			class BasicSession;

			// ��Ϣ����������
			class Parser
			{
			public:
				Parser(){}
				~Parser(){}
			public:
				virtual size_t GetPacketHeadLen() const = 0;

				virtual bool Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const = 0;
				virtual bool UnmarshalHead(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, size_t& contentLen) const = 0;
				virtual bool UnmarshalContent(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer) const = 0;
			};

			class DefaultParser : public Parser
			{
			public:
				DefaultParser(){}
				~DefaultParser(){}
			public:
#pragma pack(1)
				// ���ݰ�ͷ
				struct PacketHead
				{
					byte	magic;			// �㶨Ϊ88
					byte	hearbeat;		// ��������ʶ
					unsigned int len;		// ���ݳ��ȣ�����������ͷ
					unsigned long reserve;	// ��������
				};
#pragma pack()
			public:
				virtual size_t GetPacketHeadLen() const override;

				virtual bool Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const override;
				virtual bool UnmarshalHead(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, size_t& contentLen) const override;
				virtual bool UnmarshalContent(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer) const override;
			};
		}// namespace common
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_COMMON_PARSER_H


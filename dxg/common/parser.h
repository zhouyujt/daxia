/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file parser.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 * 消息解析器接口类
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

			// 消息解析器基类
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
				// 数据包头
				struct PacketHead
				{
					byte	magic;			// 恒定为88
					byte	hearbeat;		// 心跳包标识
					unsigned int len;		// 数据长度，不包括本包头
					unsigned long reserve;	// 保留数据
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


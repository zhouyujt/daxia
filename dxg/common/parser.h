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
				enum Result : int
				{
					Result_Success = 0,		// 解析成功
					Result_Fail,			// 格式错误，解析失败
					Result_Uncomplete		// 需要更多数据
				};
			public:
				// 封装消息
				virtual bool Marshal(daxia::dxg::common::BasicSession* session,	// 会话指针
					const daxia::dxg::common::byte* data,							// 需封装的数据
					size_t len,														// data大小，单位字节
					daxia::dxg::common::shared_buffer& buffer						// 封装后的数据
					) const = 0;

				// 解析消息
				virtual Result Unmarshal(daxia::dxg::common::BasicSession* session,	// 会话指针 
					const daxia::dxg::common::byte* data,								// 解封的数据
					size_t len,															// data大小，单位字节
					int& msgID,															// 解析出的消息ID
					daxia::dxg::common::shared_buffer& buffer,							// 解析后的数据			
					size_t& packetLen													// 封包长度，单位字节
					) const = 0;
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
				virtual bool Marshal(daxia::dxg::common::BasicSession* session, 
					const daxia::dxg::common::byte* data, 
					size_t len,
					daxia::dxg::common::shared_buffer& buffer
					) const override;

				virtual Result Unmarshal(daxia::dxg::common::BasicSession* session, 
					const daxia::dxg::common::byte* data, 
					size_t len,
					int& msgID,
					daxia::dxg::common::shared_buffer& buffer, 
					size_t& packetLen
					) const override;
			};
		}// namespace common
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_COMMON_PARSER_H


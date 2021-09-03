/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file parser.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 * 消息解析器接口类
 *
 */

#ifndef __DAXIA_NET_COMMON_PARSER_H
#define __DAXIA_NET_COMMON_PARSER_H

#include <vector>
#include "define.h"
#include "buffer.h"

namespace daxia
{
	namespace net
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
				virtual bool Marshal(daxia::net::common::BasicSession* session,	// 会话指针
					int msgId,													// 消息ID				
					const void* data,											// 需封装的数据
					size_t len,													// data大小，单位字节
					std::vector<daxia::net::common::Buffer>& buffers			// 封装后的数据
					) const = 0;

				// 解析消息
				virtual Result Unmarshal(daxia::net::common::BasicSession* session,	// 会话指针 
					const void* data,												// 解封的数据
					size_t len,														// data大小，单位字节
					int& msgID,														// 解析出的消息ID
					daxia::net::common::Buffer& buffer,								// 解析后的数据			
					size_t& packetLen												// 封包长度(包括头跟正文)，单位字节
					) const = 0;
			};

			class DefaultParser : public Parser
			{
			public:
				DefaultParser(){}
				~DefaultParser(){}
			public:
#ifdef _MSC_VER
#define ATTRIBUTE_PACKED
#else
#define ATTRIBUTE_PACKED  __attribute__ ((__packed__))
#endif // _MSC_VER

#ifdef _MSC_VER
#pragma pack(1)
#endif
				// 数据包头
				struct ATTRIBUTE_PACKED PacketHead
				{
					char	magic{ 88 };			// 恒定为88
					int		msgId;					// 消息ID
					unsigned int contentLength;		// 数据长度，不包括本包头
					PageInfo pageInfo;				// 分页数据
				};

#ifdef _MSC_VER
#pragma pack()
#endif

#undef ATTRIBUTE_PACKED
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session, 
					int msgId,
					const void* data,
					size_t len,
					std::vector<daxia::net::common::Buffer>& buffers
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session, 
					const void* data, 
					size_t len,
					int& msgID,
					daxia::net::common::Buffer& buffer, 
					size_t& packetLen
					) const override;
			};
		}// namespace common
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_COMMON_PARSER_H


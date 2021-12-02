/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file websocket_parser.h
* \author 漓江里的大虾
* \date 十二月 2021
*
* websocket协议解析器
*
* 参考文献：
* RFC6455 https://datatracker.ietf.org/doc/rfc6455/
*/

#ifndef __DAXIA_NET_COMMON_WEBSOCKETPARSER_H
#define __DAXIA_NET_COMMON_WEBSOCKETPARSER_H
#include "http_parser.h"
#include "../../string.hpp"

/*
*Websocket协议帧:
*+---------------+---------------+---------------+---------------+
*|       0		 |       1       |       2	     |	     3       |		
*|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*|F|R|R|R|opcode |M|Payload len  |    Extended payload length    |
*|I|S|S|S|  (4)  |A|    (7)	     |              (16/64)		     |
*|N|V|V|V|       |S|		     | (if payload len == 126/127)   |
*| |1|2|3|		 |K|		     |                               |
*+-+-+-+-+-------+-+-------------+-------------------------------+
*|	Extended payload length continued, if payload len == 127     |
*+-------------------------------+-------------------------------+
*|							     |Masking-key,if MASK set to 1   |
*+-------------------------------+-------------------------------+
*| Masking-key(continued)	     |            PayloadData        |
*+-------------------------------+-------------------------------+
*:					Payload Data continued ...                   :
*+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
*|					Payload Data continued ...                   |
*+---------------------------------------------------------------+
*
* FIN: 1位，用来表明这是一个帧的最后的帧片段，当然第一个帧片段也可能是最后的一个帧片段
*
* RSV1, RSV2, RSV3: 分别都是1位，如果双方之间没有约定自定义协议，那么这几位的值都必须为0,否则必须断掉WebSocket连接
* Opcode: 4位操作码，定义有效负载数据，如果收到了一个未知的操作码，连接也必须断掉，以下是定义的操作码
*	%x0 表示帧片段
*	%x1 表示是文本帧
*	%x2 表示是二进制帧
*	%x3-7 为将来的非控制消息片断保留的操作码
*	%x8 表示连接关闭
*	%x9 表示心跳检查的ping
*	%xA 表示心跳检查的pong
*	%xB-F 为将来的控制消息片断的保留操作码
*
* Mask: 1位，定义传输的数据是否有加掩码,如果设置为1,掩码键必须放在masking-key区域，客户端发送给服务端的所有消息，此位的值都是1；
*
* Payload length: 传输数据的长度，以字节的形式表示：7位、7+16位、或者7+64位。如果这个值以字节表示是0-125这个范围，那这个值就表示传输数据的长度；
* 如果这个值是126，则随后的两个字节表示的是一个16进制无符号数，用来表示传输数据的长度；如果这个值是127,则随后的是8个字节表示的一个64位无符合数，
* 这个数用来表示传输数据的长度。多字节长度的数量是以网络字节的顺序表示。负载数据的长度为扩展数据及应用数据之和，扩展数据的长度可能为0,因而此时
* 负载数据的长度就为应用数据的长度
*
* Masking-key: 0或4个字节，客户端发送给服务端的数据，都是通过内嵌的一个32位值作为掩码的；掩码键只有在掩码位设置为1的时候存在
*
* Payload data: (x+y)位，负载数据为扩展数据及应用数据长度之和
*
* Extension data: x位，如果客户端与服务端之间没有特殊约定，那么扩展数据的长度始终为0，任何的扩展都必须指定扩展数据的长度，或者长度的计算方式，
* 以及在握手时如何确定正确的握手方式。如果存在扩展数据，则扩展数据就会包括在负载数据的长度之内
*
* Application data: y位，任意的应用数据，放在扩展数据之后，应用数据的长度=负载数据的长度-扩展数据的长度
*/

namespace daxia
{
	namespace net
	{
		namespace common
		{
			class WebsocketServerParser;
			class WebsocketClientParser;

#ifdef _MSC_VER
#define ATTRIBUTE_PACKED
#else
#define ATTRIBUTE_PACKED  __attribute__ ((__packed__))
#endif // _MSC_VER

#ifdef _MSC_VER
#pragma pack(1)
#endif
			// Websocket 协议头
			struct ATTRIBUTE_PACKED WebsocketHeader
			{
				unsigned char op : 4;
				unsigned char rsv3 : 1;
				unsigned char rsv2 : 1;
				unsigned char rsv1 : 1;
				unsigned char fin : 1;
				unsigned char len : 7;
				unsigned char mask : 1;

				WebsocketHeader()
				{
					op = 0;
					rsv1 = 0;
					rsv2 = 0;
					rsv3 = 0;
					fin = 0;
					len = 0;
					mask = 0;
				}
			};

			// Websocket 协议头（16位数据长度）
			struct ATTRIBUTE_PACKED WebsocketHeader16 : public WebsocketHeader
			{
				unsigned short len16;

				WebsocketHeader16()
				{
					len16 = 0;
				}
			};

			// Websocket 协议头（64位数据长度）
			struct ATTRIBUTE_PACKED WebsocketHeader64 : public WebsocketHeader
			{
				unsigned long long len64;

				WebsocketHeader64()
				{
					len64 = 0;
				}
			};

#ifdef _MSC_VER
#pragma pack()
#endif

#undef  ATTRIBUTE_PACKED

			// Websocket 服务端解析器
			class WebsocketServerParser : public HttpServerParser
			{
			public:
				WebsocketServerParser();
				~WebsocketServerParser();
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session,
					int msgId,
					const void* data,
					size_t len,
					const daxia::net::common::PageInfo* pageInfo,
					std::vector<daxia::net::common::Buffer>& buffers,
					size_t maxPacketLength
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const void* data,
					size_t len,
					int& msgID,
					daxia::net::common::Buffer& buffer,
					size_t& packetLen
					) const override;
			public:
				// 协议握手
				static bool Handshake(daxia::net::common::BasicSession* session);
			};

			// Websocket 客户端解析器
			class WebsocketClientParser : public HttpClientParser
			{
			public:
				WebsocketClientParser();
				~WebsocketClientParser();
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session,
					int msgId,
					const void* data,
					size_t len,
					const daxia::net::common::PageInfo* pageInfo,
					std::vector<daxia::net::common::Buffer>& buffers,
					size_t maxPacketLength
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const void* data,
					size_t len,
					int& msgID,
					daxia::net::common::Buffer& buffer,
					size_t& packetLen
					) const override;
			};
		}
	}
}

#endif // !__DAXIA_NET_COMMON_WEBSOCKETPARSER_H

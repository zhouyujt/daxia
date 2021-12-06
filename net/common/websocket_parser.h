/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file websocket_parser.h
* \author �콭��Ĵ�Ϻ
* \date ʮ���� 2021
*
* websocketЭ�������
*
* �ο����ף�
* RFC6455 https://datatracker.ietf.org/doc/rfc6455/
*/

#ifndef __DAXIA_NET_COMMON_WEBSOCKETPARSER_H
#define __DAXIA_NET_COMMON_WEBSOCKETPARSER_H
#include "http_parser.h"
#include "../../string.hpp"

/*
*WebsocketЭ��֡:
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
* FIN: 1λ��������������һ��֡������֡Ƭ�Σ���Ȼ��һ��֡Ƭ��Ҳ����������һ��֡Ƭ��
*
* RSV1, RSV2, RSV3: �ֱ���1λ�����˫��֮��û��Լ���Զ���Э�飬��ô�⼸λ��ֵ������Ϊ0,�������ϵ�WebSocket����
* Opcode: 4λ�����룬������Ч�������ݣ�����յ���һ��δ֪�Ĳ����룬����Ҳ����ϵ��������Ƕ���Ĳ�����
*	%x0 ��ʾ֡Ƭ��
*	%x1 ��ʾ���ı�֡
*	%x2 ��ʾ�Ƕ�����֡
*	%x3-7 Ϊ�����ķǿ�����ϢƬ�ϱ����Ĳ�����
*	%x8 ��ʾ���ӹر�
*	%x9 ��ʾ��������ping
*	%xA ��ʾ��������pong
*	%xB-F Ϊ�����Ŀ�����ϢƬ�ϵı���������
*
* Mask: 1λ�����崫��������Ƿ��м�����,�������Ϊ1,������������masking-key���򣬿ͻ��˷��͸�����˵�������Ϣ����λ��ֵ����1��
*
* Payload length: �������ݵĳ��ȣ����ֽڵ���ʽ��ʾ��7λ��7+16λ������7+64λ��������ֵ���ֽڱ�ʾ��0-125�����Χ�������ֵ�ͱ�ʾ�������ݵĳ��ȣ�
* ������ֵ��126�������������ֽڱ�ʾ����һ��16�����޷�������������ʾ�������ݵĳ��ȣ�������ֵ��127,��������8���ֽڱ�ʾ��һ��64λ�޷�������
* �����������ʾ�������ݵĳ��ȡ����ֽڳ��ȵ��������������ֽڵ�˳���ʾ���������ݵĳ���Ϊ��չ���ݼ�Ӧ������֮�ͣ���չ���ݵĳ��ȿ���Ϊ0,�����ʱ
* �������ݵĳ��Ⱦ�ΪӦ�����ݵĳ���
*
* Masking-key: 0��4���ֽڣ��ͻ��˷��͸�����˵����ݣ�����ͨ����Ƕ��һ��32λֵ��Ϊ����ģ������ֻ��������λ����Ϊ1��ʱ�����
*
* Payload data: (x+y)λ����������Ϊ��չ���ݼ�Ӧ�����ݳ���֮��
*
* Extension data: xλ������ͻ���������֮��û������Լ������ô��չ���ݵĳ���ʼ��Ϊ0���κε���չ������ָ����չ���ݵĳ��ȣ����߳��ȵļ��㷽ʽ��
* �Լ�������ʱ���ȷ����ȷ�����ַ�ʽ�����������չ���ݣ�����չ���ݾͻ�����ڸ������ݵĳ���֮��
*
* Application data: yλ�������Ӧ�����ݣ�������չ����֮��Ӧ�����ݵĳ���=�������ݵĳ���-��չ���ݵĳ���
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
			// Websocket Э��ͷ
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

			// Websocket Э��ͷ��16λ���ݳ��ȣ�
			struct ATTRIBUTE_PACKED WebsocketHeader16 : public WebsocketHeader
			{
				unsigned short len16;

				WebsocketHeader16()
				{
					len16 = 0;
				}
			};

			// Websocket Э��ͷ��64λ���ݳ��ȣ�
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

			// Websocket ����˽�����
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
				// Э������
				static bool Handshake(daxia::net::common::BasicSession* session);
			};

			// Websocket �ͻ��˽�����
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

/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file parser.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 * ��Ϣ�������ӿ���
 *
 */

#ifndef __DAXIA_NET_COMMON_PARSER_H
#define __DAXIA_NET_COMMON_PARSER_H

#include "define.h"
#include "shared_buffer.h"

namespace daxia
{
	namespace net
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
				enum Result : int
				{
					Result_Success = 0,		// �����ɹ�
					Result_Fail,			// ��ʽ���󣬽���ʧ��
					Result_Uncomplete		// ��Ҫ��������
				};
			public:
				// ��װ��Ϣ
				virtual bool Marshal(daxia::net::common::BasicSession* session,	// �Ựָ��
					const void* data,												// ���װ������
					size_t len,														// data��С����λ�ֽ�
					daxia::net::common::shared_buffer& buffer						// ��װ�������
					) const = 0;

				// ������Ϣ
				virtual Result Unmarshal(daxia::net::common::BasicSession* session,	// �Ựָ�� 
					const void* data,													// ��������
					size_t len,															// data��С����λ�ֽ�
					int& msgID,															// ����������ϢID
					daxia::net::common::shared_buffer& buffer,							// �����������			
					size_t& packetLen													// �������(����ͷ������)����λ�ֽ�
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
				// ���ݰ�ͷ
				struct ATTRIBUTE_PACKED PacketHead
				{
					char	magic;					// �㶨Ϊ88
					char	hearbeat;				// ��������ʶ
					unsigned int contentLength;		// ���ݳ��ȣ�����������ͷ
					unsigned int reserve;			// ��������
				};

#ifdef _MSC_VER
#pragma pack()
#endif

#undef ATTRIBUTE_PACKED
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session, 
					const void* data, 
					size_t len,
					daxia::net::common::shared_buffer& buffer
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session, 
					const void* data, 
					size_t len,
					int& msgID,
					daxia::net::common::shared_buffer& buffer, 
					size_t& packetLen
					) const override;
			};
		}// namespace common
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_COMMON_PARSER_H


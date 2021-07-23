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
				enum Result : int
				{
					Result_Success = 0,		// �����ɹ�
					Result_Fail,			// ��ʽ���󣬽���ʧ��
					Result_Uncomplete		// ��Ҫ��������
				};
			public:
				// ��װ��Ϣ
				virtual bool Marshal(daxia::dxg::common::BasicSession* session,	// �Ựָ��
					const daxia::dxg::common::byte* data,							// ���װ������
					size_t len,														// data��С����λ�ֽ�
					daxia::dxg::common::shared_buffer& buffer						// ��װ�������
					) const = 0;

				// ������Ϣ
				virtual Result Unmarshal(daxia::dxg::common::BasicSession* session,	// �Ựָ�� 
					const daxia::dxg::common::byte* data,								// ��������
					size_t len,															// data��С����λ�ֽ�
					int& msgID,															// ����������ϢID
					daxia::dxg::common::shared_buffer& buffer,							// �����������			
					size_t& packetLen													// ������ȣ���λ�ֽ�
					) const = 0;
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


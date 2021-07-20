/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file parser.hpp
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* HTTPЭ�������
*
*/

#ifndef __DAXIA_DXG_COMMON_HTTPPARSER_H
#define __DAXIA_DXG_COMMON_HTTPPARSER_H
#include "parser.h"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			class HttpParser : public Parser
			{
			public:
				HttpParser();
				~HttpParser();
			public:
				virtual size_t GetPacketHeadLen() const override;
				virtual bool Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const override;
				virtual bool UnmarshalHead(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, size_t& contentLen) const override;
				virtual bool UnmarshalContent(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer) const override;
			};
		}
	}
}
#endif // !__DAXIA_DXG_COMMON_HTTPPARSER_H

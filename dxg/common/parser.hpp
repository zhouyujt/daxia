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

#ifndef __DAXIA_DXG_COMMON_PARSER_HPP
#define __DAXIA_DXG_COMMON_PARSER_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <daxia/dxg/common/shared_buffer.hpp>

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
				typedef std::shared_ptr<Parser> ptr;
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

			inline size_t DefaultParser::GetPacketHeadLen() const
			{
				return sizeof(PacketHead);
			}

			inline bool DefaultParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const
			{
				buffer.clear();
				buffer.resize(sizeof(PacketHead) + len);

				PacketHead head;
				head.magic = 88;
				head.len = len;
				head.hearbeat = (data == nullptr && len == 0) ? 1 : 0;
				head.reserve = 0;

				memcpy(buffer.get(), &head, sizeof(head));

				if (head.hearbeat != 0)
				{
					memcpy(buffer.get() + sizeof(head), data, len);
				}

				return true;
			}

			inline bool DefaultParser::UnmarshalHead(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, size_t& contentLen) const
			{
				// 数据不足
				if (len < sizeof(PacketHead))
				{
					return false;
				}

				// 非法数据
				if (data[0] != 88)
				{
					return false;
				}

				const PacketHead* head = reinterpret_cast<const PacketHead*>(data);
				contentLen = head->len;

				return true;
			}

			inline bool DefaultParser::UnmarshalContent(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer) const
			{
				buffer.clear();

				// 数据不足
				if (len < sizeof(PacketHead))
				{
					return false;
				}

				// 非法数据
				if (data[0] != 88)
				{
					return false;
				}

				// 数据不足
				const PacketHead* head = reinterpret_cast<const PacketHead*>(data);
				if (head->len + sizeof(PacketHead) != len)
				{
					return false;
				}

				boost::property_tree::ptree root;
				std::stringstream s(std::string((const char*)data + sizeof(PacketHead), len - sizeof(PacketHead)));
				boost::property_tree::read_json<boost::property_tree::ptree>(s, root);

				try
				{
					msgID = root.get<int>("msgId");
				}
				catch (...)
				{
					return false;
				}

				buffer.resize(len - sizeof(PacketHead));
				memcpy(buffer.get(), data + sizeof(PacketHead), len - sizeof(PacketHead));

				return true;
			}

		}// namespace common
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_COMMON_PARSER_HPP


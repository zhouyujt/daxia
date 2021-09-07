#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "parser.h"
#include "../../encode/strconv.h"
#include "../../string.hpp"
#include "byte_order.hpp"

namespace daxia
{
	namespace net
	{
		namespace common
		{
			bool DefaultParser::Marshal(daxia::net::common::BasicSession* session, int msgId, const void* data, size_t len, const PageInfo* pageInfo, std::vector<daxia::net::common::Buffer>& buffers, size_t maxPacketLength) const
			{
#ifndef MIN
#define MIN(x,y) x < y ? x : y
#endif
				buffers.clear();
				using daxia::net::common::Buffer;

				PacketHead head;
				head.msgId = ByteOrder::hton(msgId);
				if (msgId == DefMsgID_Heartbeat)
				{
					Buffer buffer;
					buffer.Resize(sizeof(PacketHead));
					memcpy(buffer, &head, sizeof(head));
					buffers.push_back(buffer);
				}
				else
				{
					const unsigned int maxContentLength = maxPacketLength - sizeof(PacketHead);
					for (unsigned int offset = 0; offset < static_cast<unsigned int>(len); offset += maxContentLength)
					{
						Buffer buffer;
						unsigned int contentLength = MIN(static_cast<unsigned int>(len)-offset, maxContentLength);
						buffer.Resize(contentLength + sizeof(PacketHead));
						head.contentLength = ByteOrder::hton(contentLength);
						if (pageInfo == nullptr)
						{
							head.pageInfo.startPos = ByteOrder::hton(offset);
							head.pageInfo.endPos = ByteOrder::hton(offset + contentLength - 1);
							head.pageInfo.total = ByteOrder::hton(static_cast<unsigned int>(len));
							buffer.Page().startPos = offset;
							buffer.Page().endPos = offset + contentLength - 1;
							buffer.Page().total = len;
						}
						else
						{
							head.pageInfo.startPos = ByteOrder::hton(pageInfo->startPos);
							head.pageInfo.endPos = ByteOrder::hton(pageInfo->endPos);
							head.pageInfo.total = ByteOrder::hton(pageInfo->total);
							buffer.Page() = *pageInfo;
						}
						memcpy(buffer, &head, sizeof(head));
						memcpy(buffer + sizeof(head), static_cast<const char*>(data)+offset, contentLength);
						buffers.push_back(buffer);
					}
				}

				return true;
			}

			Parser::Result DefaultParser::Unmarshal(daxia::net::common::BasicSession* session, const void* data, size_t len, int& msgID, daxia::net::common::Buffer& buffer, size_t& packetLen) const
			{
				buffer.Clear();

				// 数据不足
				if (len < sizeof(PacketHead)) return Parser::Result::Result_Uncomplete;

				// 非法数据
				if (static_cast<const char*>(data)[0] != 88) return Parser::Result::Result_Fail;

				// 数据不足
				const PacketHead* head = static_cast<const PacketHead*>(data);
				int contentLen = ByteOrder::ntoh(head->contentLength);
				if (contentLen + sizeof(PacketHead) > static_cast<unsigned int>(len))  return Parser::Result::Result_Uncomplete;

				msgID = ByteOrder::ntoh(head->msgId);

				buffer.Resize(contentLen);
				memcpy(buffer, static_cast<const char*>(data)+sizeof(PacketHead), contentLen);
				buffer.Page().startPos = ByteOrder::ntoh(head->pageInfo.startPos);
				buffer.Page().endPos = ByteOrder::ntoh(head->pageInfo.endPos);
				buffer.Page().total = ByteOrder::ntoh(head->pageInfo.total);

				packetLen = sizeof(PacketHead)+contentLen;

				return Parser::Result::Result_Success;
			}
		}// namespace common
	}// namespace net
}// namespace daxia

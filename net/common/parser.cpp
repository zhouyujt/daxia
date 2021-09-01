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
			bool DefaultParser::Marshal(daxia::net::common::BasicSession* session, const void* data, size_t len, daxia::net::common::shared_buffer& buffer) const
			{
				buffer.clear();
				buffer.resize(sizeof(PacketHead) + len);

				PacketHead head;
				head.magic = 88;
				head.hearbeat = (data == nullptr && len == 0) ? 1 : 0;
				head.contentLength = ByteOrder::hton(static_cast<int>(len));
				head.reserve = 0;

				memcpy(buffer.get(), &head, sizeof(head));

				if (head.hearbeat == 0)
				{
					memcpy(buffer.get() + sizeof(head), data, len);
				}

				return true;
			}

			Parser::Result DefaultParser::Unmarshal(daxia::net::common::BasicSession* session, const void* data, size_t len, int& msgID, daxia::net::common::shared_buffer& buffer, size_t& packetLen) const
			{
				buffer.clear();

				// 数据不足
				if (len < sizeof(PacketHead)) return Parser::Result::Result_Uncomplete;

				// 非法数据
				if (reinterpret_cast<const char*>(data)[0] != 88) return Parser::Result::Result_Fail;

				// 数据不足
				const PacketHead* head = reinterpret_cast<const PacketHead*>(data);
				int contentLen = ByteOrder::ntoh(head->contentLength);
				if (contentLen + sizeof(PacketHead) > static_cast<unsigned int>(len))  return Parser::Result::Result_Uncomplete;
			
				if (!head->hearbeat)
				{
					// 为了提高效率不使用read_json，自己解析msgId
					daxia::string test(reinterpret_cast<const char*>(data), 32 > len ? len : 32);
					const char* field = "msgId\":";
					size_t start = test.Find(field);
					if (start != -1)
					{
						daxia::string value = test.Mid(start + strlen(field), 16);
						if (!value.IsEmpty())
						{
							msgID = value.NumericCast<int>();
						}
						else
						{
							return Parser::Result::Result_Fail;
						}
					}
					else
					{
						return Parser::Result::Result_Fail;
					}
				}
				else
				{
					msgID = DefMsgID_Heartbeat;
				}

				buffer.resize(contentLen);
				memcpy(buffer.get(), reinterpret_cast<const char*>(data) + sizeof(PacketHead), contentLen);

				packetLen = sizeof(PacketHead) + contentLen;

				return Parser::Result::Result_Success;
			}
		}// namespace common
	}// namespace net
}// namespace daxia

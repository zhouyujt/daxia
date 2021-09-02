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
			bool DefaultParser::Marshal(daxia::net::common::BasicSession* session, const void* data, size_t len, daxia::net::common::Buffer& buffer) const
			{
				buffer.Clear();
				buffer.Resize(sizeof(PacketHead) + len);

				PacketHead head;
				head.magic = 88;
				head.hearbeat = (data == nullptr && len == 0) ? 1 : 0;
				head.contentLength = ByteOrder::hton(static_cast<int>(len));
				head.reserve = 0;

				memcpy(buffer, &head, sizeof(head));

				if (head.hearbeat == 0)
				{
					memcpy(buffer + sizeof(head), data, len);
				}

				return true;
			}

			Parser::Result DefaultParser::Unmarshal(daxia::net::common::BasicSession* session, const void* data, size_t len, const daxia::net::common::PageInfo& lastPageInfo, int& msgID, daxia::net::common::Buffer& buffer, size_t& packetLen) const
			{
				buffer.Clear();

				if (lastPageInfo.Count() == 0 || lastPageInfo.Index() == lastPageInfo.Count() - 1)
				{
					// 数据不足
					if (len < sizeof(PacketHead)) return Parser::Result::Result_Uncomplete;

					// 非法数据
					if (static_cast<const char*>(data)[0] != 88) return Parser::Result::Result_Fail;

					size_t copied = 0;
					const PacketHead* head = static_cast<const PacketHead*>(data);
					int contentLen = ByteOrder::ntoh(head->contentLength);
					if (contentLen + sizeof(PacketHead) > static_cast<unsigned int>(len))
					{
						if (len < daxia::net::common::MaxBufferSize)
						{
							// 数据不足
							return Parser::Result::Result_Uncomplete;
						}
						else if(len == daxia::net::common::MaxBufferSize)
						{
							copied = len - sizeof(PacketHead);
						}
						else
						{
							assert(false);
						}
					}
					else
					{
						copied = contentLen;
					}

					// 获取msgId
					if (!head->hearbeat)
					{
						// 为了提高效率不使用read_json，自己解析msgId
						daxia::string test(static_cast<const char*>(data), 64 > len ? len : 64);
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

					buffer.Page().msgId = msgID;
					buffer.Page().total = contentLen;
					buffer.Page().startPos = 0;
					buffer.Page().endPos = copied > 0 ? copied - 1 : 0;
					buffer.Resize(copied);
					memcpy(buffer, static_cast<const char*>(data)+sizeof(PacketHead), copied);

					packetLen = copied + sizeof(PacketHead);

					return Parser::Result::Result_Success;
				}
				else
				{
					size_t remain = lastPageInfo.total - 1 - lastPageInfo.endPos;	// 剩余还未接收到的数据
					size_t copyied = len < remain ? len : remain;					// 本次需接收的数据

					buffer.Page().msgId = lastPageInfo.msgId;
					buffer.Page().startPos = lastPageInfo.endPos + 1;
					buffer.Page().endPos = buffer.Page().startPos + copyied - 1;
					buffer.Page().total = lastPageInfo.total;

					buffer.Resize(copyied);
					memcpy(buffer, data, copyied);

					msgID = lastPageInfo.msgId;
					packetLen = copyied;

					return Parser::Result::Result_Success;
				}
			}
		}// namespace common
	}// namespace net
}// namespace daxia

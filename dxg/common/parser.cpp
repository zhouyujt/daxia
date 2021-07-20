#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "parser.h"
#include "../../encode/strconv.h"
#include "../../string.hpp"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			size_t DefaultParser::GetPacketHeadLen() const
			{
				return sizeof(PacketHead);
			}

			bool DefaultParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const
			{
				buffer.clear();
				buffer.resize(sizeof(PacketHead) + len);

				PacketHead head;
				head.magic = 88;
				head.len = len;
				head.hearbeat = (data == nullptr && len == 0) ? 1 : 0;
				head.reserve = 0;

				memcpy(buffer.get(), &head, sizeof(head));

				if (head.hearbeat == 0)
				{
					memcpy(buffer.get() + sizeof(head), data, len);
				}

				return true;
			}

			bool DefaultParser::UnmarshalHead(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, size_t& contentLen) const
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

			bool DefaultParser::UnmarshalContent(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer) const
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
				if (head->len + sizeof(PacketHead) > static_cast<unsigned int>(len))
				{
					return false;
				}

				if (!head->hearbeat)
				{
					// 为了提高效率不使用read_json，自己解析msgId
					daxia::StringA test(reinterpret_cast<const char*>(data), 32 > len ? len : 32);
					const char* field = "msgId\":";
					int start = test.Find(field);
					if (start != -1)
					{
						daxia::StringA value = test.Mid(start + strlen(field), 16);
						if (!value.IsEmpty())
						{
							msgID = atoi(value);
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					msgID = DefMsgID_Heartbeat;
				}

				buffer.resize(head->len);
				memcpy(buffer.get(), data + sizeof(PacketHead), head->len);

				return true;
			}

		}// namespace common
	}// namespace dxg
}// namespace daxia

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

			Parser::Result DefaultParser::Unmarshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer, int& packetLen) const
			{
				buffer.clear();

				// ���ݲ���
				if (len < sizeof(PacketHead)) return Parser::Result::Result_Uncomplete;

				// �Ƿ�����
				if (data[0] != 88) return Parser::Result::Result_Fail;

				// ���ݲ���
				const PacketHead* head = reinterpret_cast<const PacketHead*>(data);
				int contentLen = head->len;
				if (head->len + sizeof(PacketHead) > static_cast<unsigned int>(len))  return Parser::Result::Result_Uncomplete;
			
				if (!head->hearbeat)
				{
					// Ϊ�����Ч�ʲ�ʹ��read_json���Լ�����msgId
					daxia::StringA test(reinterpret_cast<const char*>(data), 32 > len ? len : 32);
					const char* field = "msgId\":";
					int start = test.Find(field);
					if (start != -1)
					{
						daxia::StringA value = test.Mid(start + static_cast<int>(strlen(field)), 16);
						if (!value.IsEmpty())
						{
							msgID = atoi(value);
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

				buffer.resize(head->len);
				memcpy(buffer.get(), data + sizeof(PacketHead), head->len);

				packetLen = sizeof(PacketHead) + head->len;

				return Parser::Result::Result_Success;
			}
		}// namespace common
	}// namespace dxg
}// namespace daxia

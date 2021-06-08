#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "parser.h"

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

				if (head.hearbeat != 0)
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

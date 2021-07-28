#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include "session.h"

namespace daxia
{
	namespace net
	{
		Session::Session(common::BasicSession::socket_ptr sock, std::shared_ptr<common::Parser> parser, handler onMessage, long long id)
			: onMessage_(onMessage)
			, id_(id)
		{
			initSocket(sock);
			SetParser(parser);
			postRead();
		}

		Session::~Session()
		{
		}

		long long Session::GetSessionID() const
		{
			return id_;
		}

		void Session::onPacket(const boost::system::error_code& error, int msgId, const common::shared_buffer& buffer)
		{
			if (onMessage_)
			{
				onMessage_(error, id_, msgId, buffer);
			}
		}
	}// namespace net
}// namespace daxia

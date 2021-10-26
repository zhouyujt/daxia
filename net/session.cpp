#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "session.h"

namespace daxia
{
	namespace net
	{
		Session::Session(common::BasicSession::socket_ptr sock, std::shared_ptr<common::Parser> parser, handler onMessage, long long id)
			: id_(id)
			, onMessage_(onMessage)
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

		void Session::onPacket(const boost::system::error_code& error, int msgId, const common::Buffer& buffer)
		{
			if (onMessage_)
			{
				onMessage_(error, id_, msgId, buffer);
			}
		}
	}// namespace net
}// namespace daxia

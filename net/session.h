/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file session.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 */

#ifndef __DAXIA_NET_SERVER_SESSION_H
#define __DAXIA_NET_SERVER_SESSION_H

#include "common/basic_session.h"

namespace daxia
{
	namespace net
	{
		class Router;

		// �Ự��
		class Session : public common::BasicSession
		{
			friend Router;
		public:
			typedef std::shared_ptr<Session> ptr;
			typedef std::function<void(const boost::system::error_code&, long long, int, common::Buffer)> handler;
		protected:
			Session(common::BasicSession::socket_ptr sock, std::shared_ptr<common::Parser> parser, handler onMessage, long long id);
			Session(common::BasicSession::sslsocket_ptr sock, std::shared_ptr<common::Parser> parser, handler onMessage, long long id);
		public:
			~Session();

			// ��ȡ�ͻ���ID
			long long GetSessionID() const;
		protected:
			virtual void onPacket(const boost::system::error_code& error, int msgId, const common::Buffer& buffer) override;
		private:
			long long id_;
			handler onMessage_;
		};
	}// namespace net
}// namespace daxia

#endif	// !__DAXIA_NET_SERVER_SESSION_H
/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file session.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
 * 
 */

#ifndef __DAXIA_DXG_SERVER_SESSION_HPP
#define __DAXIA_DXG_SERVER_SESSION_HPP

#include <memory>
#include <daxia/dxg/common/basic_session.hpp>
#include <daxia/dxg/common/define.hpp>
#include <daxia/dxg/common/shared_buffer.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace server
		{
			class SessionsManager;

			// 会话类
			class Session : public common::BasicSession
			{
				friend SessionsManager;
			public:
				typedef std::shared_ptr<Session> ptr;
				typedef std::function<void(const boost::system::error_code&, long long, int, common::shared_buffer)> handler;
			protected:
				Session(common::BasicSession::socket_ptr sock, common::Parser::ptr parser, handler onMessage, long long id);
			public:
				~Session();

				// 获取客户端ID
				long long GetSessionID() const;
			protected:
				virtual void onPacket(const boost::system::error_code& error, int msgId, const common::shared_buffer& buffer) override;
			private:
				long long id_;
				handler onMessage_;
			};

			//////////////////////////////////////////////////////////////////////////
			inline Session::Session(common::BasicSession::socket_ptr sock, common::Parser::ptr parser, handler onMessage, long long id)
				: onMessage_(onMessage)
				, id_(id)
			{
				initSocket(sock);
				SetParser(parser);
				postRead();
			}

			inline Session::~Session()
			{
			}

			inline long long Session::GetSessionID() const
			{
				return id_;
			}

			inline void Session::onPacket(const boost::system::error_code& error, int msgId, const common::shared_buffer& buffer)
			{
				if (onMessage_)
				{
					onMessage_(error, id_, msgId, buffer);
				}
			}

		}// namespace server
	}// namespace dxg
}// namespace daxia

#endif	// !__DAXIA_DXG_SERVER_SESSION_HPP
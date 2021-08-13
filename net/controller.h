/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file controller.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 */

#ifndef __DAXIA_NET_SERVER_CONTROLLER_H
#define __DAXIA_NET_SERVER_CONTROLLER_H

#include <memory>
#include "common/shared_buffer.h"
#include "common/http_parser.h"
#include "session.h"
#include "sessions_manager.h"
#include "../encode/json.h"

namespace daxia
{
	namespace net
	{
		// 逻辑控制器接口类
		class Controller
		{
		public:
			Controller(){}
			virtual ~Controller(){}
		public:
			virtual void Proc(int msgId, daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data) = 0;
		};

		// HTTP逻辑控制器接口类
		class HttpController
		{
		public:
			HttpController(){}
			virtual ~HttpController(){}
		public:
			virtual void Get(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
			virtual void Post(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
			virtual void Put(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
			virtual void Head(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
			virtual void Delete(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
			virtual void Options(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
			virtual void Trace(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
			virtual void Connect(daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::shared_buffer data){}
		public:
			void SetContext(std::shared_ptr<Session> session);
			void ResetContext();
		public:
			class ContentTypeHelper
			{
			public:
				ContentTypeHelper()
					: json("application/json;charset=gbk")
					, xml("text/xml")
					, stream("application/octet-stream")
				{
				}
				~ContentTypeHelper() {}
			public:
				std::string json;
				std::string xml;
				std::string stream;
			};
			static ContentTypeHelper ContentType;
		protected:
			const common::HttpParser::RequestHeader& Request() const;
			common::HttpParser::ResponseHeader& Response();
		protected:
			void ServeNone(int status);
			template<typename T>
			void ServeJson(const T& v)
			{
				if (!context_.expired())
				{
					Response().StartLine.StatusCode = "200";
					Response().ContentType.Value() = ContentType.json;
					context_.lock()->WriteMessage(daxia::encode::Json::Marshal(v));
				}
			}
		private:
			std::weak_ptr<Session> context_;
		};

	}// namespace net
}// namespace daxia

#endif	// !__DAXIA_NET_SERVER_CONTROLLER_H

/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file controller.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 */

#ifndef __DAXIA_DXG_SERVER_CONTROLLER_H
#define __DAXIA_DXG_SERVER_CONTROLLER_H

#include <memory>
#include "common/shared_buffer.h"
#include "common/http_parser.h"
#include "session.h"
#include "sessions_manager.h"

namespace daxia
{
	namespace dxg
	{
		// �߼��������ӿ���
		class Controller
		{
		public:
			Controller(){}
			virtual ~Controller(){}
		public:
			virtual void Proc(int msgId, Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data) = 0;
		};

		// HTTP�߼��������ӿ���
		class HttpController
		{
		public:
			HttpController(){}
			virtual ~HttpController(){}
		public:
			virtual void Get(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
			virtual void Post(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
			virtual void Put(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
			virtual void Head(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
			virtual void Delete(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
			virtual void Options(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
			virtual void Trace(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
			virtual void Connect(Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data){}
		public:
			common::HttpParser::RequestHeader requestHeader_;
			common::HttpParser::ResponseHeader responserHeader_;
		};

	}// namespace dxg
}// namespace daxia

#endif	// !__DAXIA_DXG_SERVER_CONTROLLER_H

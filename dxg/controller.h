/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file controller.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
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
		// 逻辑控制器接口类
		class Controller
		{
		public:
			Controller(){}
			virtual ~Controller(){}
		public:
			virtual void Proc(int msgId, daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data) = 0;
		};

		// HTTP逻辑控制器接口类
		class HttpController
		{
		public:
			HttpController(){}
			virtual ~HttpController(){}
		public:
			virtual void Get(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
			virtual void Post(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
			virtual void Put(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
			virtual void Head(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
			virtual void Delete(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
			virtual void Options(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
			virtual void Trace(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
			virtual void Connect(daxia::dxg::Session* session, daxia::dxg::SessionsManager* sessionsMgr, const daxia::dxg::common::shared_buffer data){}
		public:
			void InitRequestHeader(const common::HttpParser::RequestHeader& requestHeader){ requestHeader_ = requestHeader; }
		public:
			const common::HttpParser::RequestHeader& Header() const { return requestHeader_; }
		private:
			common::HttpParser::RequestHeader requestHeader_;
		};

	}// namespace dxg
}// namespace daxia

#endif	// !__DAXIA_DXG_SERVER_CONTROLLER_H

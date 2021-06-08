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

#ifndef __DAXIA_DXG_SERVER_CONTROLLER_HPP
#define __DAXIA_DXG_SERVER_CONTROLLER_HPP

#include <daxia/dxg/common/shared_buffer.hpp>
#include <daxia/dxg/server/sessions_manager.hpp>
#include <daxia/dxg/server/session.hpp>
#include <daxia/dxg/server/scheduler.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace server
		{
			class Server;

			// 逻辑控制器接口类
			class Controller
			{
			public:
				typedef std::shared_ptr<Controller> ptr;
			public:
				Controller(){}
				virtual ~Controller(){}
			public:
				virtual void Proc(int msgId, Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data) = 0;
			};

		}// namespace server
	}// namespace dxg
}// namespace daxia

#endif	// !__DAXIA_DXG_SERVER_CONTROLLER_HPP

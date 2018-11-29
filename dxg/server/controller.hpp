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

			// �߼��������ӿ���
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

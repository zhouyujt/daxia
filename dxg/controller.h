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

namespace daxia
{
	namespace dxg
	{
		class Controller;
		class Session;
		class SessionsManager;

		// 逻辑控制器接口类
		class Controller
		{
		public:
			Controller(){}
			virtual ~Controller(){}
		public:
			virtual void Proc(int msgId, Session* session, SessionsManager* sessionsMgr, const common::shared_buffer data) = 0;
		};

	}// namespace dxg
}// namespace daxia

#endif	// !__DAXIA_DXG_SERVER_CONTROLLER_H

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

namespace daxia
{
	namespace dxg
	{
		class Controller;
		class Session;
		class SessionsManager;

		// �߼��������ӿ���
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

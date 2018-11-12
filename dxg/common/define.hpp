/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file define.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 */

#ifndef __DAXIA_DXG_COMMON_DEFINE_HPP
#define __DAXIA_DXG_COMMON_DEFINE_HPP

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			typedef unsigned char byte;

			enum Protocol
			{
				Protocol_TCP,
				Protocol_UDP,
				Protocol_Websocket,
				Protocol_HTTP
			};

			enum DefMsgID
			{
				DefMsgID_Connect = -1,
				DefMsgID_DisConnect = -2,
				DefMsgID_Heartbeat = -3,
				DefMsgID_UnHandle = -4
			};
		}

	}
}


#endif // !__DAXIA_DXG_COMMON_DEFINE_HPP

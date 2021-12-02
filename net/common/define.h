/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file define.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 */

#ifndef __DAXIA_NET_COMMON_DEFINE_H
#define __DAXIA_NET_COMMON_DEFINE_H

namespace daxia
{
	namespace net
	{
		namespace common
		{
			enum Protocol
			{
				Protocol_TCP,
				Protocol_UDP,
				Protocol_HTTP,
				Protocol_HTTPS,
				Protocol_Websocket,
				Protocol_WebsocketSSL
			};

			enum DefMsgID : int
			{
				DefMsgID_Connect = -1,
				DefMsgID_DisConnect = -2,
				DefMsgID_Heartbeat = -3,
				DefMsgID_UnHandle = -4
			};

			enum BuffSize : int
			{
				MaxBufferSize = 1024 * 16
			};

			enum ReserveUserDataIndex
			{
				HttpRequestHeaderIndex = 10,
				HttpResponseHeaderIndex
			};
		}

	}
}


#endif // !__DAXIA_NET_COMMON_DEFINE_H

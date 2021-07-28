/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file define.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
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
			typedef unsigned char byte;

			enum Protocol
			{
				Protocol_TCP,
				Protocol_UDP,
				Protocol_Websocket,
				Protocol_HTTP
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
				MaxBufferSize = 1024 * 32
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

/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file server.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 * 一个通用的服务器框架
 *
 */

#ifndef __DAXIA_NET_SERVER_H
#define __DAXIA_NET_SERVER_H

#include <memory>
#include "common/parser.h"
#include "common/define.h"
#include "controller.h"
#include "scheduler.h"
#include "router.h"

namespace daxia
{
	namespace net
	{
		class Server;

		// 服务器启动需要的额外参数
		class ExtraParam
		{
			friend Server;
		protected:
			ExtraParam() 
				: protocol_(common::Protocol_TCP) 
			{}
			virtual ~ExtraParam(){}
		protected:
			common::Protocol protocol_;
		};

		// HTTP 服务器启动需要的额外参数
		class ExtraParamHttp : public ExtraParam
		{
		public:
			ExtraParamHttp(const char* root)
				: root_ (root)
			{
				ExtraParam::protocol_ = common::Protocol_HTTP;
			}
		public:
			daxia::string root_;
		};

		// HTTPS 服务器启动需要的额外参数
		class ExtraParamHttps : public ExtraParamHttp
		{
		public:
			ExtraParamHttps(const char* root, const char* pubCert, const char* priKey) 
				: ExtraParamHttp(root) 
				, pubCert_(pubCert)
				, priKey_(priKey)
			{
				ExtraParam::protocol_ = common::Protocol_HTTPS;
			}
		public:
			daxia::string pubCert_;
			daxia::string priKey_;
		};

		class Server
		{
		public:
			Server();
			~Server();
		public:
			// 启动服务
			bool Run(short port, const ExtraParam* extraParam = nullptr);
			// 停止服务
			void Stop();
			// 设置协议解析器
			void SetParser(std::shared_ptr<common::Parser> parser);
			// 设置消息处理控制器
			void Handle(int msgId, std::shared_ptr<Controller> controller);
			// 设置消息处理控制器
			void Handle(const char* url, std::shared_ptr<HttpController> controller);
			// 开启心跳检测，当milliseconds为0时，则关闭
			void EnableCheckHeartbeat(unsigned long milliseconds);
			// 设置帧率
			void SetFPS(unsigned long fps);
			long long ScheduleUpdate(Scheduler::scheduleFunc func);
			long long Schedule(Scheduler::scheduleFunc func, unsigned long duration);
			long long ScheduleOnce(Scheduler::scheduleFunc func, unsigned long duration);
			void UnscheduleUpdate(long long scheduleID);
			void Unschedule(long long scheduleID);
			void UnscheduleAll();
			SessionsManager& GetSessionManager();
		private:
			Router router_;
		};
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_SERVER_H
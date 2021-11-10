/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file server.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 * һ��ͨ�õķ��������
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

		// ������������Ҫ�Ķ������
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

		// HTTP ������������Ҫ�Ķ������
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

		// HTTPS ������������Ҫ�Ķ������
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
			// ��������
			bool Run(short port, const ExtraParam* extraParam = nullptr);
			// ֹͣ����
			void Stop();
			// ����Э�������
			void SetParser(std::shared_ptr<common::Parser> parser);
			// ������Ϣ���������
			void Handle(int msgId, std::shared_ptr<Controller> controller);
			// ������Ϣ���������
			void Handle(const char* url, std::shared_ptr<HttpController> controller);
			// ����������⣬��millisecondsΪ0ʱ����ر�
			void EnableCheckHeartbeat(unsigned long milliseconds);
			// ����֡��
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
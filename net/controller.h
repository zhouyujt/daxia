/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file controller.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 */

#ifndef __DAXIA_NET_SERVER_CONTROLLER_H
#define __DAXIA_NET_SERVER_CONTROLLER_H

#include <memory>
#include "common/buffer.h"
#include "common/http_parser.h"
#include "session.h"
#include "sessions_manager.h"
#include "../encode/json.h"

#define DECLARE_OVERRIDE_METHODS() virtual void Proc(int msgId, daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::Buffer& data) override;

#define DECLARE_CONTROLLER(name)\
class name##: public daxia::net::Controller\
{\
public:\
	name##(){}\
	~##name##(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
}

#define DECLARE_CONTROLLER_1(name,type1,var1)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1) : var1##_(var1){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
}

#define DECLARE_CONTROLLER_2(name,type1,var1,type2,var2)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1, type2 var2) : var1##_(var1)\
	, var2##_(var2){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
}

#define DECLARE_CONTROLLER_3(name,type1,var1,type2,var2,type3,var3)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1, type2 var2, type3 var3) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
}

#define DECLARE_CONTROLLER_4(name,type1,var1,type2,var2,type3,var3,type4,var4)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1,type2 var2, type3 var3, type4 var4) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
}

#define DECLARE_CONTROLLER_5(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
}

#define DECLARE_CONTROLLER_6(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5)\
	, var6##_(var6){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
}

#define DECLARE_CONTROLLER_7(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6,type7,var7)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6, type7 var7) : var1_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5)\
	, var6##_(var6)\
	, var7##_(var7){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
	type7 var7##_; \
}

#define DECLARE_CONTROLLER_8(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6,type7,var7,type8,var8)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6, type7 var7, type8 var8) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5)\
	, var6##_(var6)\
	, var7##_(var7)\
	, var8##_(var8){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
	type7 var7##_; \
	type8 var8##_; \
}

#define DECLARE_CONTROLLER_9(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6,type7,var7,type8,var8,type9,var9)\
class name : public daxia::net::Controller\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6, type7 var7, type8 var8, type9 var9) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5)\
	, var6##_(var6)\
	, var7##_(var7)\
	, var8##_(var8)\
	, var9##_(var9){}\
	~name(){}\
public:\
	DECLARE_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
	type7 var7##_; \
	type8 var8##_; \
	type9 var9##_; \
}

#define DECLARE_HTTP_OVERRIDE_METHODS() virtual void InitMethods() override;

#define DECLARE_HTTP_CONTROLLER(name)\
class name : public daxia::net::HttpController\
{\
public:\
	name(){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
}

#define DECLARE_HTTP_CONTROLLER_1(name,type1,var1)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1) : var1##_(var1){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
}

#define DECLARE_HTTP_CONTROLLER_2(name,type1,var1,type2,var2)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2) : var1##_(var1)\
	, var2##_(var2){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
}

#define DECLARE_HTTP_CONTROLLER_3(name,type1,var1,type2,var2,type3,var3)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2, type3 var3) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
}

#define DECLARE_HTTP_CONTROLLER_4(name,type1,var1,type2,var2,type3,var3,type4,var4)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
}

#define DECLARE_HTTP_CONTROLLER_5(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
}

#define DECLARE_HTTP_CONTROLLER_6(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5)\
	, var6##_(var6){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
}

#define DECLARE_HTTP_CONTROLLER_7(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6,type7,var7)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6, type7 var7) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5)\
	, var6##_(var6)\
	, var7##_(var7){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
	type7 var7##_; \
}

#define DECLARE_HTTP_CONTROLLER_8(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6,type7,var7,type8,var8)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6, type7 var7, type8 var8) : var1##_(var1)\
	, var2##_(var2)\
	, var3##_(var3)\
	, var4##_(var4)\
	, var5##_(var5)\
	, var6##_(var6)\
	, var7##_(var7)\
	, var8##_(var8){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
	type7 var7##_; \
	type8 var8##_; \
}

#define DECLARE_HTTP_CONTROLLER_9(name,type1,var1,type2,var2,type3,var3,type4,var4,type5,var5,type6,var6,type7,var7,type8,var8,type9,var9)\
class name : public daxia::net::HttpController\
{\
public:\
	name(type1 var1, type2 var2, type3 var3, type4 var4, type5 var5, type6 var6, type7 var7, type8 var8, type9 var9) : var1##_(var1)\
	, var2_(var2)\
	, var3_(var3)\
	, var4_(var4)\
	, var5_(var5)\
	, var6_(var6)\
	, var7_(var7)\
	, var8_(var8)\
	, var9_(var9){}\
	~name(){}\
public:\
	DECLARE_HTTP_OVERRIDE_METHODS()\
private:\
	type1 var1##_; \
	type2 var2##_; \
	type3 var3##_; \
	type4 var4##_; \
	type5 var5##_; \
	type6 var6##_; \
	type7 var7##_; \
	type8 var8##_; \
	type9 var9##_; \
}

namespace daxia
{
	namespace net
	{
		// 逻辑控制器接口类
		class Controller
		{
		public:
			Controller() : useCoroutine_(true){}
			virtual ~Controller(){}
		public:
			void SetCoroutine(bool useCoroutine){ useCoroutine_ = useCoroutine; }
			bool IsCoroutine() const { return useCoroutine_; }
		public:
			virtual void Proc(int msgId, daxia::net::Session* session, daxia::net::SessionsManager* sessionsMgr, const daxia::net::common::Buffer& data) = 0;
		private:
			bool useCoroutine_;
		};

		// HTTP逻辑控制器接口类
		class HttpController
		{
		public:
			HttpController() : useCoroutine_(true){}
			virtual ~HttpController(){}
		public:
			void SetCoroutine(bool useCoroutine){ useCoroutine_ = useCoroutine; }
			bool IsCoroutine() const { return useCoroutine_; }
		public:
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Get;
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Post;
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Put;
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Head;
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Delete;
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Options;
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Trace;
			std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> Connect;
			virtual void InitMethods() = 0;
		public:
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultGet;
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultPost;
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultPut;
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultHead;
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultDelete;
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultOptions;
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultTrace;
			static std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> DefaultConnect;
			static common::HttpParser::RequestHeader DefaultRequest;
			static common::HttpParser::ResponseHeader DefaultResponser;
		public:
			void SetContext(std::shared_ptr<Session> session);
			void ResetContext();
		protected:
			const common::HttpParser::RequestHeader& Request() const;
			common::HttpParser::ResponseHeader& Response();
		protected:
			void ServeNone(int status);
			template<typename T>
			void ServeJson(const T& v)
			{
				if (!context_.expired())
				{
					Response().StartLine.StatusCode = "200";
					Response().ContentType = MIME_HELPER().Find("json") + ";charset=UTF-8";
					context_.lock()->WriteMessage(0, daxia::encode::Json::Marshal(v));
				}
			}
			void ServeFile(const daxia::string& filename);
		private:
			std::weak_ptr<Session> context_;
			bool useCoroutine_;
		};
#undef HTTP_MIME_MAP

		class DefaultWebsocketControllor : public HttpController
		{
		public:
			DefaultWebsocketControllor();
		public:
			virtual void InitMethods() override;
		private:
			std::shared_ptr<common::Parser> websocketParser_;
		};

	}// namespace net
}// namespace daxia

#endif	// !__DAXIA_NET_SERVER_CONTROLLER_H

#include <fstream>
#include "controller.h"
#include "common/websocket_parser.h"
#include "router.h"

namespace daxia
{
	namespace net
	{
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultGet;
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultPost;
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultPut;
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultHead;
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultDelete;
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultOptions;
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultTrace;
		std::function<void(daxia::net::Session*, daxia::net::SessionsManager*, const daxia::net::common::Buffer&)> HttpController::DefaultConnect;
		common::HttpParser::RequestHeader HttpController::DefaultRequest;
		common::HttpParser::ResponseHeader HttpController::DefaultResponser;

		void HttpController::SetContext(std::shared_ptr<Session> session)
		{
			context_ = session;
		}

		void HttpController::ResetContext()
		{
			context_.reset();
		}

		const daxia::net::common::HttpParser::RequestHeader& HttpController::Request() const
		{
			using common::BasicSession;

			static common::HttpParser::RequestHeader null;

			if (!context_.expired())
			{
				auto* request = context_.lock()->GetUserData<common::HttpParser::RequestHeader>(SESSION_USERDATA_REQUEST_INDEX);
				if (request)
				{
					return *request;
				}
				else
				{
					return null;
				}
			}
			else
			{
				return null;
			}
		}

		daxia::net::common::HttpParser::ResponseHeader& HttpController::Response()
		{
			using common::BasicSession;

			static common::HttpParser::ResponseHeader null;

			if (!context_.expired())
			{
				auto* responser = context_.lock()->GetUserData<common::HttpParser::ResponseHeader>(SESSION_USERDATA_RESPONSE_INDEX);
				if (responser)
				{
					return *responser;
				}
				else
				{
					return null;
				};
			}
			else
			{
				return null;
			}
		}

		void HttpController::ServeNone(int status)
		{
			Response().StartLine.StatusCode.Format("%d", status);

			if (!context_.expired())
			{
				context_.lock()->WriteMessage(0,nullptr,0);
			}
		}

		void HttpController::ServeFile(const daxia::string& filename)
		{
			if (!context_.expired())
			{
				// 获取后缀名
				size_t pos = filename.Find(".");
				if (pos == (size_t)-1)
				{
					ServeNone(404);
					return;
				}

				daxia::string extension = filename.Mid(pos + 1, -1).MakeLower();
				daxia::string type = MIME_HELPER().Find(extension);
				if (type.IsEmpty())
				{
					type = MIME_HELPER().Find("bin");
				}

				Response().StartLine.StatusCode = "200";
				Response().ContentType = type;
				
				std::ifstream ifs;
				ifs.open(filename,std::ios::binary);
				if (ifs.is_open())
				{
					ifs.seekg(0, ifs.end);
					size_t total = static_cast<size_t>(ifs.tellg());
					ifs.seekg(0, ifs.beg);

					common::PageInfo pi;
					pi.total = static_cast<unsigned int>(total);
					pi.startPos = 0;
					pi.endPos = 0;

					daxia::buffer buffer;
					const size_t maxlen = common::MaxBufferSize / 2;
					size_t wrote = 0;

					if (pi.total == 0)
					{
						context_.lock()->WriteMessage(0, buffer, &pi);
					}
					else
					{
						while (wrote < pi.total)
						{
							size_t readlen = pi.total - wrote < maxlen ? pi.total - wrote : maxlen;
							if (ifs.read(buffer.GetBuffer(readlen), readlen))
							{
								buffer.ReSize(ifs.gcount());

								pi.endPos += static_cast<unsigned int>(buffer.GetLength()) - 1;
								context_.lock()->WriteMessage(0, buffer, &pi);
								pi.startPos = pi.endPos + 1;
								++pi.endPos;
								wrote += buffer.GetLength();
							}
							else
							{
								break;
							}
						}
					}

					ifs.close();
				}
				else
				{
					ServeNone(404);
				}
			}
		}

		DefaultWebsocketControllor::DefaultWebsocketControllor()
		{
			websocketParser_ = std::shared_ptr<common::Parser>(new common::WebsocketServerParser);
		}

		void DefaultWebsocketControllor::InitMethods()
		{
			Get = [&](Session* session, SessionsManager* sessionsMgr, const common::Buffer& data)
			{
				if (common::WebsocketServerParser::Handshake(session))
				{
					// 握手成功，更改协议
					session->SetParser(websocketParser_);

					// 转发连接成功消息
					Router* router = dynamic_cast<Router*>(sessionsMgr);
					if (router)
					{
						router->GetScheduler().PushNetRequest(sessionsMgr->GetSession(session->GetSessionID()), daxia::net::common::DefMsgID_Connect, daxia::net::common::Buffer());
					}
				}
				else
				{
					session->Close();
				}
			};
		}

	}// namespace net
}// namespace daxia
#include "controller.h"

namespace daxia
{
	namespace net
	{
		daxia::net::HttpController::ContentTypeHelper HttpController::ContentType;

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
				context_.lock()->WriteMessage(nullptr,0);
			}
		}

	}// namespace net
}// namespace daxia
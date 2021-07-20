#include "http_parser.h"
#include "../../string.hpp"

// 请求行方法
#define Method_GET			"GET"
#define Method_POST			"POST"
#define Method_HEAD			"HEAD"
#define Method_OPTIONS		"OPTIONS"
#define Method_PUT			"PUT"
#define Method_DELETE		"DELETE"
#define Method_TRACE		"TRACE"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			namespace httpdefine
			{
				// 请求行索引
				enum RequstLineIndex : int
				{
					RequstLineIndex_Method = 0,
					RequstLineIndex_Url,
					RequstLineIndex_Version,
					RequstLineIndex_End
				};
			}

			HttpParser::HttpParser()
			{

			}

			HttpParser::~HttpParser()
			{

			}

			size_t HttpParser::GetPacketHeadLen() const
			{
				return 0;
			}

			bool HttpParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

			bool HttpParser::UnmarshalHead(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, size_t& contentLen) const
			{
				typedef daxia::StringA string;
				static const int maxHeadLength = 1024;
				static const char* endFlag = "\r\n";

				string str((const char*)data, len > maxHeadLength ? maxHeadLength : len);

				// 获取请求行
				int endpos = str.Find("\r\n");
				if (endpos == -1)
				{
					if (len >= maxHeadLength)
					{
						// 请求行长度大于maxHeadLength，则解析失败
						return false;
					}
					else
					{
						// 返回继续接收
						contentLen = maxHeadLength;
						return true;
					}
				}
				else
				{
					// 获取请求行各个参数并校验
					int start = 0;
					string requestLine = str.Tokenize(endFlag, start);
					std::vector<string> params;
					requestLine.Split(" ", params);

					// 校验参数个数
					if (params.size() != httpdefine::RequstLineIndex_End)
					{
						return false;
					}

					// 校验方法
					const auto& method = params.front();
					if (method != Method_GET
						&& method != Method_POST
						&& method != Method_HEAD
						&& method != Method_OPTIONS
						&& method != Method_PUT
						&& method != Method_DELETE
						&& method != Method_TRACE
						)
					{
						return false;
					}
				}

				return true;
			}

			bool HttpParser::UnmarshalContent(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer) const
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

		}
	}
}

#undef Method_GET
#undef Method_POST
#undef Method_HEAD
#undef Method_OPTIONS
#undef Method_PUT
#undef Method_DELETE
#undef Method_TRACE
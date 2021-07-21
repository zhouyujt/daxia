#include "http_parser.h"
#include "../../string.hpp"
#include "../../encode/json.h"

using daxia::encode::Json;

#define LIMIT_START_LINE_SIZE	1024
#define LIMIT_HEAD_SIZE			8192

#ifndef MAX
#define MAX(a,b) a > b ? a : b
#endif

#ifndef MIN
#define MIN(a,b) a < b ? a : b
#endif

#define STRLEN_INT(s) static_cast<int>(strlen(s))

#define CRLF				"\r\n"
#define CRLFCRLF			"\r\n\r\n"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			HttpParser::Methods HttpParser::methodsHelp_;
			HttpParser::HeaderHelp HttpParser::headerHelp_;

			HttpParser::HttpParser()
			{

			}

			HttpParser::~HttpParser()
			{

			}

			bool HttpParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const
			{
				daxia::string msg;
				msg += "HTTP/1.1 200 ok\r\n";
				msg += "Content-Length:";
				msg.Format("%s%d\r\n", msg.GetString(), len);
				msg += "\r\n";
				msg += daxia::string(reinterpret_cast<const char*>(data), len);
				buffer.resize(msg.GetLength());
				memcpy(buffer.get(), msg, msg.GetLength());

				return true;
			}

			Parser::Result HttpParser::Unmarshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer, int& packetLen) const
			{
				daxia::string header((const char*)data, MIN(len, LIMIT_START_LINE_SIZE));

				// 获取起始行结束位置
				int startLineEndPos = header.Find(CRLF);
				if (startLineEndPos == -1)
				{
					if (len >= LIMIT_START_LINE_SIZE)
					{
						// 请求行长度大于起始行大小限制，则解析失败
						return Parser::Result::Result_Fail;
					}
					else
					{
						// 返回继续接收
						return Parser::Result::Result_Uncomplete;
					}
				}

				// 获取起始行各个参数并校验
				daxia::string stratLine = header.Left(startLineEndPos);
				std::vector<daxia::string> params;
				stratLine.Split(" ", params);

				// 校验参数个数
				if (params.size() != RequstLineIndex_End && params.size() != ResponseLineIndex_End) return Parser::Result::Result_Fail;

				// 确定是请求头还是响应头
				bool isRequest = true;
				if (stratLine.Left(4).CompareNoCase("HTTP") == 0)
				{
					isRequest = false;
				}

				if (isRequest)
				{
					// 校验方法是否合法
					if (!methodsHelp_.IsValidMethod(params.front())) return Parser::Result::Result_Fail;
				}

				// 获取整个头
				int headerEndPos = header.Find(CRLFCRLF, startLineEndPos + STRLEN_INT(CRLF));
				if (headerEndPos == -1)
				{
					if (len >= LIMIT_HEAD_SIZE)
					{
						// 请求头长度大于消息头大小限制，则解析失败
						return Parser::Result::Result_Fail;
					}
					else
					{
						// 返回继续接收
						return Parser::Result::Result_Uncomplete;
					}
				}

				packetLen = headerEndPos + STRLEN_INT(CRLFCRLF);

				// 获取Content-Length
				daxia::string ContentLengtTag = headerHelp_.request_.ContentLength.Tag("http");
				ContentLengtTag.MakeLower();
				int lastLineEndPos = startLineEndPos;
				int lineEndPos = -1;
				while ((lineEndPos = header.Find(CRLF, lastLineEndPos + STRLEN_INT(CRLF))) != -1)
				{
					daxia::string line = header.Mid(lastLineEndPos + STRLEN_INT(CRLF), lineEndPos - lastLineEndPos - STRLEN_INT(CRLF));
					int pos = 0;

					if (line.Tokenize(":", pos).MakeLower() == ContentLengtTag)
					{
						packetLen += atoi(line.Tokenize(":", pos));
						break;
					}

					lastLineEndPos = lineEndPos;
				}

				// 数据不足
				if (len < packetLen)  return Parser::Result::Result_Uncomplete;

				// 构造消息ID
				if (isRequest)
				{
					msgID = static_cast<int>(params[0].MakeLower().Hash());
				}
				else
				{
					msgID = atoi(params[ResponseLineIndex_StatusCode]);
				}
				
				// 构造消息
				buffer.resize(packetLen);
				memcpy(buffer.get(), data, packetLen);

				return Parser::Result::Result_Success;
			}

			daxia::reflect::String* HttpParser::GeneralHeader::Find(const daxia::string& key) const
			{
				daxia::reflect::String* str = nullptr;
				auto iter = index_.find(key);
				if (iter != index_.end())
				{
					str = iter->second;
				}

				return str;
			}

			int HttpParser::GeneralHeader::InitFromData(const void* data, int len, bool isRequest)
			{
				daxia::string header(reinterpret_cast<const char*>(data), len);

				// 获取起始行结束位置
				int startLineEndPos = header.Find(CRLF);
				if (startLineEndPos == -1) return -1;

				// 获取起始行各个参数并校验
				daxia::string stratLine = header.Left(startLineEndPos);
				std::vector<daxia::string> params;
				stratLine.Split(" ", params);

				// 校验参数个数
				if (params.size() != RequstLineIndex_End && params.size() != ResponseLineIndex_End) return -1;

				// 获取整个头
				int headerEndPos = header.Find(CRLFCRLF, startLineEndPos + STRLEN_INT(CRLF));
				if (headerEndPos == -1) return -1;

				StartLine = params;

				// 获取所有请求头信息
				int lastLineEndPos = startLineEndPos;
				int lineEndPos = -1;
				while ((lineEndPos = header.Find(CRLF, lastLineEndPos + STRLEN_INT(CRLF))) != -1)
				{
					daxia::string line = header.Mid(lastLineEndPos + STRLEN_INT(CRLF), lineEndPos - lastLineEndPos - STRLEN_INT(CRLF));
					int pos = 0;
					reflect::String* address = nullptr;
					if (isRequest)
					{
						address = headerHelp_.request_.Find(line.Tokenize(":", pos).MakeLower());
					}
					else
					{
						address = headerHelp_.response_.Find(line.Tokenize(":", pos).MakeLower());
					}

					if (address)
					{
						size_t offset = (size_t)address - (size_t)&headerHelp_.request_;
						address = (reflect::String*)((size_t)this + offset);
						*address = line.Mid(pos, -1);
					}

					lastLineEndPos = lineEndPos;
				}

				int packetLen = headerEndPos + STRLEN_INT(CRLFCRLF);

				return packetLen;
			}

			int HttpParser::RequestHeader::InitFromData(const void* data, int len)
			{
				return GeneralHeader::InitFromData(data, len, true);
			}

			int HttpParser::ResponseHeader::InitFromData(const void* data, int len)
			{
				return GeneralHeader::InitFromData(data, len, false);
			}

		}
	}
}
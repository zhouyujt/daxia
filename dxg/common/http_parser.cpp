#include "http_parser.h"
#include "basic_session.h"
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
#define OFFSET "offset"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			HttpParser::Methods HttpParser::methodsHelp_;
			HttpParser::HeaderHelp HttpParser::headerHelp_;

			daxia::reflect::String* HttpParser::GeneralHeader::Find(const daxia::string& key,const void* base) const
			{
				const daxia::reflect::String* str = nullptr;
				auto iter = index_.find(key);
				if (iter != index_.end())
				{
					str = reinterpret_cast<const daxia::reflect::String*>(reinterpret_cast<const char*>(base) + iter->second);
				}

				return const_cast<daxia::reflect::String*>(str);
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
						address = headerHelp_.request_.Value().Find(line.Tokenize(":", pos).MakeLower(),this);
					}
					else
					{
						address = headerHelp_.response_.Value().Find(line.Tokenize(":", pos).MakeLower(),this);
					}

					if (address)
					{
						*address = line.Mid(pos, -1);
					}

					lastLineEndPos = lineEndPos;
				}

				int packetLen = headerEndPos + STRLEN_INT(CRLFCRLF);

				return packetLen;
			}

			int HttpParser::RequestHeader::InitFromData(const void* data, int len)
			{
				int packetLen = GeneralHeader::InitFromData(data, len, true);

				if (packetLen != -1)
				{
					StartLine.Method = GeneralHeader::StartLine[RequstLineIndex_Method];
					int pos = 0;
					StartLine.Url = GeneralHeader::StartLine[RequstLineIndex_Url].Tokenize("?",pos);
					StartLine.Version = GeneralHeader::StartLine[RequstLineIndex_Version];

					// 获取参数
					daxia::string params = GeneralHeader::StartLine[RequstLineIndex_Url].Mid(pos, -1);
					std::vector<daxia::string> key_value;
					params.Split("&", key_value);
					for each (const daxia::string& kv in key_value)
					{
						int pos = 0;
						auto key = kv.Tokenize("=", pos).MakeLower();
						StartLine.Params[key] = kv.Mid(pos, -1);
					}
				}

				return packetLen;
			}

			int HttpParser::ResponseHeader::InitFromData(const void* data, int len)
			{
				int packetLen = GeneralHeader::InitFromData(data, len, false);

				if (packetLen != -1)
				{
					StartLine.Version = GeneralHeader::StartLine[ResponseLineIndex_Version];
					StartLine.StatusCode = GeneralHeader::StartLine[ResponseLineIndex_StatusCode];
					StartLine.StatusText = GeneralHeader::StartLine[ResponseLineIndex_StatusText];
				}

				return packetLen;
			}

			bool HttpServerParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const
			{
				auto request = session->GetUserData<RequestHeader>(SESSION_USERDATA_REQUEST_INDEX);
				auto response = session->GetUserData<ResponseHeader>(SESSION_USERDATA_RESPONSE_INDEX);
				if (request == nullptr || response == nullptr) return false;
				
				// 不写响应头(分段数据)
				if (response->isNoHeader)
				{
					buffer.resize(len);
					memcpy(buffer.get(), data, len);
					return true;
				}

				daxia::string msg;

				// 设置起始行
				response->StartLine.Version = request->StartLine.Version;
				if (response->StartLine.StatusCode.IsEmpty()) response->StartLine.StatusCode = "200";
				auto iter = headerHelp_.status_.find(atoi(response->StartLine.StatusCode.GetString()));
				if (iter != headerHelp_.status_.end()) response->StartLine.StatusText = iter->second;
				msg.Format("%s %s %s", response->StartLine.Version.GetString(), response->StartLine.StatusCode.GetString(), response->StartLine.StatusText.GetString());
				msg += CRLF;

				// 设置Content-Length
				if (daxia::string(response->ContentLength.Value()).NumericCast<int>() == 0)
				{
					if (len) response->ContentLength.Value() = daxia::string::ToString(len);
				}

				// 设置Server
				if (response->Server.Value().empty()) response->Server = "powered by dxg";

				// 设置所有响应头
				auto layout = headerHelp_.response_.Layout();
				for (auto iter = layout.begin(); iter != layout.end(); ++iter)
				{
					unsigned long offset = iter->second.get<unsigned long>(OFFSET, 0);
					const reflect::String* field = nullptr;
					try{ field = dynamic_cast<const reflect::String*>(reinterpret_cast<const reflect::Reflect_base*>(reinterpret_cast<const char*>(response)+offset)); }
					catch (const std::exception&){}
					if (field == nullptr) continue;

					auto test = field->Tag("http");

					if (!field->Value().empty())
					{
						daxia::string temp;
						temp.Format("%s:%s", field->Tag("http").c_str(), field->Value().c_str());
						msg += temp;
						msg += CRLF;
					}
				}

				// 设置头结束符号
				msg += CRLF;

				// 设置content
				msg.Append(reinterpret_cast<const char*>(data), len);

				buffer.resize(msg.GetLength());
				memcpy(buffer.get(), msg, msg.GetLength());
				return true;
			}

			daxia::dxg::common::Parser::Result HttpServerParser::Unmarshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer, int& packetLen) const
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
				if (params.size() != RequstLineIndex_End) return Parser::Result::Result_Fail;

				// 校验方法是否合法
				if (!methodsHelp_.IsValidMethod(params.front())) return Parser::Result::Result_Fail;

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
				daxia::string ContentLengtTag = headerHelp_.request_.Value().ContentLength.Tag("http");
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

				msgID = static_cast<int>(params[0].MakeLower().Hash());

				// 构造消息
				buffer.resize(packetLen);
				memcpy(buffer.get(), data, packetLen);

				return Parser::Result::Result_Success;
			}

			bool HttpClientParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const
			{
				throw "尚未实现";
			}

			daxia::dxg::common::Parser::Result HttpClientParser::Unmarshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer, int& packetLen) const
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
				if ( params.size() != ResponseLineIndex_End) return Parser::Result::Result_Fail;

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
				daxia::string ContentLengtTag = headerHelp_.request_.Value().ContentLength.Tag("http");
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

				msgID = atoi(params[ResponseLineIndex_StatusCode]);

				// 构造消息
				buffer.resize(packetLen);
				memcpy(buffer.get(), data, packetLen);

				return Parser::Result::Result_Success;
			}

		}
	}
}
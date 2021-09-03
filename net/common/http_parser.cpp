#include "http_parser.h"
#include "basic_session.h"
#include "../../string.hpp"
#include "../../encode/json.h"
#include "../../singleton.hpp"
#include "../../encode/url.h"

using daxia::encode::Json;

#define LIMIT_START_LINE_SIZE	1024
#define LIMIT_HEAD_SIZE			8192

#ifndef MAX
#define MAX(a,b) a > b ? a : b
#endif

#ifndef MIN
#define MIN(a,b) a < b ? a : b
#endif

#define CRLF				"\r\n"
#define CRLFCRLF			"\r\n\r\n"
#define OFFSET "offset"

namespace daxia
{
	namespace net
	{
		using namespace reflect;

		namespace common
		{
			ref_string* HttpParser::GeneralHeader::Find(const daxia::string& key,const void* base) const
			{
				const ref_string* str = nullptr;
				auto iter = index_.find(key);
				if (iter != index_.end())
				{
					str = reinterpret_cast<const ref_string*>(reinterpret_cast<const char*>(base) + iter->second);
				}

				return const_cast<ref_string*>(str);
			}

			size_t HttpParser::GeneralHeader::InitFromData(const void* data, size_t len, bool isRequest)
			{
				daxia::string header(reinterpret_cast<const char*>(data), len);

				// 获取起始行结束位置
				size_t startLineEndPos = header.Find(CRLF);
				if (startLineEndPos == -1) return -1;

				// 获取起始行各个参数并校验
				daxia::string startLine = header.Left(startLineEndPos);
				startLine = daxia::encode::Url::Unmarshal(startLine);
				std::vector<daxia::string> params;
				startLine.Split(" ", params);

				// 校验参数个数
				if (params.size() != RequstLineIndex_End && params.size() != ResponseLineIndex_End) return -1;

				// 获取整个头
				size_t headerEndPos = header.Find(CRLFCRLF, startLineEndPos + strlen(CRLF));
				if (headerEndPos == -1) return -1;

				StartLine = params;

				// 获取所有请求头信息
				size_t lastLineEndPos = startLineEndPos;
				size_t lineEndPos = -1;
				while ((lineEndPos = header.Find(CRLF, lastLineEndPos + strlen(CRLF))) != -1)
				{
					daxia::string line = header.Mid(lastLineEndPos + strlen(CRLF), lineEndPos - lastLineEndPos - strlen(CRLF));
					size_t pos = 0;
					ref_string* address = nullptr;
					if (isRequest)
					{
						address = static_cast<RequestHeader&>(HEADER_HELPER().request_).Find(line.Tokenize(":", pos).MakeLower(), this);
					}
					else
					{
						address = static_cast<ResponseHeader&>(HEADER_HELPER().response_).Find(line.Tokenize(":", pos).MakeLower(), this);
					}

					if (address)
					{
						*address = line.Mid(pos, -1).Trim();
					}

					lastLineEndPos = lineEndPos;
				}

				size_t packetLen = headerEndPos + strlen(CRLFCRLF);

				return packetLen;
			}

			size_t HttpParser::RequestHeader::InitFromData(const void* data, size_t len)
			{
				size_t packetLen = GeneralHeader::InitFromData(data, len, true);

				if (packetLen != -1)
				{
					StartLine.Method = GeneralHeader::StartLine[RequstLineIndex_Method];
					size_t pos = 0;
					StartLine.Url = GeneralHeader::StartLine[RequstLineIndex_Url].Tokenize("?",pos);
					StartLine.Version = GeneralHeader::StartLine[RequstLineIndex_Version];

					// 获取参数
					daxia::string params = GeneralHeader::StartLine[RequstLineIndex_Url].Mid(pos, -1);
					std::vector<daxia::string> key_value;
					params.Split("&", key_value);
					for(const daxia::string& kv : key_value)
					{
						size_t pos = 0;
						auto key = kv.Tokenize("=", pos);
						StartLine.Params[key] = kv.Mid(pos, -1);
					}
				}

				return packetLen;
			}

			size_t HttpParser::ResponseHeader::InitFromData(const void* data, size_t len)
			{
				size_t packetLen = GeneralHeader::InitFromData(data, len, false);

				if (packetLen != -1)
				{
					StartLine.Version = GeneralHeader::StartLine[ResponseLineIndex_Version];
					StartLine.StatusCode = GeneralHeader::StartLine[ResponseLineIndex_StatusCode];
					StartLine.StatusText = GeneralHeader::StartLine[ResponseLineIndex_StatusText];
				}

				return packetLen;
			}

			bool HttpServerParser::Marshal(daxia::net::common::BasicSession* session, int msgId, const void* data, size_t len, std::vector<daxia::net::common::Buffer>& buffers) const
			{
				//auto request = session->GetUserData<RequestHeader>(SESSION_USERDATA_REQUEST_INDEX);
				//auto response = session->GetUserData<ResponseHeader>(SESSION_USERDATA_RESPONSE_INDEX);
				//if (request == nullptr || response == nullptr) return false;
				//
				//// 不写响应头(分段数据)
				//if (response->isNoHeader)
				//{
				//	buffer.Resize(len);
				//	memcpy(buffer, data, len);
				//	return true;
				//}

				//daxia::string msg;

				//// 设置起始行
				//response->StartLine.Version = request->StartLine.Version;
				//if (response->StartLine.StatusCode.IsEmpty()) response->StartLine.StatusCode = "200";
				//auto iter = HEADER_HELPER().status_.find(atoi(response->StartLine.StatusCode.GetString()));
				//if (iter != HEADER_HELPER().status_.end()) response->StartLine.StatusText = iter->second;
				//msg.Format("%s %s %s", response->StartLine.Version.GetString(), response->StartLine.StatusCode.GetString(), response->StartLine.StatusText.GetString());
				//msg += CRLF;

				//// 设置Content-Length
				//if (daxia::string(response->ContentLength).NumericCast<int>() == 0)
				//{
				//	response->ContentLength = daxia::string::ToString(len);
				//}

				//// 设置Server
				//if (response->Server->IsEmpty()) response->Server = "powered by daxia";

				//// 设置所有响应头
				//auto layout = HEADER_HELPER().response_.GetLayoutFast();
				//for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
				//{
				//	const ref_string* field = nullptr;
				//	try{ field = dynamic_cast<const ref_string*>(reinterpret_cast<const reflect::Reflect_base*>(reinterpret_cast<const char*>(response)+iter->offset)); }
				//	catch (const std::exception&){}
				//	if (field == nullptr) continue;

				//	if (!(*field)->IsEmpty())
				//	{
				//		daxia::string temp;
				//		temp.Format("%s:%s", field->Tag("http").GetString(), static_cast<daxia::string>(*field).GetString());
				//		msg += temp;
				//		msg += CRLF;
				//	}
				//}

				//// 设置头结束符号
				//msg += CRLF;

				//// 设置content
				//msg.Append(static_cast<const char*>(data), len);

				//buffer.Resize(msg.GetLength());
				//memcpy(buffer, msg.GetString(), msg.GetLength());
				return true;
			}

			daxia::net::common::Parser::Result HttpServerParser::Unmarshal(daxia::net::common::BasicSession* session, const void* data, size_t len, int& msgID, daxia::net::common::Buffer& buffer, size_t& packetLen) const
			{
				daxia::string header((const char*)data, MIN(len, LIMIT_START_LINE_SIZE));

				// 获取起始行结束位置
				size_t startLineEndPos = header.Find(CRLF);
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
				if (!daxia::Singleton<HttpParser::Methods>::Instance().IsValidMethod(params.front().GetString())) return Parser::Result::Result_Fail;

				// 获取整个头
				size_t headerEndPos = header.Find(CRLFCRLF, startLineEndPos + strlen(CRLF));
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

				packetLen = headerEndPos + strlen(CRLFCRLF);

				// 获取Content-Length
				daxia::string ContentLengtTag = static_cast<RequestHeader&>(HEADER_HELPER().request_).ContentLength.Tag("http");
				ContentLengtTag.MakeLower();
				size_t lastLineEndPos = startLineEndPos;
				size_t lineEndPos = -1;
				while ((lineEndPos = header.Find(CRLF, lastLineEndPos + strlen(CRLF))) != -1)
				{
					daxia::string line = header.Mid(lastLineEndPos + strlen(CRLF), lineEndPos - lastLineEndPos - strlen(CRLF));
					size_t pos = 0;

					if (line.Tokenize(":", pos).MakeLower() == ContentLengtTag)
					{
						packetLen += line.Tokenize(":", pos).NumericCast<int>();
						break;
					}

					lastLineEndPos = lineEndPos;
				}

				// 数据不足
				if (len < packetLen)  return Parser::Result::Result_Uncomplete;

				msgID = static_cast<int>(params[0].MakeLower().Hash());

				// 构造消息
				buffer.Resize(packetLen);
				memcpy(buffer, data, packetLen);

				return Parser::Result::Result_Success;
			}

			bool HttpClientParser::Marshal(daxia::net::common::BasicSession* session, int msgId, const void* data, size_t len, std::vector<daxia::net::common::Buffer>& buffers) const
			{
				throw "尚未实现";
			}

			daxia::net::common::Parser::Result HttpClientParser::Unmarshal(daxia::net::common::BasicSession* session, const void* data, size_t len, int& msgID, daxia::net::common::Buffer& buffer, size_t& packetLen) const
			{
				daxia::string header((const char*)data, MIN(len, LIMIT_START_LINE_SIZE));

				// 获取起始行结束位置
				size_t startLineEndPos = header.Find(CRLF);
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
				size_t headerEndPos = header.Find(CRLFCRLF, startLineEndPos + strlen(CRLF));
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

				packetLen = headerEndPos + strlen(CRLFCRLF);

				// 获取Content-Length
				daxia::string ContentLengtTag = static_cast<RequestHeader&>(HEADER_HELPER().request_).ContentLength.Tag("http");
				ContentLengtTag.MakeLower();
				size_t lastLineEndPos = startLineEndPos;
				size_t lineEndPos = -1;
				while ((lineEndPos = header.Find(CRLF, lastLineEndPos + strlen(CRLF))) != -1)
				{
					daxia::string line = header.Mid(lastLineEndPos + strlen(CRLF), lineEndPos - lastLineEndPos - strlen(CRLF));
					size_t pos = 0;

					if (line.Tokenize(":", pos).MakeLower() == ContentLengtTag)
					{
						packetLen += line.Tokenize(":", pos).NumericCast<int>();
						break;
					}

					lastLineEndPos = lineEndPos;
				}

				// 数据不足
				if (len < packetLen)  return Parser::Result::Result_Uncomplete;

				msgID = params[ResponseLineIndex_StatusCode].NumericCast<int>();

				// 构造消息
				buffer.Resize(packetLen);
				memcpy(buffer, data, packetLen);

				return Parser::Result::Result_Success;
			}

		}
	}
}
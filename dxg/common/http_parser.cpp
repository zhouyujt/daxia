#include "http_parser.h"
#include "basic_session.h"
#include "../../string.hpp"
#include "../../encode/json.h"
#include "../../singleton.hpp"

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
	namespace dxg
	{
		namespace common
		{
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

			size_t HttpParser::GeneralHeader::InitFromData(const void* data, size_t len, bool isRequest)
			{
				daxia::string header(reinterpret_cast<const char*>(data), len);

				// ��ȡ��ʼ�н���λ��
				size_t startLineEndPos = header.Find(CRLF);
				if (startLineEndPos == -1) return -1;

				// ��ȡ��ʼ�и���������У��
				daxia::string stratLine = header.Left(startLineEndPos);
				std::vector<daxia::string> params;
				stratLine.Split(" ", params);

				// У���������
				if (params.size() != RequstLineIndex_End && params.size() != ResponseLineIndex_End) return -1;

				// ��ȡ����ͷ
				size_t headerEndPos = header.Find(CRLFCRLF, startLineEndPos + strlen(CRLF));
				if (headerEndPos == -1) return -1;

				StartLine = params;

				// ��ȡ��������ͷ��Ϣ
				size_t lastLineEndPos = startLineEndPos;
				size_t lineEndPos = -1;
				while ((lineEndPos = header.Find(CRLF, lastLineEndPos + strlen(CRLF))) != -1)
				{
					daxia::string line = header.Mid(lastLineEndPos + strlen(CRLF), lineEndPos - lastLineEndPos - strlen(CRLF));
					size_t pos = 0;
					reflect::String* address = nullptr;
					if (isRequest)
					{
						address = daxia::Singleton<HttpParser::HeaderHelp>::Instance().request_.Value().Find(line.Tokenize(":", pos).MakeLower(),this);
					}
					else
					{
						address = daxia::Singleton<HttpParser::HeaderHelp>::Instance().response_.Value().Find(line.Tokenize(":", pos).MakeLower(), this);
					}

					if (address)
					{
						address->Value() = line.Mid(pos, -1);
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

					// ��ȡ����
					daxia::string params = GeneralHeader::StartLine[RequstLineIndex_Url].Mid(pos, -1);
					std::vector<daxia::string> key_value;
					params.Split("&", key_value);
					for(const daxia::string& kv : key_value)
					{
						size_t pos = 0;
						auto key = kv.Tokenize("=", pos).MakeLower();
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

			bool HttpServerParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, size_t len, daxia::dxg::common::shared_buffer& buffer) const
			{
				auto request = session->GetUserData<RequestHeader>(SESSION_USERDATA_REQUEST_INDEX);
				auto response = session->GetUserData<ResponseHeader>(SESSION_USERDATA_RESPONSE_INDEX);
				if (request == nullptr || response == nullptr) return false;
				
				// ��д��Ӧͷ(�ֶ�����)
				if (response->isNoHeader)
				{
					buffer.resize(len);
					memcpy(buffer.get(), data, len);
					return true;
				}

				daxia::string msg;

				// ������ʼ��
				response->StartLine.Version = request->StartLine.Version;
				if (response->StartLine.StatusCode.IsEmpty()) response->StartLine.StatusCode = "200";
				auto iter = daxia::Singleton<HttpParser::HeaderHelp>::Instance().status_.find(atoi(response->StartLine.StatusCode.GetString()));
				if (iter != daxia::Singleton<HttpParser::HeaderHelp>::Instance().status_.end()) response->StartLine.StatusText = iter->second;
				msg.Format("%s %s %s", response->StartLine.Version.GetString(), response->StartLine.StatusCode.GetString(), response->StartLine.StatusText.GetString());
				msg += CRLF;

				// ����Content-Length
				if (daxia::string(response->ContentLength.Value()).NumericCast<int>() == 0)
				{
					if (len) response->ContentLength.Value() = daxia::string::ToString(len);
				}

				// ����Server
				if (response->Server.Value().empty()) response->Server.Value() = "powered by dxg";

				// ����������Ӧͷ
				auto layout = daxia::Singleton<HttpParser::HeaderHelp>::Instance().response_.Layout();
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

				// ����ͷ��������
				msg += CRLF;

				// ����content
				msg.Append(reinterpret_cast<const char*>(data), len);

				buffer.resize(msg.GetLength());
				memcpy(buffer.get(), msg.GetString(), msg.GetLength());
				return true;
			}

			daxia::dxg::common::Parser::Result HttpServerParser::Unmarshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, size_t len, int& msgID, daxia::dxg::common::shared_buffer& buffer, size_t& packetLen) const
			{
				daxia::string header((const char*)data, MIN(len, LIMIT_START_LINE_SIZE));

				// ��ȡ��ʼ�н���λ��
				size_t startLineEndPos = header.Find(CRLF);
				if (startLineEndPos == -1)
				{
					if (len >= LIMIT_START_LINE_SIZE)
					{
						// �����г��ȴ�����ʼ�д�С���ƣ������ʧ��
						return Parser::Result::Result_Fail;
					}
					else
					{
						// ���ؼ�������
						return Parser::Result::Result_Uncomplete;
					}
				}

				// ��ȡ��ʼ�и���������У��
				daxia::string stratLine = header.Left(startLineEndPos);
				std::vector<daxia::string> params;
				stratLine.Split(" ", params);

				// У���������
				if (params.size() != RequstLineIndex_End) return Parser::Result::Result_Fail;

				// У�鷽���Ƿ�Ϸ�
				if (!daxia::Singleton<HttpParser::Methods>::Instance().IsValidMethod(params.front().GetString())) return Parser::Result::Result_Fail;

				// ��ȡ����ͷ
				size_t headerEndPos = header.Find(CRLFCRLF, startLineEndPos + strlen(CRLF));
				if (headerEndPos == -1)
				{
					if (len >= LIMIT_HEAD_SIZE)
					{
						// ����ͷ���ȴ�����Ϣͷ��С���ƣ������ʧ��
						return Parser::Result::Result_Fail;
					}
					else
					{
						// ���ؼ�������
						return Parser::Result::Result_Uncomplete;
					}
				}

				packetLen = headerEndPos + strlen(CRLFCRLF);

				// ��ȡContent-Length
				daxia::string ContentLengtTag = daxia::Singleton<HttpParser::HeaderHelp>::Instance().request_.Value().ContentLength.Tag("http");
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

				// ���ݲ���
				if (len < packetLen)  return Parser::Result::Result_Uncomplete;

				msgID = static_cast<int>(params[0].MakeLower().Hash());

				// ������Ϣ
				buffer.resize(packetLen);
				memcpy(buffer.get(), data, packetLen);

				return Parser::Result::Result_Success;
			}

			bool HttpClientParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, size_t len, daxia::dxg::common::shared_buffer& buffer) const
			{
				throw "��δʵ��";
			}

			daxia::dxg::common::Parser::Result HttpClientParser::Unmarshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, size_t len, int& msgID, daxia::dxg::common::shared_buffer& buffer, size_t& packetLen) const
			{
				daxia::string header((const char*)data, MIN(len, LIMIT_START_LINE_SIZE));

				// ��ȡ��ʼ�н���λ��
				size_t startLineEndPos = header.Find(CRLF);
				if (startLineEndPos == -1)
				{
					if (len >= LIMIT_START_LINE_SIZE)
					{
						// �����г��ȴ�����ʼ�д�С���ƣ������ʧ��
						return Parser::Result::Result_Fail;
					}
					else
					{
						// ���ؼ�������
						return Parser::Result::Result_Uncomplete;
					}
				}

				// ��ȡ��ʼ�и���������У��
				daxia::string stratLine = header.Left(startLineEndPos);
				std::vector<daxia::string> params;
				stratLine.Split(" ", params);

				// У���������
				if ( params.size() != ResponseLineIndex_End) return Parser::Result::Result_Fail;

				// ��ȡ����ͷ
				size_t headerEndPos = header.Find(CRLFCRLF, startLineEndPos + strlen(CRLF));
				if (headerEndPos == -1)
				{
					if (len >= LIMIT_HEAD_SIZE)
					{
						// ����ͷ���ȴ�����Ϣͷ��С���ƣ������ʧ��
						return Parser::Result::Result_Fail;
					}
					else
					{
						// ���ؼ�������
						return Parser::Result::Result_Uncomplete;
					}
				}

				packetLen = headerEndPos + strlen(CRLFCRLF);

				// ��ȡContent-Length
				daxia::string ContentLengtTag = daxia::Singleton<HttpParser::HeaderHelp>::Instance().request_.Value().ContentLength.Tag("http");
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

				// ���ݲ���
				if (len < packetLen)  return Parser::Result::Result_Uncomplete;

				msgID = params[ResponseLineIndex_StatusCode].NumericCast<int>();

				// ������Ϣ
				buffer.resize(packetLen);
				memcpy(buffer.get(), data, packetLen);

				return Parser::Result::Result_Success;
			}

		}
	}
}
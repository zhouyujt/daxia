#include "websocket_parser.h"
#include "basic_session.h"
#include "../../encode/sha1.h"
#include "../../encode/base64.h"
#include "../../encode/hex.h"
#include "byte_order.hpp"

// val转为保留width位的位域
#define CASTBIT(val,width) (val & ((1<<width) - 1))

namespace daxia
{
	namespace net
	{
		namespace common
		{

			WebsocketServerParser::WebsocketServerParser()
			{

			}

			WebsocketServerParser::~WebsocketServerParser()
			{

			}

			bool WebsocketServerParser::Marshal(daxia::net::common::BasicSession* session, int msgId, const void* data, size_t len, const daxia::net::common::PageInfo* pageInfo, std::vector<daxia::net::common::Buffer>& buffers, size_t maxPacketLength) const
			{
#ifndef MIN
#define MIN(x,y) x < y ? x : y
#endif
				size_t headLen = 0;
				if (len < 126)
				{
					headLen = sizeof(WebsocketHeader);
				}
				else if (len >= 126 && len <= 0xffff)
				{
					headLen = sizeof(WebsocketHeader16);
				}
				else
				{
					// 本解析器不允许超过0xffff大小的包，使用WebsocketHeader16进行强制分段
					//headLen = sizeof(WebsocketHeader64);
					headLen = sizeof(WebsocketHeader16);
				}

				WebsocketHeader16 head;

				const unsigned int maxContentLength = static_cast<unsigned int>(maxPacketLength - headLen);
				for (unsigned int offset = 0; offset < static_cast<unsigned int>(len); offset += maxContentLength)
				{
					Buffer buffer;
					unsigned int contentLength = MIN(static_cast<unsigned int>(len)-offset, maxContentLength);
					buffer.Resize(contentLength + headLen);
					if (pageInfo == nullptr)
					{
						head.fin = offset + contentLength == static_cast<unsigned int>(len) ? 1 : 0;
						head.op = offset == 0 ? CASTBIT(msgId, 4) : 0;
					}
					else
					{
						head.fin = pageInfo->IsEnd() ? 1 : 0;
						head.op = pageInfo->IsStart() ? CASTBIT(msgId, 4) : 0;
					}
					
					head.len = contentLength < 126 ? CASTBIT(contentLength,7) : 126;
					if (head.len == 126)
					{
						head.len16 = net::common::ByteOrder::hton(static_cast<unsigned short>(contentLength));
					}

					if (pageInfo == nullptr)
					{
						buffer.Page().startPos = offset;
						buffer.Page().endPos = offset + contentLength - 1;
						buffer.Page().total = static_cast<unsigned int>(len);
					}
					else
					{
						buffer.Page() = *pageInfo;
					}
					memcpy(buffer, &head, headLen);
					memcpy(buffer + headLen, static_cast<const char*>(data)+offset, contentLength);
					buffers.push_back(buffer);
				}

				return true;
			}

			Parser::Result WebsocketServerParser::Unmarshal(daxia::net::common::BasicSession* session, const void* data, size_t len, int& msgID, daxia::net::common::Buffer& buffer, size_t& packetLen) const
			{
				buffer.Clear();

				// 数据不足
				if (len < sizeof(WebsocketHeader)) return Parser::Result::Result_Uncomplete;

				// 解析数据长度
				const WebsocketHeader* head = static_cast<const WebsocketHeader*>(data);
				unsigned long long contentLen = head->len;
				size_t headerLen = sizeof(WebsocketHeader);

				// 是否有mask key
				if (head->mask == 1)
				{
					headerLen += sizeof(unsigned int);
				}
				else
				{
					return Parser::Result::Result_Fail;
				}

				// 解析数据长度
				if (contentLen == 126)
				{
					const WebsocketHeader16* head16 = static_cast<const WebsocketHeader16*>(data);
					contentLen = ByteOrder::ntoh(head16->len16);
					headerLen = sizeof(WebsocketHeader16);
				}

				// 解析数据长度
				if (contentLen == 127)
				{
					const WebsocketHeader64* head64 = static_cast<const WebsocketHeader64*>(data);
					contentLen = ByteOrder::ntoh(head64->len64);
					headerLen = sizeof(WebsocketHeader64);
				}

				// 数据不足
				if (contentLen + headerLen > static_cast<unsigned long long>(len))  return Parser::Result::Result_Uncomplete;

				msgID = head->op;

				buffer.Resize(contentLen);
				memcpy(buffer, static_cast<const char*>(data)+headerLen, contentLen);

				// 解密
				unsigned int maskkey = *reinterpret_cast<const unsigned int*>(static_cast<const char*>(data)+(headerLen - sizeof(unsigned int)));
				char* p = buffer;
				size_t size = buffer.Size();
				for (size_t i = 0; i < size; ++i)
				{
					p[i] = p[i] ^ reinterpret_cast<const char*>(&maskkey)[i % 4];
				}

				packetLen = headerLen + contentLen;


				// 处理分段
				static const char* endPosKey = "WebsocketServerParser::endPosKey";
				static const char* opKey = "WebsocketServerParser::opKey";
				if (head->fin == 0)
				{
					// 第一个帧片段
					if (head->op != 0)
					{
						session->SetUserData(endPosKey, contentLen);

						// 保存op，后续帧片段的op跟第一个帧片段相同
						session->SetUserData(opKey, head->op);

						buffer.Page().startPos = 0;
						buffer.Page().endPos = static_cast<unsigned int>(contentLen - 1);
						buffer.Page().total = static_cast<unsigned int>(-1);
					}
					else
					{
						unsigned long long* endPos = session->GetUserData<unsigned long long>(endPosKey);
						if (endPos == nullptr) return Parser::Result::Result_Fail;

						unsigned char* op = session->GetUserData<unsigned char>(opKey);
						if (op == nullptr)  return Parser::Result::Result_Fail;

						buffer.Page().startPos = static_cast<unsigned int>(*endPos + 1);
						buffer.Page().endPos = buffer.Page().startPos + static_cast<unsigned int>(contentLen - 1);
						buffer.Page().total = static_cast<unsigned int>(-1);

						msgID = *op;
					}
				}
				else
				{
					unsigned long long* endPos = session->GetUserData<unsigned long long>(endPosKey);
					if (endPos == nullptr)
					{
						buffer.Page().startPos = 0;
						buffer.Page().endPos = static_cast<unsigned int>(contentLen - 1);
						buffer.Page().total = static_cast<unsigned int>(contentLen);
					}
					else
					{
						buffer.Page().startPos = static_cast<unsigned int>(*endPos + 1);
						buffer.Page().endPos = buffer.Page().startPos + static_cast<unsigned int>(contentLen - 1);
						buffer.Page().total = buffer.Page().endPos + 1;

						session->DeleteUserData(endPosKey);
						session->DeleteUserData(opKey);
					}
				}

				return Parser::Result::Result_Success;
			}

			bool WebsocketServerParser::Handshake(daxia::net::common::BasicSession* session)
			{
				auto request = session->GetUserData<RequestHeader>(SESSION_USERDATA_REQUEST_INDEX);
				auto response = session->GetUserData<ResponseHeader>(SESSION_USERDATA_RESPONSE_INDEX);
				if (request == nullptr || response == nullptr) return false;

				if (request->Upgrade->CompareNoCase("websocket") != 0) return false;
				if (request->SecWebSocketVersion->CompareNoCase("13") != 0) return false;
				if (request->SecWebSocketKey->IsEmpty()) return false;

				// 计算Sec-WebSocket-Accept
				daxia::string key = request->SecWebSocketKey;
				key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; // magic
				key = daxia::encode::Sha1::Marshal(key);
				key = daxia::encode::Hex::Unmarshal(key);
				key = daxia::encode::Base64::Marshal(key);

				// 填充response
				response->StartLine.StatusCode = "101";
				response->Upgrade = "websocket";
				response->Connection = "Upgrade";
				response->SecWebSocketAccept = key;

				static HttpServerParser parser;
				std::vector<Buffer> buffers;
				parser.Marshal(session, 0, nullptr, 0, nullptr, buffers, 0);
				for (const Buffer& buf : buffers)
				{
					session->WriteRawData(buf, buf.Size());
				}

				return true;
			}

			WebsocketClientParser::WebsocketClientParser()
			{

			}

			WebsocketClientParser::~WebsocketClientParser()
			{

			}

			bool WebsocketClientParser::Marshal(daxia::net::common::BasicSession* session, int msgId, const void* data, size_t len, const daxia::net::common::PageInfo* pageInfo, std::vector<daxia::net::common::Buffer>& buffers, size_t maxPacketLength) const
			{
				throw 1;
			}

			daxia::net::common::Parser::Result WebsocketClientParser::Unmarshal(daxia::net::common::BasicSession* session, const void* data, size_t len, int& msgID, daxia::net::common::Buffer& buffer, size_t& packetLen) const
			{
				throw 1;
			}

		}
	}
}
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
			HttpParser::HttpParser()
			{

			}

			HttpParser::~HttpParser()
			{

			}

			bool HttpParser::Marshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, daxia::dxg::common::shared_buffer& buffer) const
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

			Parser::Result HttpParser::Unmarshal(daxia::dxg::common::BasicSession* session, const daxia::dxg::common::byte* data, int len, int& msgID, daxia::dxg::common::shared_buffer& buffer, int& packetLen) const
			{
				daxia::StringA header((const char*)data, MIN(len, LIMIT_START_LINE_SIZE));

				// ��ȡ��ʼ�н���λ��
				int startLineEndPos = header.Find(CRLF);
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
				daxia::StringA stratLine = header.Left(startLineEndPos);
				std::vector<daxia::StringA> params;
				stratLine.Split(" ", params);

				// У���������
				if (params.size() != RequstLineIndex_End && params.size() != ResponseLineIndex_End) return Parser::Result::Result_Fail;

				// ȷ��������ͷ������Ӧͷ
				bool isRequest = true;
				if (stratLine.Left(4).CompareNoCase("HTTP") == 0)
				{
					isRequest = false;
				}

				if (isRequest)
				{
					// У�鷽���Ƿ�Ϸ�
					if (!methodsHelp_.IsValidMethod(params.front())) return Parser::Result::Result_Fail;
				}

				// ��ȡ����ͷ
				int headerEndPos = header.Find(CRLFCRLF, startLineEndPos + STRLEN_INT(CRLF));
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

				packetLen = headerEndPos + STRLEN_INT(CRLFCRLF);

				// ��ȡContent-Length
				int lastLineEndPos = startLineEndPos;
				int lineEndPos = -1;
				while ((lineEndPos = header.Find(CRLF, lastLineEndPos + STRLEN_INT(CRLF))) != -1)
				{
					daxia::StringA line = header.Mid(lastLineEndPos + STRLEN_INT(CRLF), lineEndPos - lastLineEndPos - STRLEN_INT(CRLF));
					int pos = 0;
					if (line.Tokenize(":", pos) == requestHeaderHelp_.ContentLength.Tag("http").c_str())
					{
						packetLen += atoi(line.Tokenize(":", pos));
						break;
					}
				}

				// ���ݲ���
				if (len < packetLen)  return Parser::Result::Result_Uncomplete;

				// ������ϢID
				if (isRequest)
				{
					msgID = static_cast<int>((params[0] + params[1]).Hash());
				}
				else
				{
					msgID = atoi(params[ResponseLineIndex_StatusCode]);
				}
				
				// ������Ϣ
				memcpy(buffer.get(), data, packetLen);

				return Parser::Result::Result_Success;
			}
		}
	}
}
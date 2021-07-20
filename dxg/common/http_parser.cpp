#include "http_parser.h"
#include "../../string.hpp"

// �����з���
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
				// ����������
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

				// ��ȡ������
				int endpos = str.Find("\r\n");
				if (endpos == -1)
				{
					if (len >= maxHeadLength)
					{
						// �����г��ȴ���maxHeadLength�������ʧ��
						return false;
					}
					else
					{
						// ���ؼ�������
						contentLen = maxHeadLength;
						return true;
					}
				}
				else
				{
					// ��ȡ�����и���������У��
					int start = 0;
					string requestLine = str.Tokenize(endFlag, start);
					std::vector<string> params;
					requestLine.Split(" ", params);

					// У���������
					if (params.size() != httpdefine::RequstLineIndex_End)
					{
						return false;
					}

					// У�鷽��
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
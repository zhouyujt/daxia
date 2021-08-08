/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file http_parser.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* HTTPЭ�������
*
* �ο����ף�
* MDN https://developer.mozilla.org/zh-CN/docs/Web/HTTP
* ��HTTPЭ�����֪ʶ���������(ͼ��HTTP)�� https://www.cnblogs.com/nyatom/p/13038058.html
*/

#ifndef __DAXIA_NET_COMMON_HTTPPARSER_H
#define __DAXIA_NET_COMMON_HTTPPARSER_H
#include <map>
#include "parser.h"
#include "../../reflect/reflect.hpp"
#include "../../string.hpp"

using  daxia::reflect::Reflect;

#define SESSION_USERDATA_REQUEST_INDEX static_cast<daxia::net::common::BasicSession::UserDataIndex>(daxia::net::common::HttpRequestHeaderIndex)
#define SESSION_USERDATA_RESPONSE_INDEX static_cast<daxia::net::common::BasicSession::UserDataIndex>(daxia::net::common::HttpResponseHeaderIndex)
#define HTTP_STATUS_MAP(XX)						\
	XX(100,	Continue)							\
	XX(101, Switching Protocols)				\
	XX(102, Processing)							\
	XX(200, OK)									\
	XX(201, Created)							\
	XX(202, Accepted)							\
	XX(203, Non-Authoritative Information)		\
	XX(204, No Content)							\
	XX(205, Reset Content)						\
	XX(206, Partial Content)					\
	XX(207, Multi - Status)						\
	XX(208, Already Reported)					\
	XX(226, IM Used)							\
	XX(300, Multiple Choices)					\
	XX(301, Moved Permanently)					\
	XX(302, Found)								\
	XX(303, See Other)							\
	XX(304, Not Modified)						\
	XX(305, Use Proxy)							\
	XX(307, Temporary Redirect)					\
	XX(308, Permanent Redirect)					\
	XX(400, Bad Request)						\
	XX(401, Unauthorized)						\
	XX(402, Payment Required)					\
	XX(403, Forbidden)							\
	XX(404, Not Found)							\
	XX(405, Method Not Allowed)					\
	XX(406, Not Acceptable)						\
	XX(407, Proxy Authentication Required)		\
	XX(408, Request Timeout)					\
	XX(409, Conflict)							\
	XX(410, Gone)								\
	XX(411, Length Required)					\
	XX(412, Precondition Failed)				\
	XX(413, Payload Too Large)					\
	XX(414, URI Too Long)						\
	XX(415,	Unsupported Media Type)				\
	XX(416, Range Not Satisfiable)				\
	XX(417, Expectation Failed)					\
	XX(421, Misdirected Request)				\
	XX(422, Unprocessable Entity)				\
	XX(423,	Locked)								\
	XX(424, Failed Dependency)					\
	XX(426, Upgrade Required)					\
	XX(428, Precondition Required)				\
	XX(429, Too Many Requests)					\
	XX(431, Request Header Fields Too Large)	\
	XX(451, Unavailable For Legal Reasons)		\
	XX(500,	Internal Server Error)				\
	XX(501, Not Implemented)					\
	XX(502, Bad Gateway)						\
	XX(503, Service Unavailable)				\
	XX(504, Gateway Timeout)					\
	XX(505, HTTP Version Not Supported)			\
	XX(506, Variant Also Negotiates)			\
	XX(507, Insufficient Storage)				\
	XX(508, Loop Detected)						\
	XX(510, Not Extended)						\
	XX(511, Network Authentication Required)	\

namespace daxia
{
	namespace net
	{
		namespace common
		{
			class HttpParser : public Parser
			{
			protected:
				HttpParser(){}
				virtual ~HttpParser(){}
			public:
				class HeaderHelp;
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session,
					const daxia::net::common::byte* data,
					size_t len,
					daxia::net::common::shared_buffer& buffer
					) const = 0;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const daxia::net::common::byte* data,
					size_t len,
					int& msgID,
					daxia::net::common::shared_buffer& buffer,
					size_t& packetLen
					) const = 0;
			public:

				// ������ʼ�зִ�����
				enum RequstLineIndex : int
				{
					RequstLineIndex_Method = 0,
					RequstLineIndex_Url,
					RequstLineIndex_Version,
					RequstLineIndex_End
				};

				// ��Ӧ��ʼ�зִ�����
				enum ResponseLineIndex : int
				{
					ResponseLineIndex_Version = 0,
					ResponseLineIndex_StatusCode,
					ResponseLineIndex_StatusText,
					ResponseLineIndex_End
				};

				// ���󷽷�
				struct Methods
				{
					reflect::String Get = "http:GET";				// ��ȡ��Դ
					reflect::String Post = "http:POST";				// ����ʵ������
					reflect::String Put = "http:PUT";				// �����ļ� �������REST��׼
					reflect::String Head = "http:HEAD";				// ��ñ����ײ�, ��GET����һ��,ֻ�ǲ����ر������岿��. ����ȷ��URI����Ч�Լ���Դ���µ�����ʱ���.
					reflect::String Delete = "http:DELETE";			// ɾ���ļ� �������REST��׼
					reflect::String Options = "http:OPTIONS";		// ѯ��֧�ֵķ���, ������ѯ�������URIָ������Դ֧�ֵķ���
					reflect::String Trace = "http:TRACE";			// ׷��·��, ��������
					reflect::String Connect = "http:CONNECT";		// Ҫ�������Э�����Ӵ���

					bool IsValidMethod(const char* method)
					{
						daxia::string temp(method);
						if (temp.CompareNoCase(Get.Tag("http").GetString()) == 0) return true;
						if (temp.CompareNoCase(Post.Tag("http").GetString()) == 0) return true;
						if (temp.CompareNoCase(Put.Tag("http").GetString()) == 0) return true;
						if (temp.CompareNoCase(Head.Tag("http").GetString()) == 0) return true;
						if (temp.CompareNoCase(Delete.Tag("http").GetString()) == 0) return true;
						if (temp.CompareNoCase(Options.Tag("http").GetString()) == 0) return true;
						if (temp.CompareNoCase(Trace.Tag("http").GetString()) == 0) return true;
						if (temp.CompareNoCase(Connect.Tag("http").GetString()) == 0) return true;

						return false;
					}
				};

				struct RequestStartLine
				{
					daxia::string Method;
					daxia::string Url;
					daxia::string Version;
					std::map<daxia::string, daxia::string> Params;

					daxia::string GetParam(const char* param) const
					{
						daxia::string value;
						auto iter = Params.find(param);
						if (iter != Params.end())
						{
							value = iter->second;
						}

						return value;
					}
				};

				struct ResponseStartLine
				{
					daxia::string Version;
					daxia::string StatusCode;
					daxia::string StatusText;
				};

				// ͨ���ײ��ֶΣ�General Header Fields�������ĺ���Ӧ����˫������ʹ�õ��ײ�
				class GeneralHeader
				{
					friend HeaderHelp;
				protected:
					std::vector<daxia::string> StartLine;

					/*
					�ײ��ֶ���			˵��									��ע
					Cache-Control		���ƻ������Ϊ							Cache-Control:private,max-age=0,no-cache ��ָ��
					Connection			�����ײ�/���ӵĹ���						1. ���Ʋ���ת����������ײ��ֶ� 2. ����־�����
					Date				�������ĵ�����ʱ��
					Pragma				����ָ��								��Ϊ��HTTP/1.0�������ݶ�����
					Trailer				����ĩ�˵��ײ�һ��						ʵ��˵���ڱ���������¼����Щ�ײ��ֶ�
					Transfer-Encoding	ָ����������Ĵ�����뷽ʽ				HTTP/1.1�Ĵ�����뷽ʽ���Էֿ鴫�������Ч
					Upgrade				����Ϊ����Э��							ʹ��ʱ�������ָ��Connection:Upgrade
					Via					����������������Ϣ					Via���ײ���Ϊ��׷�ٴ���·��,���TRACE����ʹ��
					Warning				����֪ͨ								110 111 112 113 199 214 299
					*/
				public:
					reflect::String CacheControl = "http:Cache-Control";
					reflect::String Connection = "http:Connection";
					reflect::String Date = "http:Date";
					reflect::String Pragma = "http:Pragma";
					reflect::String Trailer = "http:Trailer";
					reflect::String TransferEncoding = "http:Transfer-Encoding";
					reflect::String Upgrade = "http:Upgrade";
					reflect::String Via = "http:Via";
					reflect::String Warning = "http:Warning";
				public:
					reflect::String* Find(const daxia::string& key,const void* base) const;
				protected:
					size_t InitFromData(const void* data, size_t len, bool isRequest);
				private:
					// �ӿ���ҵ�����
					std::map<daxia::string/*tag*/, unsigned long/*offset*/> index_;
				};

				class RequestHeader : public GeneralHeader
				{
				public:
					RequestStartLine StartLine;

					/*�����ײ��ֶ�(Request Header Fields) �ӿͻ�����������˷���������ʱʹ�õ��ײ�.����������ĸ�������/�ͻ�����Ϣ/��Ӧ����������ȼ�����Ϣ
					�ײ��ֶ���			˵��									��ע
					Accept				�û�����ɴ����ý������				�ı��ļ� text/html ͼƬ�ļ�	image/jpeg ��Ƶ�ļ�	video/mpeg Ӧ�ó���ʹ�õĶ������ļ� eg: application/json
					Accept-Charset		���ȵ��ַ���
					Accept-Encoding		���ȵ����ݱ���
					Accept-Language		���ȵ�����(��Ȼ����)
					Authorization		Web��֤��Ϣ
					Expect				�ڴ����������ض���Ϊ
					From				�û��ĵ��������ַ
					Host				������Դ���ڷ�����
					If-Match			�Ƚ�ʵ����(ETag)
					If-Modified-Since	�Ƚ���Դ�ĸ���ʱ��
					If-None-Match		�Ƚ�ʵ����(��If-Match�෴)
					If-Range			��Դδ����ʱ����ʵ��Byte�ķ�Χ����
					If-Unmodified-Since	�Ƚ���Դ�ĸ���ʱ��(��If-Modified-Since�෴)
					Max-Forwards		�����������
					Proxy-Authorization	���������Ҫ��ͻ��˵���֤��Ϣ
					Range				ʵ����ֽڷ�Χ����
					Referer				��������URI��ԭʼ��ȡ��
					TE					�����������ȼ�
					User-Agent			HTTP�ͻ��˳������Ϣ					�������������� \�û��������Ƶ���Ϣ
					*/
				public:
					reflect::String Accept = "http:Accept";
					reflect::String AcceptCharset = "http:Accept-Charset";
					reflect::String AcceptEncoding = "http:Accept-Encoding";
					reflect::String AcceptLanguage = "http:Accept-Language";
					reflect::String Authorization = "http:Authorization";
					reflect::String Expect = "http:Expect";
					reflect::String From = "http:From";
					reflect::String Host = "http:Host";
					reflect::String IfMatch = "http:If-Match";
					reflect::String IfModifiedSince = "http:If-Modified-Since";
					reflect::String IfNoneMatch = "http:If-None-match";
					reflect::String IfRange = "http:If-Range";
					reflect::String IfUnmodifiedSince = "http:If-Unmodified-Since";
					reflect::String MaxForward = "http:Max-Forward";
					reflect::String ProxyAuthorization = "http:Proxy-Authorization";
					reflect::String Range = "http:Range";
					reflect::String Referer = "http:Referer";
					reflect::String TE = "http:TE";
					reflect::String UserAgent = "http:User-Agent";

					/*ʵ���ײ��ֶ�(Entity Header Fields) ��������ĺ���Ӧ���ĵ�ʵ�岿��ʹ�õ��ײ�.��������Դ���ݸ���ʱ�����ʵ���йص���Ϣ
					�ײ��ֶ���			˵��									��ע
					Allow				��Դ��֧�ֵ�HTTP����					��֧��,����405 Method Not Allowed
					Cookie				���������յ���Cookie��Ϣ
					Content-Encoding	ʵ������ʹ�õı��뷽ʽ					���ݱ����ڲ���ʧʵ����Ϣ��ǰ���������е�ѹ��eg: gzip compress deflate identity ������Ҫ���ݱ���
					Content-Language	ʵ���������Ȼ����
					Content-Length		ʵ������Ĵ�С(�ֽ�)
					Content-Location	�����Ӧ��Դ��URI
					Content-MD5			ʵ������ı���ժҪ
					Content-Range		ʵ�������λ�÷�Χ
					Content-Type		ʵ�������ý������						ʵ�������ڶ����ý������,���ײ��ֶ�Acceptһ��,�ֶ�ֵ��type/subtype��ʽ��ֵ.eg: application/json
					Expires				ʵ��������ڵ�����ʱ��
					Last-Modified		��Դ������޸�����ʱ��
					*/
				public:
					reflect::String Allow = "http:Allow";
					reflect::String Cookie = "http:Cookie";
					reflect::String ContentEncoding = "http:Content-Encoding";
					reflect::String ContentLanguage = "http:Content-Language";
					reflect::String ContentLength = "http:Content-Length";
					reflect::String ContentLocation = "http:Content-Location";
					reflect::String ContentMD5 = "http:Content-MD5";
					reflect::String ContentRange = "http:Content-Range";
					reflect::String ContentType = "http:Content-Type";
					reflect::String Expires = "http:Expires";
					reflect::String LastModified = "http:Last-Modified";

				public:
					size_t InitFromData(const void* data, size_t len);
				};

				class ResponseHeader : public GeneralHeader
				{
				public:
					bool isNoHeader;
					ResponseHeader() : isNoHeader(false){}
				public:
					ResponseStartLine StartLine;

					/*��Ӧ�ײ��ֶ�(Response Header Fields) �ӷ���������ͻ��˷�����Ӧ����ʱʹ�õ��ײ�.��������Ӧ�ĸ�������,Ҳ��Ҫ��ͻ��˸��Ӷ����������Ϣ
					�ײ��ֶ���			˵��
					Accept-Ranges		�Ƿ�����ֽڷ�Χ����
					Age					������Դ��������ʱ��
					ETage				��Դ��ƥ����Ϣ
					Location			��ͻ����ض�����ָ��URI
					Proxy-Authenticate	����������Կͻ��˵���֤��Ϣ
					Retry-After			���ٴη��������ʱ��Ҫ��
					Server				HTTP�������İ�װ��Ϣ
					Vary				�������������Ĺ�����Ϣ
					WWW-Authenticate	�������Կͻ��˵���֤��Ϣ
					*/
				public:
					reflect::String AcceptRanges = "http:Accept-Ranges";
					reflect::String Age = "http:Age";
					reflect::String ETage = "http:ETage";
					reflect::String Location = "http:Location";
					reflect::String ProxyAuthenticate = "http:Proxy-Authenticate";
					reflect::String RetryAfter = "http:Retry-After";
					reflect::String Server = "http:Server";
					reflect::String Vary = "http:Vary";
					reflect::String WWWAuthenticate = "http:WWW-Authenticate";

					/*ʵ���ײ��ֶ�(Entity Header Fields) ��������ĺ���Ӧ���ĵ�ʵ�岿��ʹ�õ��ײ�.��������Դ���ݸ���ʱ�����ʵ���йص���Ϣ
					�ײ��ֶ���			˵��									��ע
					Allow				��Դ��֧�ֵ�HTTP����					��֧��,����405 Method Not Allowed
					Set-Cookie			��ʼ״̬������ʹ�õ�Cookie��Ϣ
					Content-Encoding	ʵ������ʹ�õı��뷽ʽ					���ݱ����ڲ���ʧʵ����Ϣ��ǰ���������е�ѹ��eg: gzip compress deflate identity ������Ҫ���ݱ���
					Content-Language	ʵ���������Ȼ����
					Content-Length		ʵ������Ĵ�С(�ֽ�)
					Content-Location	�����Ӧ��Դ��URI
					Content-MD5			ʵ������ı���ժҪ
					Content-Range		ʵ�������λ�÷�Χ
					Content-Type		ʵ�������ý������						ʵ�������ڶ����ý������,���ײ��ֶ�Acceptһ��,�ֶ�ֵ��type/subtype��ʽ��ֵ.eg: application/json
					Expires				ʵ��������ڵ�����ʱ��
					Last-Modified		��Դ������޸�����ʱ��
					*/
				public:
					reflect::String Allow = "http:Allow";
					reflect::String SetCookie = "http:Set-Cookie";
					reflect::String ContentEncoding = "http:Content-Encoding";
					reflect::String ContentLanguage = "http:Content-Language";
					reflect::String ContentLength = "http:Content-Length";
					reflect::String ContentLocation = "http:Content-Location";
					reflect::String ContentMD5 = "http:Content-MD5";
					reflect::String ContentRange = "http:Content-Range";
					reflect::String ContentType = "http:Content-Type";
					reflect::String Expires = "http:Expires";
					reflect::String LastModified = "http:Last-Modified";

				public:
					size_t InitFromData(const void* data, size_t len);
				};
			public:
				class HeaderHelp
				{
				public:
					HeaderHelp()
					{
						InitIndex(request_);
						InitIndex(response_);

#define XX(code,text)	status_[code] = #text;
						HTTP_STATUS_MAP(XX)
#undef XX
					}
					~HeaderHelp()
					{

					}
				private:
					template<class T>
					void InitIndex(T& obj)
					{
						auto& layout = obj.GetLayout();
						for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
						{
							const reflect::String* field = nullptr;
							try{ field = dynamic_cast<const reflect::String*>(reinterpret_cast<const reflect::Reflect_base*>(reinterpret_cast<const char*>(&obj.Value()) + iter->offset)); }
							catch (const std::exception&){}
							if (field == nullptr) continue;

							obj.Value().index_[daxia::string(field->Tag("http")).MakeLower()] = iter->offset;
						}
					}
				public:
					reflect::Reflect<RequestHeader> request_;
					reflect::Reflect<ResponseHeader> response_;
					std::map<int, std::string> status_;
				};
			};

			class HttpServerParser : public HttpParser
			{
			public:
				HttpServerParser(){}
				~HttpServerParser(){}
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session,
					const daxia::net::common::byte* data,
					size_t len,
					daxia::net::common::shared_buffer& buffer
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const daxia::net::common::byte* data,
					size_t len,
					int& msgID,
					daxia::net::common::shared_buffer& buffer,
					size_t& packetLen
					) const override;
			};

			class HttpClientParser : public HttpParser
			{
			public:
				HttpClientParser(){}
				~HttpClientParser(){}
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session,
					const daxia::net::common::byte* data,
					size_t len,
					daxia::net::common::shared_buffer& buffer
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const daxia::net::common::byte* data,
					size_t len,
					int& msgID,
					daxia::net::common::shared_buffer& buffer,
					size_t& packetLen
					) const override;
			};
		}
	}
}

#undef OFFSET
#undef MAKE_INDEX

#endif // !__DAXIA_NET_COMMON_HTTPPARSER_H

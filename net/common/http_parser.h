/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file http_parser.h
* \author 漓江里的大虾
* \date 七月 2021
*
* HTTP协议解析器
*
* 参考文献：
* MDN https://developer.mozilla.org/zh-CN/docs/Web/HTTP
* 《HTTP协议相关知识点整理汇总(图解HTTP)》 https://www.cnblogs.com/nyatom/p/13038058.html
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

				// 请求起始行分词索引
				enum RequstLineIndex : int
				{
					RequstLineIndex_Method = 0,
					RequstLineIndex_Url,
					RequstLineIndex_Version,
					RequstLineIndex_End
				};

				// 响应起始行分词索引
				enum ResponseLineIndex : int
				{
					ResponseLineIndex_Version = 0,
					ResponseLineIndex_StatusCode,
					ResponseLineIndex_StatusText,
					ResponseLineIndex_End
				};

				// 请求方法
				struct Methods
				{
					reflect::String Get = "http:GET";				// 获取资源
					reflect::String Post = "http:POST";				// 传送实体主体
					reflect::String Put = "http:PUT";				// 传输文件 尽量配合REST标准
					reflect::String Head = "http:HEAD";				// 获得报文首部, 和GET方法一样,只是不返回报文主体部分. 用于确认URI的有效性及资源更新的日期时间等.
					reflect::String Delete = "http:DELETE";			// 删除文件 尽量配合REST标准
					reflect::String Options = "http:OPTIONS";		// 询问支持的方法, 用来查询针对请求URI指定的资源支持的方法
					reflect::String Trace = "http:TRACE";			// 追踪路径, 极不常用
					reflect::String Connect = "http:CONNECT";		// 要求用隧道协议连接代理

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

				// 通用首部字段（General Header Fields）请求报文和响应报文双方都会使用的首部
				class GeneralHeader
				{
					friend HeaderHelp;
				protected:
					std::vector<daxia::string> StartLine;

					/*
					首部字段名			说明									备注
					Cache-Control		控制缓存的行为							Cache-Control:private,max-age=0,no-cache 多指令
					Connection			逐跳首部/连接的管理						1. 控制不能转发给代理的首部字段 2. 管理持久连接
					Date				创建报文的日期时间
					Pragma				报文指令								作为与HTTP/1.0的向后兼容而定义
					Trailer				报文末端的首部一览						实现说明在报文主体后记录了哪些首部字段
					Transfer-Encoding	指定报文主体的传输编码方式				HTTP/1.1的传输编码方式仅对分块传输编码有效
					Upgrade				升级为其他协议							使用时还需额外指定Connection:Upgrade
					Via					代理服务器的相关信息					Via的首部是为了追踪传输路径,配合TRACE方法使用
					Warning				错误通知								110 111 112 113 199 214 299
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
					// 加快查找的索引
					std::map<daxia::string/*tag*/, unsigned long/*offset*/> index_;
				};

				class RequestHeader : public GeneralHeader
				{
				public:
					RequestStartLine StartLine;

					/*请求首部字段(Request Header Fields) 从客户端向服务器端发送请求报文时使用的首部.补充了请求的附加内容/客户端信息/响应内容相关优先级等信息
					首部字段名			说明									备注
					Accept				用户代理可处理的媒体类型				文本文件 text/html 图片文件	image/jpeg 视频文件	video/mpeg 应用程序使用的二进制文件 eg: application/json
					Accept-Charset		优先的字符集
					Accept-Encoding		优先的内容编码
					Accept-Language		优先的语言(自然语言)
					Authorization		Web认证信息
					Expect				期待服务器的特定行为
					From				用户的电子邮箱地址
					Host				请求资源所在服务器
					If-Match			比较实体标记(ETag)
					If-Modified-Since	比较资源的更新时间
					If-None-Match		比较实体标记(与If-Match相反)
					If-Range			资源未更新时发送实体Byte的范围请求
					If-Unmodified-Since	比较资源的更新时间(与If-Modified-Since相反)
					Max-Forwards		最大传输逐跳数
					Proxy-Authorization	代理服务器要求客户端的认证信息
					Range				实体的字节范围请求
					Referer				对请求中URI的原始获取方
					TE					传输编码的优先级
					User-Agent			HTTP客户端程序的信息					创建请求的浏览器 \用户代理名称等信息
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

					/*实体首部字段(Entity Header Fields) 针对请求报文和响应报文的实体部分使用的首部.补充了资源内容更新时间等与实体有关的信息
					首部字段名			说明									备注
					Allow				资源可支持的HTTP方法					不支持,返回405 Method Not Allowed
					Cookie				服务器接收到的Cookie信息
					Content-Encoding	实体主体使用的编码方式					内容编码在不丢失实体信息的前提下所进行的压缩eg: gzip compress deflate identity 四种主要内容编码
					Content-Language	实体主体的自然语言
					Content-Length		实体主体的大小(字节)
					Content-Location	替代对应资源的URI
					Content-MD5			实体主体的报文摘要
					Content-Range		实体主体的位置范围
					Content-Type		实体主体的媒体类型						实体主体内对象的媒体类型,和首部字段Accept一样,字段值用type/subtype形式赋值.eg: application/json
					Expires				实体主体过期的日期时间
					Last-Modified		资源的最后修改日期时间
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

					/*响应首部字段(Response Header Fields) 从服务器端向客户端返回响应报文时使用的首部.补充了响应的附加内容,也会要求客户端附加额外的内容信息
					首部字段名			说明
					Accept-Ranges		是否接受字节范围请求
					Age					推算资源创建经过时间
					ETage				资源的匹配信息
					Location			令客户端重定向至指定URI
					Proxy-Authenticate	代理服务器对客户端的认证信息
					Retry-After			对再次发起请求的时机要求
					Server				HTTP服务器的安装信息
					Vary				代理服务器缓存的管理信息
					WWW-Authenticate	服务器对客户端的认证信息
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

					/*实体首部字段(Entity Header Fields) 针对请求报文和响应报文的实体部分使用的首部.补充了资源内容更新时间等与实体有关的信息
					首部字段名			说明									备注
					Allow				资源可支持的HTTP方法					不支持,返回405 Method Not Allowed
					Set-Cookie			开始状态管理所使用的Cookie信息
					Content-Encoding	实体主体使用的编码方式					内容编码在不丢失实体信息的前提下所进行的压缩eg: gzip compress deflate identity 四种主要内容编码
					Content-Language	实体主体的自然语言
					Content-Length		实体主体的大小(字节)
					Content-Location	替代对应资源的URI
					Content-MD5			实体主体的报文摘要
					Content-Range		实体主体的位置范围
					Content-Type		实体主体的媒体类型						实体主体内对象的媒体类型,和首部字段Accept一样,字段值用type/subtype形式赋值.eg: application/json
					Expires				实体主体过期的日期时间
					Last-Modified		资源的最后修改日期时间
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

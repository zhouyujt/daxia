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
#include <unordered_map>
#include "parser.h"
#include "../../reflect/reflect.hpp"
#include "../../string.hpp"
#include "../../singleton.hpp"

using  daxia::reflect::Reflect;

#define SESSION_USERDATA_REQUEST_INDEX static_cast<daxia::net::common::BasicSession::UserDataIndex>(daxia::net::common::HttpRequestHeaderIndex)
#define SESSION_USERDATA_RESPONSE_INDEX static_cast<daxia::net::common::BasicSession::UserDataIndex>(daxia::net::common::HttpResponseHeaderIndex)
#define HEADER_HELPER() daxia::Singleton<typename daxia::net::common::HttpParser::HeaderHelp>::Instance()
#define MIME_HELPER() daxia::Singleton<typename daxia::net::common::HttpParser::MimeHelp>::Instance()
#define HTTP_STATUS_MAP(XX)						\
	XX(100, Continue)							\
	XX(101, Switching Protocols)				\
	XX(102, Processing)							\
	XX(200, OK)									\
	XX(201, Created)							\
	XX(202, Accepted)							\
	XX(203, Non - Authoritative Information)	\
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
	XX(415, Unsupported Media Type)				\
	XX(416, Range Not Satisfiable)				\
	XX(417, Expectation Failed)					\
	XX(421, Misdirected Request)				\
	XX(422, Unprocessable Entity)				\
	XX(423, Locked)								\
	XX(424, Failed Dependency)					\
	XX(426, Upgrade Required)					\
	XX(428, Precondition Required)				\
	XX(429, Too Many Requests)					\
	XX(431, Request Header Fields Too Large)	\
	XX(451, Unavailable For Legal Reasons)		\
	XX(500, Internal Server Error)				\
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

// HTTP MIME 类型
#define HTTP_MIME_MAP(XX)								\
	XX(323, "text/h323")								\
	XX(acx, "application/nternet-property-stream")		\
	XX(ai, "application/postscript")					\
	XX(aif, "audio/x-aiff")								\
	XX(aifc, "audio/x-aiff")							\
	XX(aiff, "audio/x-aiff")							\
	XX(asf, "video/x-ms-asf")							\
	XX(asr, "video/x-ms-asf")							\
	XX(asx, "video/x-ms-asf")							\
	XX(au, "audio/basic")								\
	XX(avi, "video/x-msvideo")							\
	XX(axs, "application/olescript")					\
	XX(bas, "text/plain")								\
	XX(bcpio, "application/x-bcpio")					\
	XX(bin, "application/octet-stream")					\
	XX(bmp, "image/bmp")								\
	XX(c, "text/plain")									\
	XX(cat, "application/vnd.ms-pkiseccat")				\
	XX(cdf, "application/x-cdf")						\
	XX(cer, "application/x-x509-ca-cert")				\
	XX(class, "application/octet-stream")				\
	XX(clp, "application/x-msclip")						\
	XX(cmx, "image/x-cmx")								\
	XX(cod, "image/cis-cod")							\
	XX(cpio, "application/x-cpio")						\
	XX(crd, "application/x-mscardfile")					\
	XX(crl, "application/pkix-crl")						\
	XX(crt, "application/x-x509-ca-cert")				\
	XX(csh, "application/x-csh")						\
	XX(css, "text/css")									\
	XX(dcr, "application/x-director")					\
	XX(der, "application/x-x509-ca-cert")				\
	XX(dir, "application/x-director")					\
	XX(dll, "application/x-msdownload")					\
	XX(dms, "application/octet-stream")					\
	XX(doc, "application/msword")						\
	XX(dot, "application/msword")						\
	XX(dvi, "application/x-dvi")						\
	XX(dxr, "application/x-director")					\
	XX(eps, "application/postscript")					\
	XX(etx, "text/x-setext")							\
	XX(evy, "application/envoy")						\
	XX(exe, "application/octet-stream")					\
	XX(fif, "application/fractals")						\
	XX(flr, "x-world/x-vrml")							\
	XX(gif, "image/gif")								\
	XX(gtar, "application/x-gtar")						\
	XX(gz, "application/x-gzip")						\
	XX(h, "text/plain")									\
	XX(hdf, "application/x-hdf")						\
	XX(hlp, "application/winhlp")						\
	XX(hqx, "application/mac-binhex40")					\
	XX(hta, "application/hta")							\
	XX(htc, "text/x-component")							\
	XX(htm, "text/html")								\
	XX(html, "text/html")								\
	XX(htt, "text/webviewhtml")							\
	XX(ico, "image/x-icon")								\
	XX(ief, "image/ief")								\
	XX(iii, "application/x-iphone")						\
	XX(ins, "application/x-internet-signup")			\
	XX(isp, "application/x-internet-signup")			\
	XX(jfif, "image/pipeg")								\
	XX(jpe, "image/jpeg")								\
	XX(jpeg, "image/jpeg")								\
	XX(jpg, "image/jpeg")								\
	XX(js, "application/x-javascript")					\
	XX(json, "application/json")						\
	XX(latex, "application/x-latex")					\
	XX(lha, "application/octet-stream")					\
	XX(lsf, "video/x-la-asf")							\
	XX(lsx, "video/x-la-asf")							\
	XX(lzh, "application/octet-stream")					\
	XX(m13, "application/x-msmediaview")				\
	XX(m14, "application/x-msmediaview")				\
	XX(m3u, "audio/x-mpegurl")							\
	XX(man, "application/x-troff-man")					\
	XX(mdb, "application/x-msaccess")					\
	XX(me, "application/x-troff-me")					\
	XX(mht, "message/rfc822")							\
	XX(mhtml, "message/rfc822")							\
	XX(mid, "audio/mid")								\
	XX(mny, "application/x-msmoney")					\
	XX(mov, "video/quicktime")							\
	XX(movie, "video/x-sgi-movie")						\
	XX(mp2, "video/mpeg")								\
	XX(mp3, "audio/mpeg")								\
	XX(mpa, "video/mpeg")								\
	XX(mpe, "video/mpeg")								\
	XX(mpeg, "video/mpeg")								\
	XX(mpg, "video/mpeg")								\
	XX(mpp, "application/vnd.ms-project")				\
	XX(mpv2, "video/mpeg")								\
	XX(ms, "application/x-troff-ms")					\
	XX(mvb, "application/x-msmediaview")				\
	XX(nws, "message/rfc822")							\
	XX(oda, "application/oda")							\
	XX(p10, "application/pkcs10")						\
	XX(p12, "application/x-pkcs12")						\
	XX(p7b, "application/x-pkcs7-certificates")			\
	XX(p7c, "application/x-pkcs7-mime")					\
	XX(p7m, "application/x-pkcs7-mime")					\
	XX(p7r, "application/x-pkcs7-certreqresp")			\
	XX(p7s, "application/x-pkcs7-signature")			\
	XX(pbm, "image/x-portable-bitmap")					\
	XX(pdf, "application/pdf")							\
	XX(pfx, "application/x-pkcs12")						\
	XX(pgm, "image/x-portable-graymap")					\
	XX(pko, "application/ynd.ms-pkipko")				\
	XX(pma, "application/x-perfmon")					\
	XX(pmc, "application/x-perfmon")					\
	XX(pml, "application/x-perfmon")					\
	XX(pmr, "application/x-perfmon")					\
	XX(pmw, "application/x-perfmon")					\
	XX(pnm, "image/x-portable-anymap")					\
	XX(pot, "application/vnd.ms-powerpoint")			\
	XX(ppm, "image/x-portable-pixmap")					\
	XX(pps, "application/vnd.ms-powerpoint")			\
	XX(ppt, "application/vnd.ms-powerpoint")			\
	XX(prf, "application/pics-rules")					\
	XX(ps, "application/postscript")					\
	XX(pub, "application/x-mspublisher")				\
	XX(qt, "video/quicktime")							\
	XX(ra, "audio/x-pn-realaudio")						\
	XX(ram, "audio/x-pn-realaudio")						\
	XX(ras, "image/x-cmu-raster")						\
	XX(rgb, "image/x-rgb")								\
	XX(rmi, "audio/mid")								\
	XX(roff, "application/x-troff")						\
	XX(rtf, "application/rtf")							\
	XX(rtx, "text/richtext")							\
	XX(scd, "application/x-msschedule")					\
	XX(sct, "text/scriptlet")							\
	XX(setpay, "application/set-payment-initiation")	\
	XX(setreg, "application/set-registration-initiation")	\
	XX(sh, "application/x-sh")							\
	XX(shar, "application/x-shar")						\
	XX(sit, "application/x-stuffit")					\
	XX(snd, "audio/basic")								\
	XX(spc, "application/x-pkcs7-certificates")			\
	XX(spl, "application/futuresplash")					\
	XX(src, "application/x-wais-source")				\
	XX(sst, "application/vnd.ms-pkicertstore")			\
	XX(stl, "application/vnd.ms-pkistl")				\
	XX(stm, "text/html")								\
	XX(svg, "image/svg+xml")							\
	XX(sv4cpio, "application/x-sv4cpio")				\
	XX(sv4crc, "application/x-sv4crc")					\
	XX(swf, "application/x-shockwave-flash")			\
	XX(t, "application/x-troff")						\
	XX(tar, "application/x-tar")						\
	XX(tcl, "application/x-tcl")						\
	XX(tex, "application/x-tex")						\
	XX(texi, "application/x-texinfo")					\
	XX(texinfo, "application/x-texinfo")				\
	XX(tgz, "application/x-compressed")					\
	XX(tif, "image/tiff")								\
	XX(tiff, "image/tiff")								\
	XX(tr, "application/x-troff")						\
	XX(trm, "application/x-msterminal")					\
	XX(tsv, "text/tab-separated-values")				\
	XX(txt, "text/plain")								\
	XX(uls, "text/iuls")								\
	XX(ustar, "application/x-ustar")					\
	XX(vcf, "text/x-vcard")								\
	XX(vrml, "x-world/x-vrml")							\
	XX(wav, "audio/x-wav")								\
	XX(wcm, "application/vnd.ms-works")					\
	XX(wdb, "application/vnd.ms-works")					\
	XX(wks, "application/vnd.ms-works")					\
	XX(wmf, "application/x-msmetafile")					\
	XX(wps, "application/vnd.ms-works")					\
	XX(wri, "application/x-mswrite")					\
	XX(wrl, "x-world/x-vrml")							\
	XX(wrz, "x-world/x-vrml")							\
	XX(xaf, "x-world/x-vrml")							\
	XX(xbm, "image/x-xbitmap")							\
	XX(xla, "application/vnd.ms-excel")					\
	XX(xlc, "application/vnd.ms-excel")					\
	XX(xlm, "application/vnd.ms-excel")					\
	XX(xls, "application/vnd.ms-excel")					\
	XX(xlt, "application/vnd.ms-excel")					\
	XX(xlw, "application/vnd.ms-excel")					\
	XX(xof, "x-world/x-vrml")							\
	XX(xpm, "image/x-xpixmap")							\
	XX(xwd, "image/x-xwindowdump")						\
	XX(z, "application/x-compress")						\
	XX(zip, "application/zip")							\

namespace daxia
{
	namespace net
	{
		namespace common
		{
			using namespace reflect;

			class HttpParser : public Parser
			{
			protected:
				HttpParser(){}
				virtual ~HttpParser(){}
			public:
				class HeaderHelp;
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session,
					int msgId,
					const void* data,
					size_t len,
					const daxia::net::common::PageInfo* pageInfo,
					std::vector<daxia::net::common::Buffer>& buffers,
					size_t maxPacketLength
					) const = 0;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const void* data,
					size_t len,
					int& msgID,
					daxia::net::common::Buffer& buffer,
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
					ref_string Get{ "http:GET" };				// 获取资源
					ref_string Post{ "http:POST" };				// 传送实体主体
					ref_string Put{ "http:PUT" };				// 传输文件 尽量配合REST标准
					ref_string Head{ "http:HEAD" };				// 获得报文首部, 和GET方法一样,只是不返回报文主体部分. 用于确认URI的有效性及资源更新的日期时间等.
					ref_string Delete{ "http:DELETE" };			// 删除文件 尽量配合REST标准
					ref_string Options{ "http:OPTIONS" };		// 询问支持的方法, 用来查询针对请求URI指定的资源支持的方法
					ref_string Trace{ "http:TRACE" };			// 追踪路径, 极不常用
					ref_string Connect{ "http:CONNECT" };		// 要求用隧道协议连接代理

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
					std::unordered_map<daxia::string, daxia::string> Params;

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
					ref_string CacheControl{ "http:Cache-Control" };
					ref_string Connection{ "http:Connection" };
					ref_string Date{ "http:Date" };
					ref_string Pragma{ "http:Pragma" };
					ref_string Trailer{ "http:Trailer" };
					ref_string TransferEncoding{ "http:Transfer-Encoding" };
					ref_string Upgrade{ "http:Upgrade" };
					ref_string Via{ "http:Via" };
					ref_string Warning{ "http:Warning" };
				public:
					size_t PacketLen{ 0 };	// 消息头字符串长度
				public:
					ref_string* Find(const daxia::string& key, const void* base) const;
				protected:
					size_t InitFromData(const void* data, size_t len, bool isRequest);
				private:
					// 加快查找的索引
					std::unordered_map<daxia::string/*tag*/, size_t/*offset*/> index_;
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
					ref_string Accept{ "http:Accept" };
					ref_string AcceptCharset{ "http:Accept-Charset" };
					ref_string AcceptEncoding{ "http:Accept-Encoding" };
					ref_string AcceptLanguage{ "http:Accept-Language" };
					ref_string Authorization{ "http:Authorization" };
					ref_string Expect{ "http:Expect" };
					ref_string From{ "http:From" };
					ref_string Host{ "http:Host" };
					ref_string IfMatch{ "http:If-Match" };
					ref_string IfModifiedSince{ "http:If-Modified-Since" };
					ref_string IfNoneMatch{ "http:If-None-match" };
					ref_string IfRange{ "http:If-Range" };
					ref_string IfUnmodifiedSince{ "http:If-Unmodified-Since" };
					ref_string MaxForward{ "http:Max-Forward" };
					ref_string ProxyAuthorization{ "http:Proxy-Authorization" };
					ref_string Range{ "http:Range" };
					ref_string Referer{ "http:Referer" };
					ref_string TE{ "http:TE" };
					ref_string UserAgent{ "http:User-Agent" };

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
					ref_string Allow{ "http:Allow" };
					ref_string Cookie{ "http:Cookie" };
					ref_string ContentEncoding{ "http:Content-Encoding" };
					ref_string ContentLanguage{ "http:Content-Language" };
					ref_string ContentLength{ "http:Content-Length" };
					ref_string ContentLocation{ "http:Content-Location" };
					ref_string ContentMD5{ "http:Content-MD5" };
					ref_string ContentRange{ "http:Content-Range" };
					ref_string ContentType{ "http:Content-Type" };
					ref_string Expires{ "http:Expires" };
					ref_string LastModified{ "http:Last-Modified" };

				public:
					size_t InitFromData(const void* data, size_t len);
				};

				class ResponseHeader : public GeneralHeader
				{
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
					ref_string AcceptRanges{ "http:Accept-Ranges" };
					ref_string Age{ "http:Age" };
					ref_string ETage{ "http:ETage" };
					ref_string Location{ "http:Location" };
					ref_string ProxyAuthenticate{ "http:Proxy-Authenticate" };
					ref_string RetryAfter{ "http:Retry-After" };
					ref_string Server{ "http:Server" };
					ref_string Vary{ "http:Vary" };
					ref_string WWWAuthenticate{ "http:WWW-Authenticate" };

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
					ref_string Allow{ "http:Allow" };
					ref_string SetCookie{ "http:Set-Cookie" };
					ref_string ContentEncoding{ "http:Content-Encoding" };
					ref_string ContentLanguage{ "http:Content-Language" };
					ref_string ContentLength{ "http:Content-Length" };
					ref_string ContentLocation{ "http:Content-Location" };
					ref_string ContentMD5{ "http:Content-MD5" };
					ref_string ContentRange{ "http:Content-Range" };
					ref_string ContentType{ "http:Content-Type" };
					ref_string Expires{ "http:Expires" };
					ref_string LastModified{ "http:Last-Modified" };

				public:
					size_t InitFromData(const void* data, size_t len);
				};
			public:
				class HeaderHelp
				{
				public:
					HeaderHelp()
					{
						InitIndex<RequestHeader>(request_);
						InitIndex<ResponseHeader>(response_);

#define XX(code,text)	status_[code] = #text;
						HTTP_STATUS_MAP(XX)
#undef XX
					}
					~HeaderHelp()
					{

					}
				private:
					template<typename T>
					void InitIndex(reflect::Reflect<T>& obj)
					{
						auto layout = obj.GetLayoutFast();
						for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
						{
							const ref_string* field = nullptr;
							try{ field = dynamic_cast<const ref_string*>(reinterpret_cast<const reflect::Reflect_base*>(reinterpret_cast<const char*>(obj.ValueAddr()) + iter->offset)); }
							catch (const std::exception&){}
							if (field == nullptr) continue;

							static_cast<T&>(obj).index_[daxia::string(field->Tag("http")).MakeLower()] = iter->offset;
						}
					}
				public:
					reflect::Reflect<RequestHeader> request_;
					reflect::Reflect<ResponseHeader> response_;
					std::unordered_map<int, std::string> status_;
				};

				class MimeHelp
				{
				public:
					MimeHelp()
					{
#define XX(extension,type)	 mimeMap_[#extension] = type;
						HTTP_MIME_MAP(XX)
#undef XX
					}
					~MimeHelp() {}
				public:
					daxia::string Find(const daxia::string& extension)
					{
						std::unordered_map<daxia::string, daxia::string>::const_iterator iter = mimeMap_.find(extension);
						if (iter != mimeMap_.end())
						{
							return iter->second;
						}
						else
						{
							return daxia::string();
						}
					}
				private:
					std::unordered_map<daxia::string, daxia::string> mimeMap_;
				};
			};

			class HttpServerParser : public HttpParser
			{
			public:
				HttpServerParser(){}
				~HttpServerParser(){}
			public:
				virtual bool Marshal(daxia::net::common::BasicSession* session,
					int msgId,
					const void* data,
					size_t len,
					const daxia::net::common::PageInfo* pageInfo,
					std::vector<daxia::net::common::Buffer>& buffers,
					size_t maxPacketLength
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const void* data,
					size_t len,
					int& msgID,
					daxia::net::common::Buffer& buffer,
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
					int msgId,
					const void* data,
					size_t len,
					const daxia::net::common::PageInfo* pageInfo,
					std::vector<daxia::net::common::Buffer>& buffers,
					size_t maxPacketLength
					) const override;

				virtual Result Unmarshal(daxia::net::common::BasicSession* session,
					const void* data,
					size_t len,
					int& msgID,
					daxia::net::common::Buffer& buffer,
					size_t& packetLen
					) const override;
			};
		}
	}
}

#undef HTTP_STATUS_MAP

#endif // !__DAXIA_NET_COMMON_HTTPPARSER_H

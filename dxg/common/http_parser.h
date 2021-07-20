/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file parser.hpp
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* HTTPЭ�������
*
* �ο����ף�
* MDN https://developer.mozilla.org/zh-CN/docs/Web/HTTP
* ��HTTPЭ�����֪ʶ���������(ͼ��HTTP)�� https://www.cnblogs.com/nyatom/p/13038058.html
*/

#ifndef __DAXIA_DXG_COMMON_HTTPPARSER_H
#define __DAXIA_DXG_COMMON_HTTPPARSER_H
#include "parser.h"
#include <map>
#include "../../reflect/reflect.hpp"
#include "../../string.hpp"

using  daxia::reflect::Reflect;

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			class HttpParser : public Parser
			{
			public:
				HttpParser();
				~HttpParser();
			public:
				virtual bool Marshal(daxia::dxg::common::BasicSession* session,
					const daxia::dxg::common::byte* data,
					int len,
					daxia::dxg::common::shared_buffer& buffer
					) const override;

				virtual Result Unmarshal(daxia::dxg::common::BasicSession* session,
					const daxia::dxg::common::byte* data,
					int len,
					int& msgID,
					daxia::dxg::common::shared_buffer& buffer,
					int& packetLen
					) const override;
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
						daxia::StringA temp(method);
						if (temp.CompareNoCase(Get.Tag("http").c_str()) == 0) return true;
						if (temp.CompareNoCase(Post.Tag("http").c_str()) == 0) return true;
						if (temp.CompareNoCase(Put.Tag("http").c_str()) == 0) return true;
						if (temp.CompareNoCase(Head.Tag("http").c_str()) == 0) return true;
						if (temp.CompareNoCase(Delete.Tag("http").c_str()) == 0) return true;
						if (temp.CompareNoCase(Options.Tag("http").c_str()) == 0) return true;
						if (temp.CompareNoCase(Trace.Tag("http").c_str()) == 0) return true;
						if (temp.CompareNoCase(Connect.Tag("http").c_str()) == 0) return true;

						return false;
					}
				};

				// ͨ���ײ��ֶΣ�General Header Fields�������ĺ���Ӧ����˫������ʹ�õ��ײ�
				struct GeneralHeader
				{
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
					reflect::String* Find(const reflect::String& key);
				protected:
					// ��������
					template<class T>
					void makeIndex(T* p)
					{
						index_.clear();
						for (reflect::String* start = (reflect::String*)p; start < start + sizeof(T); ++start)
						{
							index_[start->Tag("http")] = start;
						}
					}
				protected:
					// �ӿ���ҵ�����
					std::map<std::string, reflect::String*> index_;
				};

				struct RequestHeader : public GeneralHeader
				{
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

					RequestHeader()
					{
						makeIndex(this);
					}
				};

				struct ResponseHeader : public GeneralHeader
				{
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

					ResponseHeader()
					{
						makeIndex(this);
					}
				};
			private:
				static Methods methodsHelp_;
				static RequestHeader requestHeaderHelp_;
				static ResponseHeader responseHeaderHelp_;
			};
		}
	}
}
#endif // !__DAXIA_DXG_COMMON_HTTPPARSER_H

/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2021 漓江里的大虾.
 * All rights reserved.
 *
 * \file http_client.h
 * \author 漓江里的大虾
 * \date 十一月 2021
 *
 */

#ifndef __DAXIA_NET_HTTP_CLIENT_H
#define __DAXIA_NET_HTTP_CLIENT_H

#include <mutex>
#include <condition_variable>
#include "client.h"
#include "common/http_parser.h"
#include "../string.hpp"

namespace daxia
{
	namespace net
	{
		using RequestHeader = daxia::net::common::HttpParser::RequestHeader;

		// Http客户端类
		class HttpClient
		{
		public:
			HttpClient(const char* host, short port = 80);
			HttpClient(const wchar_t* host, short port = 80);
			~HttpClient();
		public:
			// 请求结果
			class Result
			{
				friend HttpClient;
			protected:
				Result() : success_(false){}
			public:
				~Result(){}
				Result(Result&& r)
				{
					response_.Swap(r.response_);
					header_.Swap(r.header_);
					data_.Swap(r.data_);
					std::swap(success_, r.success_);
				}
			public:
				operator bool() const
				{
					return success_;
				}
			public:
				daxia::net::common::HttpParser::ResponseHeader response_;
				daxia::buffer header_;
				daxia::buffer data_;
			private:
				bool success_;
			};
		public:
			Result Get(const char* url, void* data = nullptr, size_t len = 0, const RequestHeader* header = nullptr);
			Result Post(const char* url, void* data, size_t len, const RequestHeader* header = nullptr);
			Result Put(const char* url, void* data, size_t len, const RequestHeader* header = nullptr);
			Result Head(const char* url, void* data, size_t len, const RequestHeader* header = nullptr);
			Result Delete(const char* url, void* data, size_t len, const RequestHeader* header = nullptr);
			Result Options(const char* url, void* data, size_t len, const RequestHeader* header = nullptr);
			Result Trace(const char* url, void* data, size_t len, const RequestHeader* header = nullptr);
			Result Connect(const char* url, void* data, size_t len, const RequestHeader* header = nullptr);
		private:
			void init();
			void setRequest(const RequestHeader* header);
			Result writeMessage(const char* method, const char* url, const void* data, size_t len, const common::PageInfo* pageInfo, size_t maxPacketLength);
		private:
			Client client_;
			std::shared_ptr<daxia::net::common::HttpClientParser> parser_;
			daxia::string host_;
			short port_;
			common::HttpParser::Methods methodsHelper_;
			std::mutex locker_;
			std::condition_variable cv_;
			common::Buffer buffer_;
			bool success_;
			RequestHeader defaultRequest_;
		};
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_HTTP_CLIENT_H

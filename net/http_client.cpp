#include "http_client.h"

namespace daxia
{
	namespace net
	{
		const HttpClient::Result HttpClient::resultCache_;

		HttpClient::HttpClient(const char* host, short port)
			: client_(new daxia::net::Client(true))
			, host_(host)
			, port_(port)
			, success_(false)
		{
			init();
		}

		HttpClient::HttpClient(const wchar_t* host, short port /*= 80*/)
			: client_(new daxia::net::Client(true))
			, host_(daxia::wstring(host).ToAnsi())
			, port_(port)
			, success_(false)
		{
			init();
		}

		HttpClient::~HttpClient()
		{

		}

		daxia::net::HttpClient::Result HttpClient::Get(const char* url, const void* data /*= nullptr*/, size_t len /*= 0*/, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Get.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		daxia::net::HttpClient::Result HttpClient::Post(const char* url, const void* data, size_t len, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Post.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		daxia::net::HttpClient::Result HttpClient::Put(const char* url, const void* data, size_t len, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Put.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		daxia::net::HttpClient::Result HttpClient::Head(const char* url, const void* data, size_t len, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Head.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		daxia::net::HttpClient::Result HttpClient::Delete(const char* url, const void* data, size_t len, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Delete.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		daxia::net::HttpClient::Result HttpClient::Options(const char* url, const void* data, size_t len, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Options.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		daxia::net::HttpClient::Result HttpClient::Trace(const char* url, const void* data, size_t len, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Trace.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		daxia::net::HttpClient::Result HttpClient::Connect(const char* url, const void* data, size_t len, const daxia::net::common::HttpParser::RequestHeader* header /*= nullptr*/)
		{
			if (header) client_->SetUserData(SESSION_USERDATA_REQUEST_INDEX, *header);
			return  writeMessage(methodsHelper_.Connect.Tag("http").GetString(), url, data, len, nullptr, 0);
		}

		void HttpClient::init()
		{
			parser_ = std::shared_ptr<daxia::net::common::HttpClientParser>(new daxia::net::common::HttpClientParser);
			client_->SetParser(parser_);

			client_->Handle(common::DefMsgID_UnHandle, [&](int msgId, const boost::system::error_code& err, const common::Buffer& data)
			{
				if (err)
				{
					std::unique_lock<std::mutex> locker(locker_);
					success_ = !err;
					buffer_.Clear();
					cv_.notify_one();
				}
				else
				{
					if (data.Page().IsStart())
					{
						buffer_.Resize(data.Page().total);
					}

					memcpy((char*)buffer_ + data.Page().startPos, data, data.Size());

					if (data.Page().IsEnd())
					{
						std::unique_lock<std::mutex> locker(locker_);
						success_ = !err;
						cv_.notify_one();
					}
				}

			});
		}

		HttpClient::Result HttpClient::writeMessage(const char* method, const char* url, const void* data, size_t len, const common::PageInfo* pageInfo, size_t maxPacketLength)
		{
			using namespace daxia::net::common;

			// 避免每次构造tag,提升效率
			HttpClient::Result result(resultCache_);

			// 连接
			if (!client_->Connect(host_.GetString(), port_, true)) return result;

			// 协议编码
			std::vector<Buffer> buffers;
			parser_->Marshal(client_.get(), method, url, data, len, pageInfo, buffers, maxPacketLength);

			// 发送数据并同步等待另一通讯端返回
			{
				std::unique_lock<std::mutex> locker(locker_);

				for (const Buffer& buffer : buffers)
				{
					client_->WriteRawData(buffer, buffer.Size());
				}

				cv_.wait(locker);

				// 读取结果
				result.success_ = success_;
				if (success_)
				{
					size_t headerLen = result.response_.InitFromData(buffer_, buffer_.Size());
					if (headerLen != size_t(-1))
					{
						result.header_ = daxia::buffer(buffer_, headerLen);
						result.data_ = daxia::buffer(buffer_ + headerLen, buffer_.Size() - headerLen);
					}
				}
			}
			
			// 断开连接
			client_->Close();

			return result;
		}
	}// namespace net
}// namespace daxia

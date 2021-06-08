#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include "client.h"
#include "../encode/strconv.h"

#define DXG_CLIENT_HANDLER(id,error,date,len) [&](int id, const boost::system::error_code& error, const void* date, int len)

namespace daxia
{
	namespace dxg
	{
		Client::Client()
			: hearbeatInterval_(0)
			, nextTimerId_(0)
		{
			// ����ʵ������
			static initHelper helper;
			initHelper_ = &helper;

			initSocket(BasicSession::socket_ptr(new socket(initHelper_->netIoService_)));
			parser_ = common::Parser::ptr(new common::DefaultParser);
		}

		Client::~Client()
		{
			Close();

			// �����sock_����ʱ���������netIoService_,����ʹ֮��ǰ����
			initSocket(BasicSession::socket_ptr());

			lock_guard locker(scheduleLocker_);
			for (auto iter = timers_.begin(); iter != timers_.end(); ++iter)
			{
				iter->second->cancel();
				delete iter->second;
			}
			timers_.clear();
		}

			Client::initHelper::initHelper()
			{
				startLogicThread();
				startIoThread();
			}

			Client::initHelper::~initHelper()
			{
				stopLogicThread();
				stopIoThread();
			}

			void Client::initHelper::startLogicThread()
			{
				int coreCount = getCoreCount();

				//for (int i = 0; i < coreCount * 2; ++i)
				for (int i = 0; i < 1; ++i)
				{
					logicThreads_.push_back(std::thread([&]()
					{
						boost::asio::io_service::work worker(logicIoService_);
						logicIoService_.run();
					}));
				}
			}

			void Client::initHelper::stopLogicThread()
			{
				logicIoService_.stop();

				for (size_t i = 0; i < logicThreads_.size(); ++i)
				{
					if (logicThreads_[i].joinable())
					{
						logicThreads_[i].join();
					}
				}

				logicThreads_.clear();
			
			}

			void Client::initHelper::startIoThread()
			{
				int coreCount = getCoreCount();

				for (int i = 0; i < coreCount * 2; ++i)
				{
					ioThreads_.push_back(std::thread([&]()
					{
						boost::asio::io_service::work worker(netIoService_);
						netIoService_.run();
					}));
				}
			}

			void Client::initHelper::stopIoThread()
			{
				netIoService_.stop();

				for (size_t i = 0; i < ioThreads_.size(); ++i)
				{
					if (ioThreads_[i].joinable())
					{
						try
						{
							ioThreads_[i].join();
						}
						catch (...)
						{
						}
					}
				}
				netIoService_.reset();
			}

			int Client::initHelper::getCoreCount() const
			{
				int count = 1; // ����һ��

#if !defined (_WIN32) && !defined (_WIN64) 
				count = sysconf(_SC_NPROCESSORS_CONF);
#else
				SYSTEM_INFO si;
				GetSystemInfo(&si);
				count = si.dwNumberOfProcessors;
#endif  

				return count;
			}

			void Client::onPacket(const boost::system::error_code& error, int msgId, const common::shared_buffer& buffer)
			{
				pushLogciMessage(LogicMessage(error, msgId, buffer));
			}

			void Client::Handle(int msgId, handler h)
			{
				handler_[msgId] = h;
			}

			void Client::EnableHeartbeat(unsigned long milliseconds)
			{
				hearbeatInterval_ = milliseconds;
			}

			void Client::Connect(const char* ip, short port)
			{
				Close();
				endpoint_ = endpoint(boost::asio::ip::address::from_string(ip), port);
				doConnect();
			}

			void Client::Connect(const wchar_t* ip, short port)
			{
				std::string ip2 = daxia::encode::Strconv::Unicode2Ansi(ip);
				Connect(ip2.c_str(), port);
			}

			long long Client::Schedule(scheduleFunc func,unsigned long firstDuration, unsigned long loopDuration)
			{
				lock_guard locker(scheduleLocker_);

				long long id = nextTimerId_;

				timers_[id] = new boost::asio::deadline_timer(initHelper_->logicIoService_, boost::posix_time::milliseconds(firstDuration));
				timers_[id]->async_wait(std::bind(&Client::asyncWaitCB, this, func, id, loopDuration, std::placeholders::_1));

				++nextTimerId_;

				return id;
			}

			long long Client::ScheduleOnce(scheduleFunc func, unsigned long duration)
			{
				lock_guard locker(scheduleLocker_);

				long long id = nextTimerId_;

				timers_[id] = new boost::asio::deadline_timer(initHelper_->logicIoService_, boost::posix_time::milliseconds(duration));
				timers_[id]->async_wait([&, func,id](const boost::system::error_code& ec)
				{
					if (!ec)
					{
						func();
					}

					lock_guard locker(scheduleLocker_);
					std::map<long long, boost::asio::deadline_timer*>::iterator iter = timers_.find(id);
					if (iter != timers_.end())
					{
						delete iter->second;
						timers_.erase(iter);
					}
				});

				++nextTimerId_;

				return id;
			}

			void Client::Unschedule(long long scheduleID)
			{
				lock_guard locker(scheduleLocker_);

				std::map<long long, boost::asio::deadline_timer*>::iterator iter = timers_.find(scheduleID);
				if (iter != timers_.end())
				{
					iter->second->cancel();
					delete iter->second;
					timers_.erase(iter);
				}
			}

			void Client::UnscheduleAll()
			{
				lock_guard locker(scheduleLocker_);
				
				for (auto iter = timers_.begin(); iter != timers_.end(); ++iter)
				{
					iter->second->cancel();
					delete iter->second;
				}

				timers_.clear();
			}

			void Client::doConnect()
			{
				getSocket()->async_connect(endpoint_, [&](const boost::system::error_code& ec)
				{
					pushLogciMessage(LogicMessage(ec, common::DefMsgID_Connect, common::shared_buffer()));

					if (!ec)
					{
						postRead();
					}
				});
			}

			void Client::hearbeat()
			{
				using namespace std::chrono;

				if (hearbeatInterval_ != 0)
				{
					time_point<system_clock, milliseconds> now = time_point_cast<milliseconds>(system_clock::now());
					if ((now - GetLastWriteTime()).count() >= hearbeatInterval_)
					{
						WriteMessage(nullptr, 0);
					}
				}
			}

			void Client::clearMessage()
			{
				// clear messages_
				logicMsgLocker_.lock();
				std::queue<LogicMessage> empty;
				logicMsgs_.swap(empty);
				logicMsgLocker_.unlock();
			}

			void Client::pushLogciMessage(const LogicMessage& msg)
			{
				lock_guard locker(logicMsgLocker_);
				logicMsgs_.push(msg);

				initHelper_->logicIoService_.post([&]()
				{
					// �ַ���Ϣ
					LogicMessage msg;
					bool hasMsg = false;
					logicMsgLocker_.lock();
					if (!logicMsgs_.empty())
					{
						hasMsg = true;
						msg = logicMsgs_.front();
						logicMsgs_.pop();
					}
					logicMsgLocker_.unlock();

					if (hasMsg)
					{
						if (msg.msgID == common::DefMsgID_DisConnect)
						{
							Close();
						}

						auto iter = handler_.find(msg.msgID);
						if (iter != handler_.end())
						{
							iter->second(msg.msgID, msg.error, msg.buffer.get(), msg.buffer.size());
						}
						else
						{
							auto iter = handler_.find(common::DefMsgID_UnHandle);
							if (iter != handler_.end())
							{
								iter->second(msg.msgID, msg.error, msg.buffer.get(), msg.buffer.size());
							}
						}
					}
				});
			}

			void Client::asyncWaitCB(scheduleFunc func, long long id, long long duration, const boost::system::error_code& ec)
			{
				if (!ec)
				{
					func();

					std::map<long long, boost::asio::deadline_timer*>::iterator iter = timers_.find(id);
					if (iter != timers_.end())
					{
						iter->second->expires_at(iter->second->expires_at() + boost::posix_time::milliseconds(duration));
						iter->second->async_wait(std::bind(&Client::asyncWaitCB, this, func, id, duration, std::placeholders::_1));
					}
				}
			}
	}// namespace dxg
}// namespace daxia
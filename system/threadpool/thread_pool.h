/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file thread_pool.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ͨ���̳߳�
*
*/
#ifndef __DAXIA_SYSTEM_THREADPOOL_H
#define __DAXIA_SYSTEM_THREADPOOL_H
#include <functional>
#include <thread>
#include <vector>
#include <future>
#include <boost/asio.hpp>

namespace daxia
{
	namespace system
	{
		class ThreadPool
		{
		public:
			ThreadPool(size_t threadCount = 0 // �߳���������Ϊ0��Ϊcpu����*2
				);
			~ThreadPool();
		public:
			// �ַ�һ������,��������������б��̳߳ؽ��Զ�Ѱ��һ�����е��߳�ִ�и�����
			template<typename T>
			void Post(std::packaged_task<T()>& task)
			{
				ios_.post(std::ref(task));
			}

			// �ַ�һ������������ô˷������߳�Ϊ�̳߳��е��߳�������ִ�У�����ͬPost��
			template<typename T>
			void Dispatch(std::packaged_task<T()>& task)
			{
				ios_.dispatch(std::ref(task));
			}
		public:
			// ��ȡCPU��������
			static size_t GetCpuCoreCount();
		private:
			std::vector<std::thread> threads_;
			boost::asio::io_service ios_;
		private:
			void start(size_t count);
			void stop();
		};

	}
}

#endif // !__DAXIA_SYSTEM_THREADPOOL_H
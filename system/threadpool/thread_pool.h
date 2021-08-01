/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file datetime.h
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
#include <boost/asio.hpp>

namespace daxia
{
	namespace system
	{
		class ThreadPool
		{
		public:
			ThreadPool(bool autoStart = true);
			~ThreadPool();
		public:
			// ����ָ���������̣߳����ȴ�Dispathch�Ĺ�������
			// count: �߳��������Ƽ�ϵͳcpu���� * ��
			void Start(size_t count);

			// ֹͣ���е��߳�
			void Stop();

			// �ַ�һ�������̳߳ؽ��Զ�Ѱ��һ�����е��߳�ִ�и�����
			// work: ��Ҫִ�е�����
			void Dispathch(std::function<void()> work);
		public:
			// ��ȡCPU��������
			static size_t GetCpuCoreCount();
		private:
			std::vector<std::thread> threads_;
			boost::asio::io_service ios_;
		};
	}
}

#endif // !__DAXIA_SYSTEM_THREADPOOL_H
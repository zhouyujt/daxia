/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file basic_driver.h
* \author 漓江里的大虾
* \date 八月 2021
*
* 数据库驱动接口
*
*/
#ifndef __DAXIA_DATABASE_DRIVER_BASIC_DRIVER_H
#define __DAXIA_DATABASE_DRIVER_BASIC_DRIVER_H
#include <functional>
#include <memory>
#include "basic_recordset.h"
#include "../../string.hpp"
#include "../../system/threadpool/thread_pool.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			class BasicDriver
			{
			public:
				BasicDriver(const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw, daxia::system::ThreadPool* tp);
				virtual ~BasicDriver();
			public:
				virtual	bool Connect() = 0;
				virtual bool CoConnect() = 0;
				virtual std::shared_ptr<BasicRecordset> Excute(const daxia::string& sql) = 0;
				virtual std::shared_ptr<BasicRecordset> CoExcute(const daxia::string& sql) = 0;
				virtual daxia::string GetLastError() const = 0;
				virtual daxia::string TypeName(const std::type_info& type) const = 0;
				virtual long long  ScopeIdentity() = 0;
				virtual void PushBlob(const daxia::buffer& blob) = 0;
			protected:
				daxia::string host_;
				daxia::string db_;
				daxia::string user_;
				daxia::string psw_;
				unsigned short port_;
				daxia::system::ThreadPool* tp_;
			};
		}
	}
}
#endif	// !__DAXIA_DATABASE_DRIVER_BASIC_DRIVER_H
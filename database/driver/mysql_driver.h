/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file mysql.h
* \author 漓江里的大虾
* \date 八月 2021
*
* mysql 驱动 
*
*/
#ifdef __DAXIA_DATABASE_DRIVER_USE_MYSQL
#ifndef __DAXIA_DATABASE_DRIVER_MYSQL_H
#define __DAXIA_DATABASE_DRIVER_MYSQL_H
#include <mysql.h>
#include <memory>
#include "basic_driver.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			class MySQLDriver : public BasicDriver
			{
			public:
				MySQLDriver(const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw, std::shared_ptr<daxia::system::ThreadPool> tp);
				~MySQLDriver();
			public:
				static void Init();
				static void Uninit();
				virtual bool Connect() override;
				virtual bool CoConnect() override;
				virtual std::shared_ptr<BasicRecordset> Excute(const daxia::string& sql) override;
				virtual std::shared_ptr<BasicRecordset> CoExcute(const daxia::string& sql) override;
				virtual daxia::string GetLastError() const override;
				virtual daxia::string TypeName(const std::type_info& type) const override;
				virtual long long  ScopeIdentity() override;
				virtual void PushBlob(const daxia::buffer& blob) override;
			private:
				MYSQL mysql_;
			};
		}
	}
}
#endif	// !__DAXIA_DATABASE_DRIVER_MYSQL_H
#endif	// !__DAXIA_DATABASE_DRIVER_USE_MYSQL
/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file sqlite_driver.h
* \author 漓江里的大虾
* \date 八月 2021
*
* sqlite3 驱动
*
*/
#ifdef __DAXIA_DATABASE_DRIVER_USE_SQLITE
#ifndef __DAXIA_DATABASE_DRIVER_SQLITE_H
#define __DAXIA_DATABASE_DRIVER_SQLITE_H
#include <vector>
#include "basic_driver.h"
#include "sqlite3/sqlite3.h"
#include "../../string.hpp"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			class SqliteDriver : public BasicDriver
			{
			public:
				SqliteDriver(const daxia::string& db, std::shared_ptr<daxia::system::ThreadPool> tp);
				~SqliteDriver();
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
				void setLastError(bool clean = false);
			private:
				sqlite3* sqlite_;
				daxia::string lastError_;
				std::vector<daxia::buffer> blob_;
			};
		}
	}
}
#endif // !__DAXIA_DATABASE_DRIVER_SQLITE_H
#endif // !__DAXIA_DATABASE_DRIVER_USE_SQLITE
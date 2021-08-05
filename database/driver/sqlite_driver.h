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
#ifndef __DAXIA_DATABASE_DRIVER_SQLITE_H
#define __DAXIA_DATABASE_DRIVER_SQLITE_H
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
				SqliteDriver(const daxia::string& db);
				~SqliteDriver();
			public:
				virtual void Init() override;
				virtual void Uninit() override;
				virtual bool Connnect() override;
				virtual void ConnnectAsync(connect_callback cb) override;
				virtual std::shared_ptr<BasicRecordset> Excute(const daxia::string& sql) override;
				virtual void ExcuteAsync(const daxia::string& sql, excute_callback cb) override;
				virtual daxia::string GetLastError() const override;
				virtual daxia::string TypeName(const std::type_info& type) const override;
			private:
				void setLastError(bool clean = false);
			private:
				sqlite3* sqlite_;
				daxia::string lastError_;
			};

			class InitHelperSqliteDriver : private SqliteDriver
			{
				friend SqliteDriver;
			private:
				InitHelperSqliteDriver()
					: SqliteDriver("")
				{
					Init();
				}
			protected:
				~InitHelperSqliteDriver()
				{
					Uninit();
				}
			};
		}
	}
}
#endif // !__DAXIA_DATABASE_DRIVER_SQLITE_H
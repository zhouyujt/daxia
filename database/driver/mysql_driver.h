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
#ifndef __DAXIA_DATABASE_DRIVER_MYSQL_H
#define __DAXIA_DATABASE_DRIVER_MYSQL_H
#include <mysql.h>
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
				MySQLDriver(const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw);
				~MySQLDriver();
			public:
				virtual void Init() override;
				virtual void Uninit() override;
				virtual bool Connnect() override;
				virtual void ConnnectAsync(connect_callback cb) override;
				virtual std::shared_ptr<BasicRecordset> Excute(const daxia::string& sql) override;
				virtual void ExcuteAsync(const daxia::string& sql, excute_callback cb) override;
				virtual daxia::string GetLastError() const override;
			private:
				MYSQL mysql_;
			};

			class InitHelperMySQL : private MySQLDriver
			{
				friend MySQLDriver;
			private:
				InitHelperMySQL()
					: MySQLDriver("",0,"","","")
				{
					Init();
				}
			protected:
				~InitHelperMySQL()
				{
					Uninit();
				}
			};
		}
	}
}
#endif	// !__DAXIA_DATABASE_DRIVER_MYSQL_H
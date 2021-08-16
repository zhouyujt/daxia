/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file mysql_recordset.h
* \author 漓江里的大虾
* \date 八月 2021
*
* mysql 数据集接口实现
*
*/
#ifdef __DAXIA_DATABASE_DRIVER_USE_MYSQL
#ifndef __DAXIA_DATABASE_DRIVER_MYSQL_RECORDSET_H
#define __DAXIA_DATABASE_DRIVER_MYSQL_RECORDSET_H
#include <mysql.h>
#include "basic_recordset.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			class MySQLRecordset : public BasicRecordset
			{
			public:
				MySQLRecordset(MYSQL_RES* recordset, MYSQL* mysql);
				~MySQLRecordset();
			public:
				virtual bool Eof() override;
				virtual void Next() override;
				virtual size_t Count() override;
				virtual daxia::buffer GetRawData(const char* field) override;
			protected:
				virtual void GetField(const char* field, db_tinyint& v) override;
				virtual void GetField(const char* field, db_int& v) override;
				virtual void GetField(const char* field, db_bigint& v) override;
				virtual void GetField(const char* field, db_float& v) override;
				virtual void GetField(const char* field, db_double& v) override;
				virtual void GetField(const char* field, db_text& v) override;
				virtual void GetField(const char* field, db_blob& v) override;
				virtual void GetField(const char* field, db_datetime& v) override;
			private:
				MYSQL_RES* recordset_;
				MYSQL_ROW row_;
				MYSQL_FIELD* fields_;
				bool isEof_;
			};
		}
	}
}
#endif // !__DAXIA_DATABASE_DRIVER_MYSQL_RECORDSET_H
#endif // !__DAXIA_DATABASE_DRIVER_USE_MYSQL
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
#ifndef __DAXIA_DATABASE_DRIVER_SQLITE_RECORDSET_H
#define __DAXIA_DATABASE_DRIVER_SQLITE_RECORDSET_H
#include "sqlite3/sqlite3.h"
#include "basic_recordset.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			class SqliteRecordset : public BasicRecordset
			{
			public:
				SqliteRecordset(sqlite3* sqlite, sqlite3_stmt* stmt);
				~SqliteRecordset();
			public:
				virtual bool Eof() override;
				virtual void Next() override;
				virtual size_t Count() override;
				virtual long long ScopeIdentity() override;
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
				int GetFieldIndex(const char* field);
			private:
				sqlite3* sqlite_;
				sqlite3_stmt* stmt_;
				bool isEof_;
			};
		}
	}
}
#endif // !__DAXIA_DATABASE_DRIVER_SQLITE_RECORDSET_H
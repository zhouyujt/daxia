/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file mysql_recordset.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* mysql ���ݼ��ӿ�ʵ��
*
*/
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
				virtual long long ScopeIdentity() override;
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
				daxia::string GetField(const char* field);
			private:
				MYSQL_RES* recordset_;
				MYSQL* mysql_;
				MYSQL_ROW row_;
				MYSQL_FIELD* fields_;
				bool isEof_;
			};
		}
	}
}
#endif // !__DAXIA_DATABASE_DRIVER_MYSQL_RECORDSET_H
#ifdef __DAXIA_DATABASE_DRIVER_USE_MYSQL
#include <string.h>	// strcmp
#include "mysql_recordset.h"
#include "../../encode/hex.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			MySQLRecordset::MySQLRecordset(MYSQL_RES* recordset, MYSQL* mysql)
				: recordset_(recordset)
				, mysql_(mysql)
				, fields_(nullptr)
				, isEof_(false)
			{
				Next();
			}

			MySQLRecordset::~MySQLRecordset()
			{
				if (recordset_)
				{
					mysql_free_result(recordset_);
				}
			}

			bool MySQLRecordset::Eof()
			{
				return isEof_;
			}

			void MySQLRecordset::Next()
			{
				row_ = mysql_fetch_row(recordset_);
				if (row_ == nullptr)
				{
					isEof_ = true;
				}
			}

			size_t MySQLRecordset::Count()
			{
				return mysql_num_rows(recordset_);
			}

			long long MySQLRecordset::ScopeIdentity()
			{
				return mysql_insert_id(mysql_);
			}

			daxia::buffer MySQLRecordset::GetRawData(const char* fieldName)
			{
				daxia::buffer result;

				unsigned int fieldsNum = mysql_num_fields(recordset_);
				MYSQL_FIELD* field;
				int index = -1;
				mysql_field_seek(recordset_, 0);
				for (int i = 0; (field = mysql_fetch_field(recordset_)) != nullptr; ++i)
				{
					if (strcmp(fieldName, field->name) == 0)
					{
						index = i;
						break;
					}
				}

				if (index != -1)
				{
					result = row_[index];
				}

				return result;
			}

			void MySQLRecordset::GetField(const char* field, db_tinyint& v)
			{
				daxia::buffer result = GetRawData(field);
				v = result.NumericCast<char>();
			}

			void MySQLRecordset::GetField(const char* field, db_int& v)
			{
				daxia::buffer result = GetRawData(field);
				v = result.NumericCast<int>();
			}

			void MySQLRecordset::GetField(const char* field, db_bigint& v)
			{
				daxia::buffer result = GetRawData(field);
				v = result.NumericCast<long long>();
			}

			void MySQLRecordset::GetField(const char* field, db_float& v)
			{
				daxia::buffer result = GetRawData(field);
				v = result.NumericCast<float>();
			}

			void MySQLRecordset::GetField(const char* field, db_double& v)
			{
				daxia::buffer result = GetRawData(field);
				v = result.NumericCast<double>();
			}

			void MySQLRecordset::GetField(const char* field, db_text& v)
			{
				v = GetRawData(field);
			}

			void MySQLRecordset::GetField(const char* field, db_blob& v)
			{
				v = daxia::encode::Hex::FromString(GetRawData(field));
			}

			void MySQLRecordset::GetField(const char* field, db_datetime& v)
			{
				v = daxia::system::DateTime(GetRawData(field).GetString());
			}
		}
	}
}
#endif // !__DAXIA_DATABASE_DRIVER_USE_MYSQL
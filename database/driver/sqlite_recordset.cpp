#include "sqlite_recordset.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			SqliteRecordset::SqliteRecordset(sqlite3* sqlite, sqlite3_stmt* stmt)
				: sqlite_(sqlite)
				, stmt_(stmt)
				, isEof_(stmt == nullptr)
			{
				
			}

			SqliteRecordset::~SqliteRecordset()
			{
				if (stmt_)
				{
					sqlite3_finalize(stmt_);
				}
			}

			bool SqliteRecordset::Eof()
			{
				return isEof_;
			}

			void SqliteRecordset::Next()
			{
				if (sqlite3_step(stmt_) != SQLITE_ROW)
				{
					isEof_ = true;
				}
			}

			size_t SqliteRecordset::Count()
			{
				throw "ипн╢й╣ож";
			}

			long long SqliteRecordset::ScopeIdentity()
			{
				return sqlite3_last_insert_rowid(sqlite_);
			}

			void SqliteRecordset::GetField(const char* field, db_tinyint& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					v = static_cast<char>(sqlite3_column_int(stmt_, index));
				}
			}

			void SqliteRecordset::GetField(const char* field, db_int& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					v = sqlite3_column_int(stmt_, index);
				}
			}

			void SqliteRecordset::GetField(const char* field, db_bigint& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					v = sqlite3_column_int64(stmt_, index);
				}
			}

			void SqliteRecordset::GetField(const char* field, db_float& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					v = static_cast<float>(sqlite3_column_double(stmt_, index));
				}
			}

			void SqliteRecordset::GetField(const char* field, db_double& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					v = sqlite3_column_double(stmt_, index);
				}
			}

			void SqliteRecordset::GetField(const char* field, db_text& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					v = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, index));
				}
			}

			void SqliteRecordset::GetField(const char* field, db_blob& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					const void* data = sqlite3_column_blob(stmt_, index);
					int size = sqlite3_column_bytes(stmt_, index);
					v = daxia::string(reinterpret_cast<const char*>(data), size);
				}
			}

			void SqliteRecordset::GetField(const char* field, db_datetime& v)
			{
				int index = GetFieldIndex(field);
				if (index != -1)
				{
					v = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, index));
				}
			}

			int SqliteRecordset::GetFieldIndex(const char* field)
			{
				int index = -1;

				int fieldsNum = sqlite3_column_count(stmt_);
				for (int i = 0; i < fieldsNum; ++i)
				{
					if (strcmp(field, sqlite3_column_name(stmt_,i)) == 0)
					{
						index = i;
						break;
					}
				}

				return index;
			}

		}
	}
}
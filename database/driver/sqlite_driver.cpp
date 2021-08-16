#ifdef __DAXIA_DATABASE_DRIVER_USE_SQLITE
#include "sqlite_driver.h"
#include "../../system/windows/path.h"
#include "sqlite_recordset.h"
namespace daxia
{
	namespace database
	{
		namespace driver
		{

			SqliteDriver::SqliteDriver(const daxia::string& db)
				: BasicDriver("",0,db,"","")
				, sqlite_(nullptr)
			{
			}

			SqliteDriver::~SqliteDriver()
			{
				if (sqlite_)
				{
					sqlite3_close(sqlite_);
					sqlite_ = nullptr;
				}
			}

			void SqliteDriver::Init()
			{
				// windows 需设置临时目录
#ifdef _MSC_VER
				daxia::tstring path = daxia::system::windows::Path::GetTempPath();
				sqlite3_temp_directory = sqlite3_mprintf("%s", path.ToUtf8().GetString());
#endif
			}

			void SqliteDriver::Uninit()
			{
#ifdef _MSC_VER
				if (sqlite3_temp_directory)
				{
					sqlite3_free(sqlite3_temp_directory);
					sqlite3_temp_directory = nullptr;
				}
#endif
			}

			bool SqliteDriver::Connnect()
			{
				return sqlite3_open(db_.ToUtf8().GetString(), &sqlite_) == SQLITE_OK;
			}

			void SqliteDriver::ConnnectAsync(connect_callback cb)
			{

			}

			std::shared_ptr<daxia::database::driver::BasicRecordset> SqliteDriver::Excute(const daxia::string& sql)
			{
				if (sqlite_ == nullptr)
				{
					if (!Connnect())
					{
						setLastError();
						return std::shared_ptr<BasicRecordset>();
					}
				}

				daxia::string temp = sql.ToUtf8();

				// 编译sql语句
				sqlite3_stmt* stmt = nullptr;
				const char* tail = nullptr;
				if (sqlite3_prepare_v2(sqlite_, temp.GetString(), temp.GetLength(), &stmt, &tail) != SQLITE_OK)
				{
					setLastError();
					return std::shared_ptr<BasicRecordset>();
				}

				// 处理blob
				for (size_t i = 0; i < blob_.size(); ++i)
				{
					sqlite3_bind_blob(stmt, i + 1, blob_[i].GetString(), blob_[i].GetLength(), nullptr);
				}
				blob_.clear();

				// 执行
				int rc = sqlite3_step(stmt);

				if (rc == SQLITE_DONE)
				{
					setLastError(true);
					return std::shared_ptr<BasicRecordset>(new SqliteRecordset(sqlite_, nullptr));
				}
				else if (rc == SQLITE_ROW)
				{
					setLastError(true);
					return std::shared_ptr<BasicRecordset>(new SqliteRecordset(sqlite_, stmt));
				}
				else
				{
					setLastError();
					sqlite3_finalize(stmt);
					return std::shared_ptr<BasicRecordset>();
				}
			}

			void SqliteDriver::ExcuteAsync(const daxia::string& sql, excute_callback cb)
			{

			}

			daxia::string SqliteDriver::GetLastError() const
			{
				return lastError_;
			}

			daxia::string SqliteDriver::TypeName(const std::type_info& type) const
			{
				if (type == typeid(db_tinyint)) return "INTEGER";
				else if (type == typeid(db_int)) return "INTEGER";
				else if (type == typeid(db_bigint)) return "INTEGER";
				else if (type == typeid(db_float)) return "REAL";
				else if (type == typeid(db_double)) return "REAL";
				else if (type == typeid(db_text)) return "TEXT";
				else if (type == typeid(db_blob)) return "BLOB";
				else if (type == typeid(db_datetime)) return "NUMERIC";
				else return "";
			}

			void SqliteDriver::PushBlob(const daxia::buffer& blob)
			{
				blob_.push_back(blob);
			}

			void SqliteDriver::setLastError(bool clean /*= false*/)
			{
				if (clean)
				{
					lastError_.Empty();
				}
				else
				{
					lastError_ = sqlite3_errmsg(sqlite_);
					lastError_.Utf8() = true;
#ifdef _MSC_VER
					lastError_ = lastError_.ToAnsi();
#endif
				}
			}

		}
	}
}
#endif // !__DAXIA_DATABASE_DRIVER_USE_SQLITE
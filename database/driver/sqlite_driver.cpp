
#include "sqlite_driver.h"
#include "../../system/windows/path.h"
#include "sqlite_recordset.h"
namespace daxia
{
	namespace database
	{
		namespace driver
		{

			Sqlite3Driver::Sqlite3Driver(const daxia::string& db)
				: BasicDriver("",0,db,"","")
				, sqlite_(nullptr)
			{

			}

			Sqlite3Driver::~Sqlite3Driver()
			{
				if (sqlite_)
				{
					sqlite3_close(sqlite_);
					sqlite_ = nullptr;
				}
			}

			void Sqlite3Driver::Init()
			{
				// windows 需设置临时目录
#ifdef _MSC_VER
				daxia::tstring path = daxia::system::windows::Path::GetTempPath();
#	ifdef UNICODE
				sqlite3_temp_directory = sqlite3_mprintf("%s", path.Unicode2Utf8().GetString());
#	else
				sqlite3_temp_directory = sqlite3_mprintf("%s", path.Ansi2Utf8().GetString());
#	endif
#endif
			}

			void Sqlite3Driver::Uninit()
			{
#ifdef _MSC_VER
				if (sqlite3_temp_directory)
				{
					sqlite3_free(sqlite3_temp_directory);
					sqlite3_temp_directory = nullptr;
				}
#endif
			}

			bool Sqlite3Driver::Connnect()
			{
#ifdef _MSC_VER
				return sqlite3_open(db_.Ansi2Utf8().GetString(), &sqlite_) == SQLITE_OK;
#else
				return sqlite3_open(db_.GetString(), &sqlite_) == SQLITE_OK;
#endif
			}

			void Sqlite3Driver::ConnnectAsync(connect_callback cb)
			{

			}

			std::shared_ptr<daxia::database::driver::BasicRecordset> Sqlite3Driver::Excute(const daxia::string& sql)
			{
#ifdef _MSC_VER
				daxia::string temp = sql.Ansi2Utf8();
#else
				const daxia::string& temp = sql;
#endif

				// 编译sql语句
				sqlite3_stmt* stmt = nullptr;
				const char* tail = nullptr;
				if (sqlite3_prepare_v2(sqlite_, temp.GetString(), temp.GetLength(), &stmt, &tail) != SQLITE_OK) return std::shared_ptr<BasicRecordset>();

				// 执行
				int rc = sqlite3_step(stmt);

				if (rc == SQLITE_DONE)
				{
					return std::shared_ptr<BasicRecordset>(new SqliteRecordset(sqlite_, nullptr));
				}
				else if (rc == SQLITE_ROW)
				{
					return std::shared_ptr<BasicRecordset>(new SqliteRecordset(sqlite_, stmt));
				}
				else
				{
					sqlite3_finalize(stmt);
					return std::shared_ptr<BasicRecordset>();
				}
			}

			void Sqlite3Driver::ExcuteAsync(const daxia::string& sql, excute_callback cb)
			{

			}

			daxia::string Sqlite3Driver::GetLastError() const
			{
				daxia::string err;
				err = sqlite3_errmsg(sqlite_);
#ifdef _MSC_VER
				return err.Utf82Ansi();
#else
				return err;
#endif
			}

		}
	}
}
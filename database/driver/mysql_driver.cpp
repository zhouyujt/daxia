#ifdef __DAXIA_DATABASE_DRIVER_USE_MYSQL
#include <memory.h>	// memset
#include "mysql_driver.h"
#include "mysql_recordset.h"
#include "../../system/coroutine.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			MySQLDriver::MySQLDriver(const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw, std::shared_ptr<daxia::system::ThreadPool> tp)
				: BasicDriver(host, port, db, user, psw, tp)
			{
				memset(&mysql_, 0, sizeof(mysql_));
			}

			MySQLDriver::~MySQLDriver()
			{
				mysql_close(&mysql_);
			}

			void MySQLDriver::Init()
			{
				mysql_library_init(0, nullptr, nullptr);
			}

			void MySQLDriver::Uninit()
			{
				mysql_library_end();
			}

			bool MySQLDriver::Connect()
			{
				mysql_init(&mysql_);
				mysql_options(&mysql_, MYSQL_INIT_COMMAND, "SET autocommit=0");
				return mysql_real_connect(&mysql_, host_.GetString(), user_.GetString(), psw_.GetString(), db_.GetString(), port_, nullptr, 0) != nullptr;
			}

			bool MySQLDriver::CoConnect()
			{
				std::packaged_task<bool()> task([&]()
				{
					return Connect();
				});

				tp_->Post(task);

				std::future<bool> result = task.get_future();
				daxia::system::this_coroutine::CoWait(WAIT_FUTURE(result));

				return result.get();
			}

			std::shared_ptr<BasicRecordset> MySQLDriver::Excute(const daxia::string& sql)
			{
				if (mysql_ping(&mysql_) != 0)
				{
					if (!Connnect()) return std::shared_ptr<BasicRecordset>();
				}

				if (mysql_real_query(&mysql_, sql.GetString(), sql.GetLength()) != 0) return std::shared_ptr<BasicRecordset>();

				MYSQL_RES* recordset = mysql_store_result(&mysql_);
				if (recordset == nullptr) return std::shared_ptr<BasicRecordset>();

				return std::shared_ptr<BasicRecordset>(new MySQLRecordset(recordset,&mysql_));
			}

			std::shared_ptr<BasicRecordset> MySQLDriver::CoExcute(const daxia::string& sql)
			{
				std::packaged_task<std::shared_ptr<BasicRecordset>()> task([&]()
				{
					return Excute(sql);
				});

				tp_->Post(task);

				std::future<std::shared_ptr<BasicRecordset>> result = task.get_future();
				daxia::system::this_coroutine::CoWait(WAIT_FUTURE(result));

				return result.get();
			}

			daxia::string MySQLDriver::GetLastError() const
			{
				return mysql_error(const_cast<MYSQL*>(&mysql_));
			}

			daxia::string MySQLDriver::TypeName(const std::type_info& type) const
			{
				if (type == typeid(db_tinyint)) return "TINYINT";
				else if (type == typeid(db_int)) return "INT";
				else if (type == typeid(db_bigint)) return "BIGINT";
				else if (type == typeid(db_float)) return "FLOAT";
				else if (type == typeid(db_double)) return "DOUBLE";
				else if (type == typeid(db_text)) return "VARCHAR";
				else if (type == typeid(db_blob)) return "LONGBLOB";
				else if (type == typeid(db_datetime)) return "DATETIME";
				else return "";
			}

			long long MySQLDriver::ScopeIdentity()
			{
				return mysql_insert_id(mysql_);
			}

			void MySQLDriver::PushBlob(const daxia::buffer& blob)
			{
				throw "ипн╢й╣ож";
			}

		}
	}
}
#endif	// !__DAXIA_DATABASE_DRIVER_USE_MYSQL
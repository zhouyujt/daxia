#include "mysql_command.h"
#include "driver/mysql/include/mysql.h"

namespace daxia
{
	namespace database
	{
		MysqlCommand::MysqlCommand(const daxia::string& host, const daxia::string& db, const daxia::string& user, const daxia::string& psw)
		{
			MYSQL mysql;
			mysql_init(&mysql);
			if (!mysql_real_connect(&mysql, host.GetString(), user.GetString(), psw.GetString(), db.GetString(), 3306, NULL, 0))
			{

			}
		}

		MysqlCommand::~MysqlCommand()
		{

		}

		std::shared_ptr<daxia::database::Recordset> MysqlCommand::Excute(const daxia::string& sql)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	}
}
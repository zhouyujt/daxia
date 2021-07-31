#include "sqlserver_command.h"

namespace daxia
{
	namespace database
	{

		SqlserverCommand::SqlserverCommand(const daxia::string& host, const daxia::string& db, const daxia::string& user, const daxia::string& psw)
		{

		}

		SqlserverCommand::~SqlserverCommand()
		{

		}

		std::shared_ptr<daxia::database::Recordset> SqlserverCommand::Excute(const daxia::string& sql)
		{
			throw "ипн╢й╣ож";
		}

	}
}
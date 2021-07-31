#include "sqlite_command.h"

namespace daxia
{
	namespace database
	{

		SqliteCommand::SqliteCommand(const daxia::string& host, const daxia::string& db, const daxia::string& user, const daxia::string& psw)
		{

		}

		SqliteCommand::~SqliteCommand()
		{

		}

		std::shared_ptr<daxia::database::Recordset> SqliteCommand::Excute(const daxia::string& sql)
		{
			throw "ипн╢й╣ож";
		}

	}
}
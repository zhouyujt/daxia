/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file sqlite_command.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* sqlite command �ӿ�ʵ��
*
*/
#ifndef __DAXIA_DATABASE_SQLITE_COMMAND_H
#define __DAXIA_DATABASE_SQLITE_COMMAND_H
#include "orm.h"
#include "../string.hpp"

namespace daxia
{
	namespace database
	{
		class SqliteCommand : public Command
		{
		public:
			SqliteCommand(const daxia::string& host, const daxia::string& db, const daxia::string& user, const daxia::string& psw);
			~SqliteCommand();
		public:
			virtual std::shared_ptr<Recordset> Excute(const daxia::string& sql) override;
		};
	}
}
#endif	// !__DAXIA_DATABASE_SQLITE_COMMAND_H
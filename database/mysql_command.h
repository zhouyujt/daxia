/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file mysql_command.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* mysql command �ӿ�ʵ��
*
*/
#ifndef __DAXIA_DATABASE_MYSQL_COMMAND_H
#define __DAXIA_DATABASE_MYSQL_COMMAND_H
#include "orm.h"
#include "../string.hpp"
#include "driver/mysql/include/mysql.h"

namespace daxia
{
	namespace database
	{
		class MysqlCommand : public Command
		{
		public:
			MysqlCommand(const daxia::string& host, const daxia::string& db, const daxia::string& user, const daxia::string& psw);
			~MysqlCommand();
		public:
			virtual std::shared_ptr<Recordset> Excute(const daxia::string& sql) override;
		private:
		};
	}
}
#endif	// !__DAXIA_DATABASE_MYSQL_COMMAND_H
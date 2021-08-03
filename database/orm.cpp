#include "orm.h"
#include "driver/mysql_driver.h"
#include "driver/sqlite_driver.h"
#include "driver/sqlserver_driver.h"

#define ORM "orm"
#define IDENTITY "identity"

namespace daxia
{
	namespace database
	{
		Orm::Orm(Driver driver, const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw)
		{
			using namespace daxia::database::driver;

			switch (driver)
			{
			case daxia::database::Orm::mysql:
				command_ = std::shared_ptr<MySQLDriver>(new MySQLDriver(host, port, db, user, psw));
				break;
			case daxia::database::Orm::sqlite:
				break;
			case daxia::database::Orm::sqlserver:
				break;
			default:
				break;
			}
		}

		Orm::Orm(Driver driver, const daxia::string& connectString)
		{
		}

		Orm::~Orm()
		{

		}

		daxia::string Orm::insert(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string fieldList;
			daxia::string valueList;
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0));
				if (reflectBase == nullptr) continue;

				// 构造表名
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					tableName = reflectBase->TagAttribute(ORM);
					continue;
				}
				
				// 构造字段列表及值列表
				if (fields == nullptr || fields->HasField(tag))
				{
					// 排除identity字段
					if (fields == nullptr	// 手动指定的identity不排除
						&& reflectBase->TagAttribute(ORM) == IDENTITY)
					{
						continue;
					}

					if (!fieldList.IsEmpty())  fieldList += ',';
					fieldList += tag;

					if (!valueList.IsEmpty()) valueList += ',';
					valueList += tostring(reflectBase);
				}

			}

			// 拼接
			daxia::string sql;
			sql.Format("INSERT INTO %s(%s) VALUES(%s)",
				tableName.GetString(),
				fieldList.GetString(),
				valueList.GetString()
				);

			// 执行
			auto recodset = command_->Excute(sql);
			return command_->GetLastError();
		}

		daxia::string Orm::delette(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* condition)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string conditionList;
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0));
				if (reflectBase == nullptr) continue;

				// 构造表名
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					tableName = reflectBase->TagAttribute(ORM);
					continue;
				}

				// 构造条件语句
				if (condition == nullptr || condition->HasField(tag))
				{
					if (!conditionList.IsEmpty())  conditionList += " AND ";
					conditionList += tag;
					conditionList += '=';
					conditionList += tostring(reflectBase);
				}

			}

			// 拼接
			daxia::string sql;
			sql.Format("DELETE %s WHERE %s",
				tableName.GetString(),
				conditionList.IsEmpty() ? makeConditionByIdentityField(layout, baseaddr).GetString() : conditionList.GetString()
				);

			// 执行
			auto recodset = command_->Excute(sql);
			return command_->GetLastError();
		}

		std::shared_ptr<Orm::Recordset> Orm::query(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields, const char* suffix, const char* prefix)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string fieldList;
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0));
				if (reflectBase == nullptr) continue;

				// 构造表名
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					tableName = reflectBase->TagAttribute(ORM);
					continue;
				}

				// 构造字段列表
				if (fields == nullptr || fields->HasField(tag))
				{
					if (!fieldList.IsEmpty())  fieldList += ',';
					fieldList += tag;
				}
			}

			// 拼接
			daxia::string sql;
			sql.Format("SELECT %s %s FROM %s WHERE %s",
				prefix == nullptr ? "" : prefix,
				fieldList.GetString(),
				tableName.GetString(),
				suffix == nullptr ? "1=1" : suffix
				);

			// 执行
			return command_->Excute(sql);
		}

		daxia::string Orm::update(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields, const FieldFilter* condition)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string valueList;
			daxia::string conditionList;
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0));
				if (reflectBase == nullptr) continue;

				// 构造表名
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					tableName = reflectBase->TagAttribute(ORM);
					continue;
				}

				// 构造赋值语句
				if (fields == nullptr || fields->HasField(tag))
				{
					if (!valueList.IsEmpty())  valueList += ','; 
					valueList += tag;
					valueList += "=";
					valueList += tostring(reflectBase);
				}

				// 构造条件语句
				if (condition && condition->HasField(tag))
				{
					if (!conditionList.IsEmpty()) conditionList += " AND ";
					conditionList += tag;
					conditionList += "=";
					conditionList += tostring(reflectBase);
				}
			}

			// 拼接
			daxia::string sql;
			sql.Format("UPDATE %s SET %s WHERE %s", 
				tableName.GetString(),
				valueList.GetString(), 
				conditionList.IsEmpty() ? makeConditionByIdentityField(layout,baseaddr).GetString() : conditionList.GetString());

			// 执行
			auto recodset = command_->Excute(sql);
			return command_->GetLastError();
		}

		const daxia::reflect::Reflect_base* Orm::cast(const void* baseaddr, unsigned long offset)
		{
			using namespace daxia::reflect;

			const Reflect_base* reflectBase = nullptr;
			try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(reinterpret_cast<const char*>(baseaddr)+offset)); }
			catch (const std::exception&){}
			
			return reflectBase;
		}

		std::shared_ptr<Orm::Recordset> Orm::Excute(const daxia::string& sql)
		{
			return command_->Excute(sql);
		}

		daxia::string Orm::tostring(const daxia::reflect::Reflect_base* reflectBase)
		{
			using namespace daxia::database::driver;

			daxia::string str;

			const auto& typeinfo = reflectBase->Type();
			if (typeinfo == typeid(db_text))
			{
				str += '\'';
				str += reinterpret_cast<const daxia::string*>(reflectBase->ValueAddr())->GetString();
				str += '\'';
			}
			else if (typeinfo == typeid(db_datetime))
			{
				str += '\'';
				str += reinterpret_cast<const daxia::system::DateTime*>(reflectBase->ValueAddr())->ToString();
				str += '\'';
			}
			else
			{
				if (typeinfo == typeid(db_tinyint)){ str += daxia::string::ToString(*reinterpret_cast<const char*>(reflectBase->ValueAddr())); }
				else if (typeinfo == typeid(db_int)){ str += daxia::string::ToString(*reinterpret_cast<const int*>(reflectBase->ValueAddr())); }
				else if (typeinfo == typeid(db_bigint)){ str += daxia::string::ToString(*reinterpret_cast<const long long*>(reflectBase->ValueAddr())); }
				else if (typeinfo == typeid(db_float)){ str += daxia::string::ToString(*reinterpret_cast<const float*>(reflectBase->ValueAddr())); }
				else if (typeinfo == typeid(db_double)){ str += daxia::string::ToString(*reinterpret_cast<const double*>(reflectBase->ValueAddr())); }
				else if (typeinfo == typeid(db_text)){ str += daxia::string::ToString(*reinterpret_cast<const int*>(reflectBase->ValueAddr())); }
				else if (typeinfo == typeid(db_blob)){ str += daxia::string::ToString(*reinterpret_cast<const unsigned int*>(reflectBase->ValueAddr())); }
				else if (typeinfo == typeid(db_datetime)){ str += daxia::string::ToString(*reinterpret_cast<const long*>(reflectBase->ValueAddr())); }
			}

			return str;
		}

		daxia::string Orm::makeConditionByIdentityField(const boost::property_tree::ptree& layout, const void* baseaddr)
		{
			using namespace daxia::reflect;

			daxia::string condition;
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				unsigned long offset = iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0);

				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(reinterpret_cast<const char*>(baseaddr)+offset)); }
				catch (const std::exception&){}
				if (reflectBase == nullptr) continue;

				if (reflectBase->TagAttribute(ORM) == IDENTITY)
				{
					condition = reflectBase->Tag(ORM);
					condition += "=";
					condition += tostring(reflectBase);
				}
			}

			return condition;
		}

	}
}

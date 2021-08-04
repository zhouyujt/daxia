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
				command_ = std::shared_ptr<SqliteDriver>(new SqliteDriver(db));
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

				// 强制排除未初始化字段
				if (!reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
				{
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

				// 强制排除未初始化字段
				if (!reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
				{
					continue;
				}

				// 构造条件语句
				if (condition == nullptr || condition->HasField(tag))
				{
					// 默认只用identity字段做删除条件
					if (condition == nullptr
						&& reflectBase->TagAttribute(ORM) != IDENTITY)
					{
						continue;
					}

					if (!conditionList.IsEmpty())  conditionList += " AND ";
					conditionList += tag;
					conditionList += '=';
					conditionList += tostring(reflectBase);
				}

			}

			// 拼接
			daxia::string sql;
			sql.Format("DELETE FROM %s WHERE %s",
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

				// 强制排除未初始化字段
				if (!reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
				{
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

		daxia::string Orm::create(const boost::property_tree::ptree& layout, const void* baseaddr)
		{
			throw "尚未实现";
		}

		daxia::string Orm::drop(const boost::property_tree::ptree& layout, const void* baseaddr)
		{
			throw "尚未实现";
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
				str += static_cast<daxia::string>(*reinterpret_cast<const db_text*>(reflectBase->ValueAddr()));
				str += '\'';
			}
			else if (typeinfo == typeid(db_datetime))
			{
				str += '\'';
				str += static_cast<daxia::system::DateTime>(*reinterpret_cast<const db_datetime*>(reflectBase->ValueAddr())).ToString();
				str += '\'';
			}
			else
			{
				if (typeinfo == typeid(db_tinyint)){ str += daxia::string::ToString(static_cast<char>(*reinterpret_cast<const db_tinyint*>(reflectBase->ValueAddr()))); }
				else if (typeinfo == typeid(db_int)){ str += daxia::string::ToString(static_cast<int>(*reinterpret_cast<const db_int*>(reflectBase->ValueAddr()))); }
				else if (typeinfo == typeid(db_bigint)){ str += daxia::string::ToString(static_cast<long long>(*reinterpret_cast<const db_bigint*>(reflectBase->ValueAddr()))); }
				else if (typeinfo == typeid(db_float)){ str += daxia::string::ToString(static_cast<float>(*reinterpret_cast<const db_float*>(reflectBase->ValueAddr()))); }
				else if (typeinfo == typeid(db_double)){ str += daxia::string::ToString(static_cast<double>(*reinterpret_cast<const db_double*>(reflectBase->ValueAddr()))); }
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
					if (reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
					{
						condition = reflectBase->Tag(ORM);
						condition += "=";
						condition += tostring(reflectBase);
					}
				}
			}

			return condition;
		}

		void Orm::record2obj(std::shared_ptr<Recordset> recordset, const boost::property_tree::ptree& layout, void* obj, const FieldFilter* fields)
		{
			using namespace daxia::reflect;
			using namespace daxia::database::driver;

			for (boost::property_tree::ptree::const_iterator iter = layout.begin(); iter != layout.end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(&obj, iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0));
				if (reflectBase == nullptr) continue;

				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG)) continue;

				if (fields == nullptr || fields->HasField(tag))
				{
					void* p = const_cast<void*>(reflectBase->ValueAddr());

					const auto& typeinfo = reflectBase->Type();
					if (typeinfo == typeid(db_tinyint)) *reinterpret_cast<db_tinyint*>(p) = recordset->Get<db_tinyint>(tag.GetString());
					else if (typeinfo == typeid(db_int))  *reinterpret_cast<db_int*>(p) = recordset->Get<db_int>(tag.GetString());
					else if (typeinfo == typeid(db_bigint)) *reinterpret_cast<db_bigint*>(p) = recordset->Get<db_bigint>(tag.GetString());
					else if (typeinfo == typeid(db_float)) *reinterpret_cast<db_float*>(p) = recordset->Get<db_float>(tag.GetString());
					else if (typeinfo == typeid(db_double)) *reinterpret_cast<db_double*>(p) = recordset->Get<db_double>(tag.GetString());
					else if (typeinfo == typeid(db_text)) *reinterpret_cast<db_text*>(p) = recordset->Get<db_text>(tag.GetString());
					else if (typeinfo == typeid(db_blob)) *reinterpret_cast<db_blob*>(p) = recordset->Get<db_blob>(tag.GetString());
					else if (typeinfo == typeid(db_datetime)) *reinterpret_cast<db_datetime*>(p) = recordset->Get<db_datetime>(tag.GetString());
				}
			}
		}

	}
}

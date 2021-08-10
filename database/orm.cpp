#include "orm.h"
#include "driver/mysql_driver.h"
#include "driver/sqlite_driver.h"
#include "driver/sqlserver_driver.h"
#include "../encode/hex.h"

#define ORM "orm"
#define IDENTITY "identity"
#define PRIMARY_KEY "primary_key"
#define UNIQUE_KEY "unique_key"
#define KEY "key"
#define DEFAULT "default"
#define NOT_NULL "not_null"
#define COMMENT "comment"

namespace daxia
{
	namespace database
	{
		Orm::Orm(Driver driver, const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw)
			: driverType_(driver)
			, scopeIdentity_(0)
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
			: driverType_(driver)
			, scopeIdentity_(0)
		{
		}

		Orm::~Orm()
		{

		}

		daxia::string Orm::insert(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string fieldList;
			daxia::string valueList;
			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->offset);
				if (reflectBase == nullptr) continue;

				// �������
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					auto attribute = reflectBase->TagAttribute(ORM);
					if (!attribute.empty())
					{
						tableName = reflectBase->TagAttribute(ORM).begin()->first;
					}
					continue;
				}

				// ǿ���ų�δ��ʼ���ֶ�
				if (!reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
				{
					continue;
				}
				
				// �����ֶ��б�ֵ�б�
				if (fields == nullptr || fields->HasField(tag))
				{
					// �ų�����
					auto attribute = reflectBase->TagAttribute(ORM);
					if (fields == nullptr	// �ֶ�ָ�����������ų�
						&& attribute.find(PRIMARY_KEY) != attribute.end())
					{
						continue;
					}

					if (!fieldList.IsEmpty())  fieldList += ',';
					fieldList += tag;

					if (!valueList.IsEmpty()) valueList += ',';
					valueList += reflectBase->ToString(ORM);
				}

			}

			// ƴ��
			daxia::string sql;
			sql.Format("INSERT INTO %s(%s) VALUES(%s)",
				tableName.GetString(),
				fieldList.GetString(),
				valueList.GetString()
				);

			// ִ��
			auto recodset = command_->Excute(sql);
			scopeIdentity_ = recodset->ScopeIdentity();
			return command_->GetLastError();
		}

		daxia::string Orm::delette(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* condition)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string conditionList;
			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->offset);
				if (reflectBase == nullptr) continue;

				// �������
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					auto attribute = reflectBase->TagAttribute(ORM);
					if (!attribute.empty())
					{
						tableName = reflectBase->TagAttribute(ORM).begin()->first;
					}
					continue;
				}

				// ǿ���ų�δ��ʼ���ֶ�
				if (!reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
				{
					continue;
				}

				// �����������
				if (condition == nullptr || condition->HasField(tag))
				{
					// Ĭ��ֻ��������ɾ������
					auto attribute = reflectBase->TagAttribute(ORM);
					if (condition == nullptr
						&& attribute.find(PRIMARY_KEY) == attribute.end())
					{
						continue;
					}

					if (!conditionList.IsEmpty())  conditionList += " AND ";
					conditionList += tag;
					conditionList += '=';
					conditionList += reflectBase->ToString(ORM);
				}

			}

			// ƴ��
			daxia::string sql;
			sql.Format("DELETE FROM %s WHERE %s",
				tableName.GetString(),
				conditionList.IsEmpty() ? makeConditionByPrimaryKey(layout, baseaddr).GetString() : conditionList.GetString()
				);

			// ִ��
			auto recodset = command_->Excute(sql);
			return command_->GetLastError();
		}

		std::shared_ptr<Orm::Recordset> Orm::query(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields, const char* suffix, const char* prefix)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string fieldList;
			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->offset);
				if (reflectBase == nullptr) continue;

				// �������
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					auto attribute = reflectBase->TagAttribute(ORM);
					if (!attribute.empty())
					{
						tableName = reflectBase->TagAttribute(ORM).begin()->first;
					}
					continue;
				}

				// �����ֶ��б�
				if (fields == nullptr || fields->HasField(tag))
				{
					if (!fieldList.IsEmpty())  fieldList += ',';
					fieldList += tag;
				}
			}

			// ƴ��
			daxia::string sql;
			sql.Format("SELECT %s %s FROM %s WHERE %s",
				prefix == nullptr ? "" : prefix,
				fieldList.GetString(),
				tableName.GetString(),
				suffix == nullptr ? "1=1" : suffix
				);

			// ִ��
			return command_->Excute(sql);
		}

		daxia::string Orm::update(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields, const FieldFilter* condition)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string valueList;
			daxia::string conditionList;
			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->offset);
				if (reflectBase == nullptr) continue;

				// �������
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					auto attribute = reflectBase->TagAttribute(ORM);
					if (!attribute.empty())
					{
						tableName = reflectBase->TagAttribute(ORM).begin()->first;
					}
					continue;
				}

				// ǿ���ų�δ��ʼ���ֶ�
				if (!reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
				{
					continue;
				}

				// ���츳ֵ���
				if (fields == nullptr || fields->HasField(tag))
				{
					if (!valueList.IsEmpty())  valueList += ','; 
					valueList += tag;
					valueList += "=";
					valueList += reflectBase->ToString(ORM);
				}

				// �����������
				if (condition && condition->HasField(tag))
				{
					if (!conditionList.IsEmpty()) conditionList += " AND ";
					conditionList += tag;
					conditionList += "=";
					conditionList += reflectBase->ToString(ORM);
				}
			}

			// ƴ��
			daxia::string sql;
			sql.Format("UPDATE %s SET %s WHERE %s", 
				tableName.GetString(),
				valueList.GetString(), 
				conditionList.IsEmpty() ? makeConditionByPrimaryKey(layout,baseaddr).GetString() : conditionList.GetString());

			// ִ��
			auto recodset = command_->Excute(sql);
			return command_->GetLastError();
		}

		daxia::string Orm::create(const daxia::reflect::Layout& layout, const void* baseaddr)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			daxia::string fieldList;
			daxia::string valueList;
			std::vector<daxia::string> createIndex;
			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->offset);
				if (reflectBase == nullptr) continue;

				// �������
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				auto attribute = reflectBase->TagAttribute(ORM);
				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					if (!attribute.empty())
					{
						tableName = reflectBase->TagAttribute(ORM).begin()->first;
					}
					continue;
				}

				// �����ֶ��б�
				if (!fieldList.IsEmpty())  fieldList += ',';
				fieldList += tag;
				fieldList += ' ';
				fieldList += command_->TypeName(reflectBase->Type());
				if (driverType_ == sqlserver)
				{

				}
				else if (driverType_ == mysql)
				{
					if (attribute.find(PRIMARY_KEY) != attribute.end())
					{
						fieldList += " PRIMARY KEY";
					}

					if (attribute.find(IDENTITY) != attribute.end())
					{
						fieldList += " AUTO_INCREMENT";
					}

					if (attribute.find(UNIQUE_KEY) != attribute.end())
					{
						fieldList += " UNIQUE KEY";
					}

					if (attribute.find(KEY) != attribute.end())
					{
						fieldList += " KEY";
					}

					if (attribute.find(DEFAULT) != attribute.end())
					{
						fieldList += " DEFAULT ";
						fieldList += attribute.find(DEFAULT)->second;
					}

					if (attribute.find(NOT_NULL) != attribute.end())
					{
						fieldList += " NOT NULL";
					}

					if (attribute.find(COMMENT) != attribute.end())
					{
						fieldList += " COMMENT ";
						fieldList += "'";
						fieldList += attribute.find(COMMENT)->second;
						fieldList += "'";
					}
				}
				else if (driverType_ == sqlite)
				{
					if (attribute.find(PRIMARY_KEY) != attribute.end())
					{
						fieldList += " PRIMARY KEY";
					}

					if (attribute.find(IDENTITY) != attribute.end())
					{
						fieldList += " AUTOINCREMENT";
					}

					if (attribute.find(UNIQUE_KEY) != attribute.end())
					{
						daxia::string sql;
						sql.Format("CREATE UNIQUE INDEX index_%s_%s ON %s(%s)", tableName.GetString(), tag.GetString(), tableName.GetString(), tag.GetString());
						createIndex.push_back(sql);
					}

					if (attribute.find(KEY) != attribute.end())
					{
						daxia::string sql;
						sql.Format("CREATE INDEX index_%s_%s ON %s(%s)", tableName.GetString(), tag.GetString(), tableName.GetString(), tag.GetString());
						createIndex.push_back(sql);
					}

					if (attribute.find(DEFAULT) != attribute.end())
					{
						fieldList += " DEFAULT ";
						fieldList += attribute.find(DEFAULT)->second;
					}

					if (attribute.find(NOT_NULL) != attribute.end())
					{
						fieldList += " NOT NULL";
					}

					//if (attribute.find(COMMENT) != attribute.end())
					//{
					//	fieldList += " /*";
					//	fieldList += attribute.find(COMMENT)->second;
					//	fieldList += " */";
					//}
				}
			}

			// ƴ��
			daxia::string sql;
			sql.Format("CREATE TABLE %s(%s)",
				tableName.GetString(),
				fieldList.GetString()
				);

			// ִ��
			command_->Excute(sql);

			// ��������
			if (command_->GetLastError().IsEmpty())
			{
				for (const daxia::string& sql : createIndex)
				{
					command_->Excute(sql);
					if (!command_->GetLastError().IsEmpty())
					{
						break;
					}
				}
			}

			return command_->GetLastError();
		}

		daxia::string Orm::drop(const daxia::reflect::Layout& layout, const void* baseaddr)
		{
			using namespace daxia::reflect;

			daxia::string tableName;
			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				const Reflect_base* reflectBase = cast(baseaddr, iter->offset);
				if (reflectBase == nullptr) continue;

				// �������
				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				auto attribute = reflectBase->TagAttribute(ORM);
				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG))
				{
					if (!attribute.empty())
					{
						tableName = reflectBase->TagAttribute(ORM).begin()->first;
					}
					break;
				}
			}

			// ƴ��
			daxia::string sql;
			sql.Format("DROP TABLE %s",
				tableName.GetString()
				);

			// ִ��
			command_->Excute(sql);

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

		long long Orm::ScopeIdentity()
		{
			return scopeIdentity_;
		}

		std::shared_ptr<Orm::Recordset> Orm::Excute(const daxia::string& sql)
		{
			return command_->Excute(sql);
		}

		daxia::string Orm::makeConditionByPrimaryKey(const daxia::reflect::Layout& layout, const void* baseaddr)
		{
			using namespace daxia::reflect;

			daxia::string condition;
			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(reinterpret_cast<const char*>(baseaddr)+iter->offset)); }
				catch (const std::exception&){}
				if (reflectBase == nullptr) continue;

				auto attribute = reflectBase->TagAttribute(ORM);
				if (attribute.find(PRIMARY_KEY) != attribute.end())
				{
					if (reinterpret_cast<const daxia::database::driver::BasicDataType*>(reflectBase->ValueAddr())->IsInit())
					{
						condition = reflectBase->Tag(ORM);
						condition += "=";
						condition += reflectBase->ToString(ORM);
					}
				}
			}

			return condition;
		}

		void Orm::record2obj(std::shared_ptr<Recordset> recordset, const daxia::reflect::Layout& layout, void* baseaddr, const FieldFilter* fields)
		{
			using namespace daxia::reflect;
			using namespace daxia::database::driver;

			for (std::vector<daxia::reflect::Field>::const_iterator iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				Reflect_base* reflectBase = const_cast<Reflect_base*>(cast(baseaddr, iter->offset));
				if (reflectBase == nullptr) continue;

				daxia::string tag = reflectBase->Tag(ORM);
				if (tag.IsEmpty()) continue;

				if (tag == DATABASE_ORM_STRING(DATABASE_ORM_TABLE_TAG)) continue;

				if (fields == nullptr || fields->HasField(tag))
				{
					reflectBase->FromString(ORM,recordset->GetRawData(tag.GetString()));
				}
			}
		}
	}
}

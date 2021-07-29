#include "orm.h"

#define ORM "orm"

namespace daxia
{
	namespace database
	{
		daxia::string Orm::Insert(const daxia::reflect::Reflect_base* table)
		{
			using namespace daxia::reflect;

			daxia::string sql;
			sql.Format("insert into %s(", table->Tag(ORM));

			daxia::string fieldList;
			auto layout = table->Layout();
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				unsigned long offset = iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0);

				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(reinterpret_cast<const char*>(table->ValueAddr()) + offset)); }
				catch (const std::exception&){}
				if (reflectBase == nullptr) continue;

				std::string tag = reflectBase->Tag(ORM);
				if (!tag.empty())
				{
					char ch = fieldList.IsEmpty() ? '(' : ',';
					fieldList += ch;
					fieldList += tag;
				}
			}
			fieldList += ") values";

			daxia::string valueList;
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				unsigned long offset = iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0);

				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(reinterpret_cast<const char*>(table->ValueAddr()) + offset)); }
				catch (const std::exception&){}
				if (reflectBase == nullptr) continue;

				char ch = valueList.IsEmpty() ? '(' : ',';
				valueList += ch;

				valueList += tostring(reflectBase);
			}
			valueList += ')';

			sql += fieldList + valueList;

			auto recodset = command_->Excute(sql);
			return recodset->GetLastError();
		}

		daxia::string Orm::Delete(const daxia::reflect::Reflect_base* table)
		{
			daxia::string sql;
			sql.Format("delete %s where ", table->Tag(ORM));

			// 拼接条件
			auto layout = table->Layout();
			sql += makeConditionByIdentityField(layout);

			auto recodset = command_->Excute(sql);
			return recodset->GetLastError();
		}

		daxia::string Orm::Query(daxia::reflect::Reflect_base* table)
		{

		}

		daxia::string Orm::Update(daxia::reflect::Reflect_base* table)
		{
			using namespace daxia::reflect;

			daxia::string sql;
			sql.Format("update %s set ", table->Tag(ORM));

			daxia::string valueList;
			auto layout = table->Layout();
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				unsigned long offset = iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0);

				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(reinterpret_cast<const char*>(table->ValueAddr()) + offset)); }
				catch (const std::exception&){}
				if (reflectBase == nullptr) continue;

				std::string tag = reflectBase->Tag(ORM);
				if (!tag.empty())
				{
					if (!valueList.IsEmpty())
					{
						valueList += ',';
					}
					
					valueList += tag;
					valueList += "=";
					valueList += tostring(reflectBase);
				}
			}

			// 拼接条件
			sql += " where ";
			auto layout = table->Layout();
			sql += makeConditionByIdentityField(layout);

			auto recodset = command_->Excute(sql);
			return recodset->GetLastError();
		}

		std::shared_ptr<daxia::database::Recordset> Orm::Excute(const daxia::string& sql)
		{
			return command_->Excute(sql);
		}

		daxia::string Orm::tostring(const daxia::reflect::Reflect_base* reflectBase)
		{
			daxia::string str;

			if (reflectBase->Type() == typeid(std::string))
			{
				str += '\'';
				str += reinterpret_cast<const std::string*>(reflectBase->ValueAddr())->c_str();
				str += '\'';
			}
			else
			{
				if (reflectBase->Type() == typeid(bool)){ str += daxia::string::ToString(*reinterpret_cast<const bool*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(char)){ str += daxia::string::ToString(*reinterpret_cast<const char*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(unsigned char)){ str += daxia::string::ToString(*reinterpret_cast<const unsigned char*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(short)){ str += daxia::string::ToString(*reinterpret_cast<const short*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(unsigned short)){ str += daxia::string::ToString(*reinterpret_cast<const unsigned short*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(int)){ str += daxia::string::ToString(*reinterpret_cast<const int*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(unsigned int)){ str += daxia::string::ToString(*reinterpret_cast<const unsigned int*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(long)){ str += daxia::string::ToString(*reinterpret_cast<const long*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(unsigned long)){ str += daxia::string::ToString(*reinterpret_cast<const unsigned long*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(long long)){ str += daxia::string::ToString(*reinterpret_cast<const long long*>(reflectBase->ValueAddr())); }
				else if (reflectBase->Type() == typeid(unsigned long long)){ str += daxia::string::ToString(*reinterpret_cast<const unsigned long long*>(reflectBase->ValueAddr())); }
			}

			return str;
		}

		daxia::string Orm::makeConditionByIdentityField(const boost::property_tree::ptree& layout)
		{
			using namespace daxia::reflect;

			daxia::string condition;
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				unsigned long offset = iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0);

				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(reinterpret_cast<const char*>(table->ValueAddr()) + offset)); }
				catch (const std::exception&){}
				if (reflectBase == nullptr) continue;

				if (reflectBase->TagAttribute(ORM) == "identify")
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

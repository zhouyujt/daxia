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

				//if ()
				//{
				//}
				//
				//	
				//valueList += tag;
			}
			valueList += ')';

			sql += fieldList + valueList;

			auto recodset = command_->Excute(sql);
			daxia::string err = recodset->GetLastError();

			if (!err.IsEmpty())
			{
				//recodset->
			}

			return err;
		}

	}
}

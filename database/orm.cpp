#include "orm.h"

#define ORM "orm"

namespace daxia
{
	namespace database
	{

		bool Orm::Insert(const char* format, ...)
		{
			daxia::string sql;
			va_list vl;
			va_start(vl, format);
			sql.FormatV(format, vl);
			va_end(vl);

			size_t pos;
			daxia::string test = sql.Tokenize(" ", pos);
			if (test.CompareNoCase("insert") != 0) return false;

			Insert(sql);
		}

		bool Orm::Insert(const Reflect_base* table)
		{
			daxia::string sql;
			sql.Format("insert into %s(", table.Tag(ORM));

			daxia::string temp;
			auto layout = table.Layout();
			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				Reflect_base* field = 
			}
		}

	}
}

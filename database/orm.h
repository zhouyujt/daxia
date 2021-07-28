#include "../string.hpp"
#include "../reflect/reflect_base.h"

namespace daxia
{
	namespace database
	{
		class Orm_base
		{
		protected:
			Orm_base() {}
			virtual ~Orm_base{}
		};

		class Orm
		{
		public:
			enum Driver
			{
				mysql,
				sqlite,
				sqlserver
			};
		public:
			Orm(Driver driver, const daxia::string& connectString);
			virtual ~Orm();
		public:
			virtual bool Insert(const daxia::string& sql) = 0;
			virtual bool Delete(const daxia::string& sql) = 0;
			virtual bool Query(const daxia::string& sql) = 0;
			virtual bool Update(const daxia::string& sql) = 0;
			virtual bool Excute(const daxia::string& sql) = 0;
			bool Insert(const char* format, ...);
			bool Insert(const Reflect_base* table);
			bool Delete(const char* format, ...);
			bool Delete(const Reflect_base* table);
			bool Query(const char* format, ...);
			bool Query(Reflect_base* table);
			bool Update(const char* format, ...);
			bool Update(Reflect_base* table);
		};
	}
}
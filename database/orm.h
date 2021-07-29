#include <memory>
#include "../string.hpp"
#include "../reflect/reflect.hpp"

namespace daxia
{
	namespace database
	{
		class Recordset
		{
		public:
			Recordset();
			~Recordset();
		public:
			virtual bool Eof() = 0;
			virtual bool Bof() = 0;
			virtual void Next() = 0;
			virtual daxia::string GetLastError() = 0;
			virtual long long  ScopeIdentity() = 0;
			template<class ValueType>
			ValueType Get(const char* field)
			{
				ValueType v;
				GetField(field,v);
				return v;
			}
		protected:
			virtual void GetField(const char* field, int& v) = 0;
			virtual void GetField(const char* field, long long& v) = 0;
			virtual void GetField(const char* field, float& v) = 0;
			virtual void GetField(const char* field, double& v) = 0;
			virtual void GetField(const char* field, daxia::string& v) = 0;
		};

		class Command
		{
		protected:
			Command();
			virtual ~Command();
		public:
			virtual std::shared_ptr<Recordset> Excute(const daxia::string& sql) = 0;
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
			daxia::string Insert(const daxia::reflect::Reflect_base* table);
			daxia::string Delete(const daxia::reflect::Reflect_base* table);
			daxia::string Query(daxia::reflect::Reflect_base* table);
			daxia::string Update(daxia::reflect::Reflect_base* table);
		private:
			std::shared_ptr<Command> command_;
		};
	}
}
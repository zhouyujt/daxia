/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file datetime.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ���ݿ���ɾ���
* ֧�ֿ��ٷ���ؽ�C++����洢�����ݿ��¼
* ֧�ֿ��ٷ���ؽ����ݿ��¼��ȡ��C++����
*
*/
#ifndef __DAXIA_DATABASE_ORM_H
#define __DAXIA_DATABASE_ORM_H
#include <memory>
#include <set>
#include "../string.hpp"
#include "../reflect/reflect.hpp"
#include "data_type.h"

#define DATABASE_ORM_TABLE_FIELD				__tableName__
#define DATABASE_ORM_TABLE_TAG					"tableName"
#define DATABASE_ORM_MAKE_TABLE_TAG(name)		"orm:" ## DATABASE_ORM_TABLE_TAG ## "(" ###name##")"
#define DATABASE_ORM_DECLARE_TABLE_FIELD(name)	daxia::string DATABASE_ORM_TABLE_FIELD = DATABASE_ORM_TABLE_TAG(name);

namespace daxia
{
	namespace database
	{
		// ���ݼ��ӿ�
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
			virtual void GetField(const char* field, db_tinyint& v) = 0;
			virtual void GetField(const char* field, db_int& v) = 0;
			virtual void GetField(const char* field, db_bigint& v) = 0;
			virtual void GetField(const char* field, db_float& v) = 0;
			virtual void GetField(const char* field, db_double& v) = 0;
			virtual void GetField(const char* field, db_text& v) = 0;
			virtual void GetField(const char* field, db_blob& v) = 0;
			virtual void GetField(const char* field, db_datetime& v) = 0;
		};

		// ����ӿ�
		class Command
		{
		protected:
			Command();
			virtual ~Command();
		public:
			virtual std::shared_ptr<Recordset> Excute(const daxia::string& sql) = 0;
		};

		// �ֶι�����
		class FieldFilter
		{
		public:
			FieldFilter();
			~FieldFilter();
		public:
			template<class ValueType>
			FieldFilter& operator()(const ValueType& v)
			{
				fields_.push_back(v.Tag());
				return *this;
			}

			const std::set<daxia::string>& GetFields() const
			{
				return fields_;
			}

			bool HasField(const daxia::string field) const
			{
				return fields_.find(field) != fields_.end();
			}
		private:
			std::set<daxia::string> fields_;
		};

		// C++���� <==> ���ݿ��¼
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
			Orm(Driver driver, const daxia::string& host, const daxia::string& db, const daxia::string& user, const daxia::string& psw);
			Orm(Driver driver, const daxia::string& connectString);
			virtual ~Orm();
		public:
			// ����
			// fields: ��Ҫ������ֶΡ�nullptr����������ֶΡ�
			template<class ValueType>
			daxia::string Insert(const ValueType& obj, const FieldFilter* fields = nullptr)
			{
				auto layout = Reflect<ValueType>().Layout();
				return insert(layout, &obj, fields);
			}

			// ɾ��
			// condition: ɾ������������¼��ָ���ֶ�ֵ��ͬ��ɾ����nullptr����ݾ���identify���Ե��ֶ�ɾ����
			template<class ValueType>
			daxia::string Delete(const ValueType& obj, const FieldFilter* condition = nullptr)
			{
				auto layout = Reflect<ValueType>().Layout();
				return delette(layout, &obj, condition);
			}

			// ��ѯ
			// fields: ��Ҫ��ѯ���ֶΡ�nullptr���ѯ�����ֶΡ�
			// suffix: ��׺�ַ�����������ѯ������������ȡ����� "id < 10 and name is not null order by name" 
			// prefix: ǰ׺�ַ��������� "top(10)"��"distinct"
			template<class ValueType>
			daxia::string Query(std::vector<ValueType>& objs,
				const FieldFilter* fields = nullptr,
				const char* suffix = nullptr,
				const char* prefix = nullptr
				)
			{
				ValueType helper;
				auto layout = Reflect<ValueType>().Layout();
				auto recordset = query(layout, &helper, fields, suffix, prefix);

				// ��ȡ���
				while (!recordset->Eof())
				{
					ValueType obj;
					for (auto iter = layout.begin(); iter != layout.end(); ++iter)
					{
						const Reflect_base* reflectBase = cast(&obj, iter->second.get<unsigned long>(REFLECT_LAYOUT_FIELD_OFFSET, 0));
						if (reflectBase == nullptr) continue;

						daxia::string tag = reflectBase->Tag("orm");
						if (tag.IsEmpty()) continue;

						if (tag == DATABASE_ORM_TABLE_TAG) continue;

						if (fields == nullptr || fields->HasField(tag))
						{
							ValueType* p = static_cast<ValueType*>(const_cast<void*>(reflectBase->ValueAddr()));

							const auto& typeinfo = reflectBase->Type();
							if (typeinfo == typeid(db_tinyint)) *p = recordset->Get<db_tinyint>(tag.GetString());
							else if (typeinfo == typeid(db_int))  *p = recordset->Get<db_int>(tag.GetString());
							else if (typeinfo == typeid(db_bigint)) *p = recordset->Get<db_bigint>(tag.GetString());
							else if (typeinfo == typeid(db_float)) *p = recordset->Get<db_float>(tag.GetString());
							else if (typeinfo == typeid(db_double)) *p = recordset->Get<db_double>(tag.GetString());
							else if (typeinfo == typeid(db_text)) *p = recordset->Get<db_text>(tag.GetString());
							else if (typeinfo == typeid(db_blob)) *p = recordset->Get<db_blob>(tag.GetString());
							else if (typeinfo == typeid(db_datetime)) *p = recordset->Get<db_datetime>(tag.GetString());
						}
					}

					objs.push_back(obj);

					recordset->Next();
				}
			}

			// ����
			// fields:		ָ����Ҫ���µ��ֶΡ�nullptr����������ֶΡ�
			// condition:	��������������¼��ָ���ֶ�ֵ��ͬ�Ÿ��¡�nullptr����ݾ���identify���Ե��ֶθ��¡�
			template<class ValueType>
			daxia::string Update(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				auto layout = Reflect<ValueType>().Layout();
				return update(layout, &obj, fields);
			}

			// ִ�и����ӵ�����
			std::shared_ptr<Recordset> Excute(const daxia::string& sql);
		private:
			daxia::string insert(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields);
			daxia::string delette(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* condition);
			std::shared_ptr<Recordset> query(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields, const char* suffix, const char* prefix);
			daxia::string update(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields, const FieldFilter* condition);
		private:
			static const daxia::reflect::Reflect_base* cast(const void* baseaddr, unsigned long offset);
			static daxia::string tostring(const daxia::reflect::Reflect_base* reflectBase);
			static daxia::string makeConditionByIdentityField(const boost::property_tree::ptree& layout, const void* baseaddr);
		private:
			std::shared_ptr<Command> command_;
		};
	}
}

#endif	// !__DAXIA_DATABASE_ORM_H
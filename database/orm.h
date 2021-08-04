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
#include "driver/basic_driver.h"
#include "driver/basic_recordset.h"
#include "driver/data_type.h"
#include "../string.hpp"
#include "../reflect/reflect.hpp"

#define DATABASE_ORM_STRING_HELP(s) #s
#define DATABASE_ORM_STRING(s) DATABASE_ORM_STRING_HELP(s)

#define DATABASE_ORM_TABLE_FIELD				__tableName__
#define DATABASE_ORM_TABLE_TAG					tableName
#define DATABASE_ORM_MAKE_TABLE_TAG(name)		orm: ## DATABASE_ORM_TABLE_TAG ## ( ##name ##)
#define DECLARE_ORM_TABLE(name)					daxia::reflect::Reflect<daxia::string> DATABASE_ORM_TABLE_FIELD = DATABASE_ORM_STRING(DATABASE_ORM_MAKE_TABLE_TAG(name));

#define ORM "orm"

namespace daxia
{
	namespace database
	{
		// �ֶι�����
		class FieldFilter
		{
		public:
			template<class ValueType>
			FieldFilter(const ValueType& v)
				: exlude_(false)
			{
				fields_.insert(v.Tag(ORM));
			}

			~FieldFilter(){}
		private:
			FieldFilter() : exlude_(false){}
		public:
			template<class ValueType>
			FieldFilter& operator()(const ValueType& v)
			{
				fields_.insert(v.Tag(ORM));
				return *this;
			}

			// ȡ����������ʾ�ų�ָ�����ֶΣ�Ĭ��Ϊ����ָ�����ֶ�
			FieldFilter operator!()
			{
				FieldFilter filter(*this);
				filter.exlude_ = !exlude_;

				return filter;
			}

			const std::set<daxia::string>& GetFields() const
			{
				return fields_;
			}

			bool HasField(const daxia::string field) const
			{
				bool has = fields_.find(field) != fields_.end();
				return exlude_ ? !has : has;
			}
		protected:
			std::set<daxia::string> fields_;
		private:
			bool exlude_;
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
			typedef daxia::database::driver::BasicRecordset Recordset;
			typedef daxia::database::driver::BasicDriver Command;
		public:
			Orm(Driver driver, const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw);
			Orm(Driver driver, const daxia::string& connectString);
			virtual ~Orm();
		public:
			// ����
			// fields: ��Ҫ������ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			template<class ValueType>
			daxia::string Insert(const ValueType& obj, const FieldFilter* fields = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return insert(layout, &obj, fields);
			}

			// ɾ��
			// condition: ɾ������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ��ɾ����nullptr����ݾ���identify�������ѳ�ʼ�����ֶ�ɾ����
			template<class ValueType>
			daxia::string Delete(const ValueType& obj, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

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
				using namespace daxia::reflect;

				ValueType helper;
				auto layout = Reflect<ValueType>().Layout();
				std::shared_ptr<Recordset> recordset = query(layout, &helper, fields, suffix, prefix);

				ReadRecordset(recordset, objs);

				return command_->GetLastError();
			}

			// ����
			// fields:		ָ����Ҫ���µ��ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			// condition:	��������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ�Ÿ��¡�nullptr����ݾ���identify�������ѳ�ʼ�����ֶθ��¡�
			template<class ValueType>
			daxia::string Update(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return update(layout, &obj, fields, condition);
			}

			// ����
			template<class ValueType>
			daxia::string Create(const ValueType& obj)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return create(layout, &obj);
			}

			// ɾ��
			template<class ValueType>
			daxia::string Drop(const ValueType& obj)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return drop(layout, &obj);
			}

			template<class ValueType>
			void ReadRecordset(std::shared_ptr<Recordset> recordset, std::vector<ValueType>& objs, const FieldFilter* fields = nullptr)
			{
				auto layout = Reflect<ValueType>().Layout();
				while (recordset && !recordset->Eof())
				{
					ValueType obj;
					record2obj(recordset, layout, &obj, fields);
					objs.push_back(obj);

					recordset->Next();
				}
			}

			// ִ�и����ӵ�����
			std::shared_ptr<Recordset> Excute(const daxia::string& sql);
		private:
			daxia::string insert(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields);
			daxia::string delette(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* condition);
			std::shared_ptr<Recordset> query(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields, const char* suffix, const char* prefix);
			daxia::string update(const boost::property_tree::ptree& layout, const void* baseaddr, const FieldFilter* fields, const FieldFilter* condition);
			daxia::string create(const boost::property_tree::ptree& layout, const void* baseaddr);
			daxia::string drop(const boost::property_tree::ptree& layout, const void* baseaddr);
		private:
			static const daxia::reflect::Reflect_base* cast(const void* baseaddr, unsigned long offset);
			static daxia::string tostring(const daxia::reflect::Reflect_base* reflectBase);
			static daxia::string makeConditionByIdentityField(const boost::property_tree::ptree& layout, const void* baseaddr);
			static void record2obj(std::shared_ptr<Recordset> recordset, const boost::property_tree::ptree& layout, void* obj, const FieldFilter* fields = nullptr);
		private:
			std::shared_ptr<Command> command_;
		};
	}
}

#undef ORM

#endif	// !__DAXIA_DATABASE_ORM_H
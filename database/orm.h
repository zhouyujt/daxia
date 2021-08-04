/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file datetime.h
* \author 漓江里的大虾
* \date 七月 2021
*
* 数据库增删查改
* 支持快速方便地将C++对象存储至数据库记录
* 支持快速方便地将数据库记录读取到C++对象
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
		// 字段过滤器
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

			// 取反操作符表示排除指定的字段，默认为包含指定的字段
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

		// C++对象 <==> 数据库记录
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
			// 插入
			// fields: 需要插入的字段(该字段需初始化，未初始化的字段会强制忽略)。nullptr则插入所有已经初始化的字段。
			template<class ValueType>
			daxia::string Insert(const ValueType& obj, const FieldFilter* fields = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return insert(layout, &obj, fields);
			}

			// 删除
			// condition: 删除条件，当记录跟指定字段值(该字段需初始化，未初始化的字段会强制忽略)相同才删除。nullptr则根据具有identify属性且已初始化的字段删除。
			template<class ValueType>
			daxia::string Delete(const ValueType& obj, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return delette(layout, &obj, condition);
			}

			// 查询
			// fields: 需要查询的字段。nullptr则查询所有字段。
			// suffix: 后缀字符串，表明查询条件或是排序等。类似 "id < 10 and name is not null order by name" 
			// prefix: 前缀字符串。类似 "top(10)"、"distinct"
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

			// 更新
			// fields:		指定需要更新的字段(该字段需初始化，未初始化的字段会强制忽略)。nullptr则更新所有已经初始化的字段。
			// condition:	更新条件，当记录跟指定字段值(该字段需初始化，未初始化的字段会强制忽略)相同才更新。nullptr则根据具有identify属性且已初始化的字段更新。
			template<class ValueType>
			daxia::string Update(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return update(layout, &obj, fields, condition);
			}

			// 建表
			template<class ValueType>
			daxia::string Create(const ValueType& obj)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return create(layout, &obj);
			}

			// 删表
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

			// 执行更复杂的命令
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
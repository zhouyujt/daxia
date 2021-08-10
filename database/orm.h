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
* 使用示例:
* struct Account
* {
*		DECLARE_ORM_TABLE(account);
*
*		Reflect<db_int> id = "orm:id(identity primary_key comment=id)";
*		Reflect<db_text> user = "orm:user(unique_key comment=用户名)";
*		Reflect<db_text> password = "orm:psw(not_null comment=密码)";
*		Reflect<db_text> name = "orm:name(key comment=姓名)";
*		Reflect<db_int> age = "orm:age(default=18 comment=年龄)";
* };
*
* Account account;
* Orm orm(...);
* orm.Query(account);
* account.id.Value() = 1;
* orm.Delete(account);
* 
* 每个自定struct必须使用DECLARE_ORM_TABLE宏来定义表名。
* 每个字段的Tag名即为字段名。
* 支持的Tag属性如下:
* identity			自动递增1
* primary_key		主键
* unique_key		唯一索引
* key				不唯一索引
* default			默认值
* not_null			不能为空
* comment			注释
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
#define DATABASE_ORM_MAKE_TABLE_TAG(name)		orm:DATABASE_ORM_TABLE_TAG(name)
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

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return insert(layout, &obj, fields);
			}

			// 删除
			// condition: 删除条件，当记录跟指定字段值(该字段需初始化，未初始化的字段会强制忽略)相同才删除。nullptr则根据具有primary_key属性且已初始化的字段删除。
			template<class ValueType>
			daxia::string Delete(const ValueType& obj, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().Layout();
				return delette(layout, &obj, condition);
			}


			// 查询一条
			// fields: 需要查询的字段。nullptr则查询所有字段。
			// suffix: 后缀字符串，表明查询条件或是排序等。类似 "id < 10 and name is not null order by name" 
			// prefix: 前缀字符串。类似 "top(10)"、"distinct"
			template<class ValueType>
			daxia::string Query(ValueType& obj,
				const FieldFilter* fields = nullptr,
				const char* suffix = nullptr,
				const char* prefix = nullptr
				)
			{
				using namespace daxia::reflect;

				ValueType helper;
				auto layout = Reflect<ValueType>::GetLayoutFast();
				std::shared_ptr<Recordset> recordset = query(layout, &helper, fields, suffix, prefix);

				if (recordset && !recordset->Eof())
				{
					record2obj(recordset, layout, &obj, fields);
				}

				return command_->GetLastError();
			}

			// 查询多条
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
				auto layout = Reflect<ValueType>::GetLayoutFast();
				std::shared_ptr<Recordset> recordset = query(layout, &helper, fields, suffix, prefix);

				ReadRecordset(recordset, objs);

				return command_->GetLastError();
			}

			// 更新
			// fields:		指定需要更新的字段(该字段需初始化，未初始化的字段会强制忽略)。nullptr则更新所有已经初始化的字段。
			// condition:	更新条件，当记录跟指定字段值(该字段需初始化，未初始化的字段会强制忽略)相同才更新。nullptr则根据具有primary_key属性且已初始化的字段更新。
			template<class ValueType>
			daxia::string Update(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return update(layout, &obj, fields, condition);
			}

			// 建表
			template<class ValueType>
			daxia::string Create(const ValueType& obj)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return create(layout, &obj);
			}

			// 删表
			template<class ValueType>
			daxia::string Drop(const ValueType& obj)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return drop(layout, &obj);
			}

			template<class ValueType>
			void ReadRecordset(std::shared_ptr<Recordset> recordset, std::vector<ValueType>& objs, const FieldFilter* fields = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				while (recordset && !recordset->Eof())
				{
					ValueType obj;
					record2obj(recordset, layout, &obj, fields);
					objs.push_back(obj);

					recordset->Next();
				}
			}

			long long  ScopeIdentity();

			// 执行更复杂的命令
			std::shared_ptr<Recordset> Excute(const daxia::string& sql);
		private:
			daxia::string insert(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields);
			daxia::string delette(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* condition);
			std::shared_ptr<Recordset> query(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields, const char* suffix, const char* prefix);
			daxia::string update(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields, const FieldFilter* condition);
			daxia::string create(const daxia::reflect::Layout& layout, const void* baseaddr);
			daxia::string drop(const daxia::reflect::Layout& layout, const void* baseaddr);
		private:
			static const daxia::reflect::Reflect_base* cast(const void* baseaddr, unsigned long offset);
			static daxia::string makeConditionByPrimaryKey(const daxia::reflect::Layout& layout, const void* baseaddr);
			static void record2obj(std::shared_ptr<Recordset> recordset, const daxia::reflect::Layout& layout, void* baseaddr, const FieldFilter* fields = nullptr);
		private:
			std::shared_ptr<Command> command_;
			Driver driverType_;
			long long scopeIdentity_;
		};
	}
}

#undef ORM

#endif	// !__DAXIA_DATABASE_ORM_H
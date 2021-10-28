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
* ʹ��ʾ��:
* struct Account
* {
*		DECLARE_ORM_TABLE(account);
*
*		Reflect<db_int> id{ "orm:id(identity primary_key comment=id)" };
*		Reflect<db_text> user{ "orm:user(unique_key comment=�û���)" };
*		Reflect<db_text> password{ "orm:psw(not_null comment=����)" };
*		Reflect<db_text> name{ "orm:name(key comment=����)" };
*		Reflect<db_int> age{ "orm:age(default=18 comment=����)" };
* };
*
* Account account;
* Orm orm(...);
* orm.Query(account);
* account.id.Value() = 1;
* orm.Delete(account);
* 
* ÿ���Զ�struct����ʹ��DECLARE_ORM_TABLE�������������
* ÿ���ֶε�Tag����Ϊ�ֶ�����
* ֧�ֵ�Tag��������:
* identity			�Զ�����1
* primary_key		����
* unique_key		Ψһ����
* key				��Ψһ����
* default			Ĭ��ֵ
* not_null			����Ϊ��
* comment			ע��
*
* Э��֧��:
* �����ṩ�ķ������������汾
* ͬ�������汾��Э�̰汾�����Ը��ݷ�����������Ǻ��ְ汾����ǰ׺��Co����ͷ�ļ�ΪЭ�̰汾����:CoInsert
* �����Ҫ����Э�̰汾����ȷ���������Ѿ���������Э���У���������δ֪�Ľ��
* ����Э�̵�ʹ�÷�����ο�Э�̿��˵��(daxia::system::CoScheduler��daxia::system::Coroutine��daxia::system::this_coroutine)
*/
#ifndef __DAXIA_DATABASE_ORM_H
#define __DAXIA_DATABASE_ORM_H
#include <memory>
#include <set>
#include "driver/basic_driver.h"
#include "driver/basic_recordset.h"
#include "driver/data_type.hpp"
#include "../string.hpp"
#include "../reflect/reflect.hpp"
#include "../system/threadpool/thread_pool.h"
#include "../system/coroutine.h"

#define DATABASE_ORM_STRING_HELP(s) #s
#define DATABASE_ORM_STRING(s) DATABASE_ORM_STRING_HELP(s)

#define DATABASE_ORM_TABLE_FIELD				__tableName__
#define DATABASE_ORM_TABLE_TAG					tableName
#define DATABASE_ORM_MAKE_TABLE_TAG(name)		orm:DATABASE_ORM_TABLE_TAG(name)
#define DECLARE_ORM_TABLE(name)					daxia::reflect::Reflect<daxia::string> DATABASE_ORM_TABLE_FIELD { DATABASE_ORM_STRING(DATABASE_ORM_MAKE_TABLE_TAG(name)) };

#define ORM "orm"
#define CO_CALL(xx) \
std::packaged_task<daxia::string()> task([&]()\
{\
	return xx;\
});\
tp_.Post(task);\
std::future<daxia::string> err = task.get_future();\
daxia::system::this_coroutine::CoWait(WAIT_FUTURE(err));\
return err.get();

namespace daxia
{
	namespace database
	{
		// �ֶι�����
		class FieldFilter
		{
		public:
			template<typename ValueType>
			FieldFilter(const ValueType& v)
				: exlude_(false)
			{
				fields_.insert(v.Tag(ORM));
			}

			~FieldFilter(){}
		private:
			FieldFilter() : exlude_(false){}
		public:
			template<typename ValueType>
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

			// ���ñ����κη���ǰ������ҽ�����һ�γ�ʼ��
			static void Init();
		public:
			// ����
			// fields: ��Ҫ������ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			template<typename ValueType>
			daxia::string Insert(const ValueType& obj, const FieldFilter* fields = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return insert(layout, &obj, fields);
			}

			// ���루Э�̰棩
			// fields: ��Ҫ������ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			template<typename ValueType>
			daxia::string CoInsert(const ValueType& obj, const FieldFilter* fields = nullptr)
			{
				CO_CALL(Insert(obj, fields));
			}

			// ɾ��
			// condition: ɾ������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ��ɾ����nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶ�ɾ����
			template<typename ValueType>
			daxia::string Delete(const ValueType& obj, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>().GetLayoutFast();
				return delette(layout, &obj, condition);
			}

			// ɾ����Э�̰棩
			// condition: ɾ������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ��ɾ����nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶ�ɾ����
			template<typename ValueType>
			daxia::string CoDelete(const ValueType& obj, const FieldFilter* condition = nullptr)
			{
				CO_CALL(Delete(obj, condition));
			}

			// ��ѯһ��
			// fields: ��Ҫ��ѯ���ֶΡ�nullptr���ѯ�����ֶΡ�
			// suffix: ��׺�ַ�����������ѯ������������ȡ����� "id < 10 and name is not null order by name" ��nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶν��в�ѯ��
			// prefix: ǰ׺�ַ��������� "top(10)"��"distinct"
			template<typename ValueType>
			daxia::string Query(ValueType& obj,
				const FieldFilter* fields = nullptr,
				const char* suffix = nullptr,
				const char* prefix = nullptr
				)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				std::shared_ptr<Recordset> recordset = query(layout, &obj, fields, suffix, prefix);

				if (recordset && !recordset->Eof())
				{
					record2obj(recordset, layout, &obj, fields);
				}

				return command_->GetLastError();
			}

			// ��ѯһ����Э�̰棩
			// fields: ��Ҫ��ѯ���ֶΡ�nullptr���ѯ�����ֶΡ�
			// suffix: ��׺�ַ�����������ѯ������������ȡ����� "id < 10 and name is not null order by name" ��nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶν��в�ѯ��
			// prefix: ǰ׺�ַ��������� "top(10)"��"distinct"
			template<typename ValueType>
			daxia::string CoQuery(ValueType& obj,
				const FieldFilter* fields = nullptr,
				const char* suffix = nullptr,
				const char* prefix = nullptr
				)
			{
				CO_CALL(Query(obj, fields, suffix, prefix));
			}

			// ��ѯ����
			// fields: ��Ҫ��ѯ���ֶΡ�nullptr���ѯ�����ֶΡ�
			// suffix: ��׺�ַ�����������ѯ������������ȡ����� "id < 10 and name is not null order by name" ��nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶν��в�ѯ��
			// prefix: ǰ׺�ַ��������� "top(10)"��"distinct"
			template<typename ValueType>
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

			// ��ѯ������Э�̰棩
			// fields: ��Ҫ��ѯ���ֶΡ�nullptr���ѯ�����ֶΡ�
			// suffix: ��׺�ַ�����������ѯ������������ȡ����� "id < 10 and name is not null order by name" ��nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶν��в�ѯ��
			// prefix: ǰ׺�ַ��������� "top(10)"��"distinct"
			template<typename ValueType>
			daxia::string CoQuery(std::vector<ValueType>& objs,
				const FieldFilter* fields = nullptr,
				const char* suffix = nullptr,
				const char* prefix = nullptr
				)
			{
				CO_CALL(Query(objs, fields, suffix, prefix));
			}

			// ����
			// fields:		ָ����Ҫ���µ��ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			// condition:	��������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ�Ÿ��¡�nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶθ��¡�
			template<typename ValueType>
			daxia::string Update(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return update(layout, &obj, fields, condition);
			}

			// ���£�Э�̰棩
			// fields:		ָ����Ҫ���µ��ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			// condition:	��������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ�Ÿ��¡�nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶθ��¡�
			template<typename ValueType>
			daxia::string CoUpdate(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				CO_CALL(Update(obj, fields, condition));
			}

			// ׷�Ӹ��¡�	�������͵��ֶΣ��µ�ֵΪԭ�ȵ�ֵ������ָ����ֵ����֮�ͣ��ַ�����blob��������ĩβ׷�ӡ�
			// fields:		ָ����Ҫ���µ��ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			// condition:	��������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ�Ÿ��¡�nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶθ��¡�
			template<typename ValueType>
			daxia::string Append(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return append(layout, &obj, fields, condition);
			}

			// ׷�Ӹ��¡���Э�̰棩	�������͵��ֶΣ��µ�ֵΪԭ�ȵ�ֵ������ָ����ֵ����֮�ͣ��ַ�����blob��������ĩβ׷�ӡ�
			// fields:		ָ����Ҫ���µ��ֶ�(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��nullptr����������Ѿ���ʼ�����ֶΡ�
			// condition:	��������������¼��ָ���ֶ�ֵ(���ֶ����ʼ����δ��ʼ�����ֶλ�ǿ�ƺ���)��ͬ�Ÿ��¡�nullptr����ݾ���primary_key�������ѳ�ʼ�����ֶθ��¡�
			template<typename ValueType>
			daxia::string CoAppend(const ValueType& obj, const FieldFilter* fields = nullptr, const FieldFilter* condition = nullptr)
			{
				CO_CALL(Append(obj, fields, condition));
			}

			// ����
			template<typename ValueType>
			daxia::string Create(const ValueType& obj)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return create(layout, &obj);
			}

			// ����Э�̰棩
			template<typename ValueType>
			daxia::string CoCreate(const ValueType& obj)
			{
				CO_CALL(Create(obj));
			}

			// ɾ��
			template<typename ValueType>
			daxia::string Drop(const ValueType& obj)
			{
				using namespace daxia::reflect;

				auto layout = Reflect<ValueType>::GetLayoutFast();
				return drop(layout, &obj);
			}

			// ɾ��Э�̰棩
			template<typename ValueType>
			daxia::string CoDrop(const ValueType& obj)
			{
				CO_CALL(Drop(obj));
			}

			template<typename ValueType>
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

			// ��ȡ����ִ����
			std::shared_ptr<Command> GetCommand();
		private:
			daxia::string insert(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields);
			daxia::string delette(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* condition);
			std::shared_ptr<Recordset> query(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields, const char* suffix, const char* prefix);
			daxia::string update(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields, const FieldFilter* condition);
			daxia::string append(const daxia::reflect::Layout& layout, const void* baseaddr, const FieldFilter* fields, const FieldFilter* condition);
			daxia::string create(const daxia::reflect::Layout& layout, const void* baseaddr);
			daxia::string drop(const daxia::reflect::Layout& layout, const void* baseaddr);
		private:
			static const daxia::reflect::Reflect_base* cast(const void* baseaddr, unsigned long offset);
			static daxia::string makeConditionByPrimaryKey(const daxia::reflect::Layout& layout, const void* baseaddr);
			static void record2obj(std::shared_ptr<Recordset> recordset, const daxia::reflect::Layout& layout, void* baseaddr, const FieldFilter* fields = nullptr);
		private:
			std::shared_ptr<Command> command_;
			Driver driverType_;
			static daxia::system::ThreadPool tp_;
		private:
			class InitHelper
			{
			public:
				InitHelper();
				~InitHelper();
			};
		};
	}
}

#undef ORM
#undef CO_CALL

#endif	// !__DAXIA_DATABASE_ORM_H
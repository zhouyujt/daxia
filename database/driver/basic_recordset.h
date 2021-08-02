/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file basic_recordset.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ���ݼ��ӿ�
*
*/
#ifndef __DAXIA_DATABASE_DRIVER_BASIC_RECORDSET_H
#define __DAXIA_DATABASE_DRIVER_BASIC_RECORDSET_H
#include "data_type.h"
#include "../../string.hpp"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			class BasicRecordset
			{
			public:
				BasicRecordset(){}
				virtual ~BasicRecordset(){}
			public:
				virtual bool Eof() = 0;
				virtual void Next() = 0;
				virtual size_t Count() = 0;
				virtual long long  ScopeIdentity() = 0;
				template<class ValueType>
				ValueType Get(const char* field)
				{
					ValueType v;
					GetField(field, v);
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
		}
	}
}

#endif // !__DAXIA_DATABASE_DRIVER_BASIC_RECORDSET_H
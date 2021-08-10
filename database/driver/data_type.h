/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file data_type.h
* \author 漓江里的大虾
* \date 七月 2021
*
* 数据库数据类型定义
*
*/
#ifndef __DAXIA_DATABASE_DATA_TYPE_H
#define __DAXIA_DATABASE_DATA_TYPE_H
#include <string>
#include "../../string.hpp"
#include "../../system/datetime.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			class BasicDataType
			{
			protected:
				BasicDataType() : init_(false) {}
				~BasicDataType() {}
			public:
				bool IsInit() const{ return init_; }
			protected:
				bool init_;
			};

			template<class ValueType>
			class DataType : public BasicDataType
			{
			public:
				DataType(){}
				DataType(const DataType& dt)
				{ 
					v_ = dt.v_; 
					init_ = dt.init_;
				}
				~DataType(){}
			public:
				DataType& operator=(ValueType v)
				{
					init_ = true;
					v_ = v;
					return *this;
				}

				operator const ValueType&() const
				{
					return v_;
				}
			public:
				ValueType& Value()
				{
					return v_;
				}

				const ValueType& Value() const
				{
					return v_;
				}
			private:
				ValueType v_;
			};

			typedef DataType<char> db_tinyint;
			typedef DataType<int> db_int;
			typedef DataType<long long> db_bigint;
			typedef DataType<float> db_float;
			typedef DataType<double> db_double;
			typedef DataType<daxia::string> db_text;
			typedef DataType<daxia::buffer> db_blob;
			typedef DataType<daxia::system::DateTime> db_datetime;
		}
	}
}
#endif	// !__DAXIA_DATABASE_DATA_TYPE_H
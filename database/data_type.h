/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file data_type.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ���ݿ��������Ͷ���
*
*/
#ifndef __DAXIA_DATABASE_DATA_TYPE_H
#define __DAXIA_DATABASE_DATA_TYPE_H
#include <string>
#include "../string.hpp"
#include "../system/datetime.h"

namespace daxia
{
	namespace database
	{
		template<class ValueType>
		class DataType
		{
		public:
			DataType() : init_(false) {}
			DataType(const DataType& dt){ v_ = dt.v_; }
			~DataType(){}
		public:
			bool IsInit() const{ return init_; }
		public:
			DataType& operator=(ValueType v)
			{
				init_ = true;
				v_ = v;
				return v_;
			}

			operator ValueType&()
			{
				return v_;
			}
		private:
			ValueType v_;
			bool init_;
		};

		typedef DataType<char> db_tinyint;
		typedef DataType<int> db_int;
		typedef DataType<long long> db_bigint;
		typedef DataType<float> db_float;
		typedef DataType<double> db_double;
		typedef DataType<daxia::string> db_text;
		typedef DataType<std::string> db_blob;
		typedef DataType<daxia::system::DateTime> db_datetime;
	}
}
#endif	// !__DAXIA_DATABASE_DATA_TYPE_H
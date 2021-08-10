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

#define		JSON	"json"
#define		ORM		"orm"
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
				DataType(){ static InitHelper<ValueType> initHelper; }
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

				// 支持反射序列化
			private:
				template<class T>
				class InitHelper
				{
				public:
					InitHelper(){ init<T>(); }
				private:
					template<class T>
					void init(){}
					template<>
					void init<char>()
					{
						Reflect<DataType<char>>::SetToString(ORM, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<char>*>(data)->Value());
						});

						Reflect<DataType<char>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							DataType<char>& v = *reinterpret_cast<DataType<char>*>(data);
							v = str.NumericCast<char>();
						});

						Reflect<DataType<char>>::SetToString(JSON, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<char>*>(data)->Value());
						});

						Reflect<DataType<char>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							DataType<char>& v = *reinterpret_cast<DataType<char>*>(data);
							v = str.NumericCast<char>();
						});
					}

					template<>
					void init<int>()
					{
						Reflect<DataType<int>>::SetToString(ORM, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<int>*>(data)->Value());
						});

						Reflect<DataType<int>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							DataType<int>& v = *reinterpret_cast<DataType<int>*>(data);
							v = str.NumericCast<int>();
						});

						Reflect<DataType<int>>::SetToString(JSON, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<int>*>(data)->Value());
						});

						Reflect<DataType<int>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							DataType<int>& v = *reinterpret_cast<DataType<int>*>(data);
							v = str.NumericCast<int>();
						});
					}

					template<>
					void init<long long>()
					{
						Reflect<DataType<long long>>::SetToString(ORM, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<long long>*>(data)->Value());
						});

						Reflect<DataType<long long>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							DataType<long long>& v = *reinterpret_cast<DataType<long long>*>(data);
							v = str.NumericCast<long long>();
						});

						Reflect<DataType<long long>>::SetToString(JSON, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<long long>*>(data)->Value());
						});

						Reflect<DataType<long long>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							DataType<long long>& v = *reinterpret_cast<DataType<long long>*>(data);
							v = str.NumericCast<long long>();
						});
					}

					template<>
					void init<float>()
					{
						Reflect<DataType<float>>::SetToString(ORM, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<float>*>(data)->Value());
						});

						Reflect<DataType<float>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							DataType<long long>& v = *reinterpret_cast<DataType<float>*>(data);
							v = str.NumericCast<float>();
						});

						Reflect<DataType<float>>::SetToString(JSON, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<float>*>(data)->Value());
						});

						Reflect<DataType<float>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							DataType<long long>& v = *reinterpret_cast<DataType<float>*>(data);
							v = str.NumericCast<float>();
						});
					}

					template<>
					void init<double>()
					{
						Reflect<DataType<double>>::SetToString(ORM, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<double>*>(data)->Value());
						});

						Reflect<DataType<double>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							DataType<long long>& v = *reinterpret_cast<DataType<double>*>(data);
							v = str.NumericCast<double>();
						});

						Reflect<DataType<double>>::SetToString(JSON, [](const void* data)
						{
							return daxia::string::ToString(reinterpret_cast<const DataType<double>*>(data)->Value());
						});

						Reflect<DataType<double>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							DataType<long long>& v = *reinterpret_cast<DataType<double>*>(data);
							v = str.NumericCast<double>();
						});
					}

					template<>
					void init<daxia::string>()
					{
						Reflect<DataType<daxia::string>>::SetToString(ORM, [](const void* data)
						{
							const daxia::string& v = reinterpret_cast<const db_text*>(data)->Value();;

							daxia::string result = "\'";
							result += v + "\'";

							return result;
						});

						Reflect<DataType<daxia::string>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							db_text& v = *reinterpret_cast<db_text*>(data);
							v = str;
						});

						Reflect<DataType<daxia::string>>::SetToString(JSON, [](const void* data)
						{
							const daxia::string& v = reinterpret_cast<const db_text*>(data)->Value();;

							daxia::string temp(v);
							temp.Replace("\\", "\\\\");
							temp.Replace("\"", "\\\"");

							daxia::string result = "\"";
							result += temp + "\"";

							return result;
						});

						Reflect<DataType<daxia::string>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							DataType<daxia::string>& v = *reinterpret_cast<DataType<daxia::string>*>(data);
							v = str;
						});
					}

					template<>
					void init<daxia::buffer>()
					{
						Reflect<DataType<daxia::buffer>>::SetToString(ORM, [](const void* data)
						{
							const daxia::buffer& v = reinterpret_cast<const db_blob*>(data)->Value();

							daxia::string result = "\'";
							result += daxia::encode::Hex::ToString(v.GetString(), v.GetLength()) + "\'";

							return result;
						});

						Reflect<DataType<daxia::buffer>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							db_blob& v = *reinterpret_cast<db_blob*>(data);
							v = daxia::encode::Hex::FromString(str);
						});

						Reflect<DataType<daxia::buffer>>::SetToString(JSON, [](const void* data)
						{
							const daxia::buffer& v = reinterpret_cast<const db_blob*>(data)->Value();

							daxia::string result = "\"";
							result += daxia::encode::Hex::ToString(v.GetString(), v.GetLength()) + "\"";

							return result;
						});

						Reflect<DataType<daxia::buffer>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							db_blob& v = *reinterpret_cast<db_blob*>(data);
							v = daxia::encode::Hex::FromString(str);
						});
					}

					template<>
					void init<daxia::system::DateTime>()
					{
						Reflect<DataType<daxia::buffer>>::SetToString(ORM, [](const void* data)
						{
							const daxia::system::DateTime& v = reinterpret_cast<const db_datetime*>(data)->Value();

							daxia::string result = "\'";
							result += v.ToString() + "\'";

							return result;
						});

						Reflect<DataType<daxia::system::DateTime>>::SetFromString(ORM, [](const daxia::string& str, void* data)
						{
							db_datetime& v = *reinterpret_cast<db_datetime*>(data);
							v = daxia::system::DateTime(str.GetString());
						});

						Reflect<DataType<daxia::system::DateTime>>::SetToString(JSON, [](const void* data)
						{
							const daxia::system::DateTime& v = reinterpret_cast<const db_datetime*>(data)->Value();

							daxia::string result = "\"";
							result += v.ToString() + "\"";

							return result;
						});

						Reflect<DataType<daxia::system::DateTime>>::SetFromString(JSON, [](const daxia::string& str, void* data)
						{
							db_datetime& v = *reinterpret_cast<db_datetime*>(data);
							v = daxia::system::DateTime(str.GetString());
						});
					}
				};
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
#undef JSON
#undef ORM
#endif	// !__DAXIA_DATABASE_DATA_TYPE_H
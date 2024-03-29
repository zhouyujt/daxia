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
#include "../../reflect/reflect.hpp"
#include "../../encode/hex.h"

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
				bool IsAssign() const{ return init_; }
			protected:
				bool init_;
			};

			template<class ValueType>
			class DataType : public BasicDataType
			{
			public:
				DataType() : v_{}{}
				DataType(const DataType& dt)
				{
					v_ = dt.v_;
					init_ = dt.init_;
				}
				~DataType(){}
			public:
				operator ValueType&()
				{
					return v_;
				}

				operator const ValueType&() const
				{
					return v_;
				}

				DataType& operator=(const DataType& v)
				{
					init_ = v.init_;
					v_ = v.v_;
					return *this;
				}

				ValueType& operator=(const ValueType& v)
				{
					init_ = true;
					v_ = v;
					return v_;
				}
			public:
				// 支持反射序列化
				inline static void Init();
			private:
				ValueType v_;
			};

			template<>
			inline void daxia::database::driver::DataType<char>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<char>>::SetToString(ORM, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const char&>(*reinterpret_cast<const DataType<char>*>(data)));
				});

				Reflect<DataType<char>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<char>& v = *reinterpret_cast<DataType<char>*>(data);
					v = str.NumericCast<char>();
				});

				Reflect<DataType<char>>::SetToString(JSON, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const char&>(*reinterpret_cast<const DataType<char>*>(data)));
				});

				Reflect<DataType<char>>::SetFromString(JSON, [](const daxia::string& str, void* data)
				{
					DataType<char>& v = *reinterpret_cast<DataType<char>*>(data);
					v = str.NumericCast<char>();
				});
			}

			template<>
			inline void daxia::database::driver::DataType<int>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<int>>::SetToString(ORM, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const int&>(*reinterpret_cast<const DataType<int>*>(data)));
				});

				Reflect<DataType<int>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<int>& v = *reinterpret_cast<DataType<int>*>(data);
					v = str.NumericCast<int>();
				});

				Reflect<DataType<int>>::SetToString(JSON, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const int&>(*reinterpret_cast<const DataType<int>*>(data)));
				});

				Reflect<DataType<int>>::SetFromString(JSON, [](const daxia::string& str, void* data)
				{
					DataType<int>& v = *reinterpret_cast<DataType<int>*>(data);
					v = str.NumericCast<int>();
				});
			}

			template<>
			inline void daxia::database::driver::DataType<long long>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<long long>>::SetToString(ORM, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const long long&>(*reinterpret_cast<const DataType<long long>*>(data)));
				});

				Reflect<DataType<long long>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<long long>& v = *reinterpret_cast<DataType<long long>*>(data);
					v = str.NumericCast<long long>();
				});

				Reflect<DataType<long long>>::SetToString(JSON, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const long long&>(*reinterpret_cast<const DataType<long long>*>(data)));
				});

				Reflect<DataType<long long>>::SetFromString(JSON, [](const daxia::string& str, void* data)
				{
					DataType<long long>& v = *reinterpret_cast<DataType<long long>*>(data);
					v = str.NumericCast<long long>();
				});
			}

			template<>
			inline void daxia::database::driver::DataType<float>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<float>>::SetToString(ORM, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const float&>(*reinterpret_cast<const DataType<float>*>(data)));
				});

				Reflect<DataType<float>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<float>& v = *reinterpret_cast<DataType<float>*>(data);
					v = str.NumericCast<float>();
				});

				Reflect<DataType<float>>::SetToString(JSON, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const float&>(*reinterpret_cast<const DataType<float>*>(data)));
				});

				Reflect<DataType<float>>::SetFromString(JSON, [](const daxia::string& str, void* data)
				{
					DataType<float>& v = *reinterpret_cast<DataType<float>*>(data);
					v = str.NumericCast<float>();
				});
			}

			template<>
			inline void daxia::database::driver::DataType<double>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<double>>::SetToString(ORM, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const double&>(*reinterpret_cast<const DataType<double>*>(data)));
				});

				Reflect<DataType<double>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<double>& v = *reinterpret_cast<DataType<double>*>(data);
					v = str.NumericCast<double>();
				});

				Reflect<DataType<double>>::SetToString(JSON, [](const void* data)
				{
					return daxia::string::ToString(static_cast<const double&>(*reinterpret_cast<const DataType<double>*>(data)));
				});

				Reflect<DataType<double>>::SetFromString(JSON, [](const daxia::string& str, void* data)
				{
					DataType<double>& v = *reinterpret_cast<DataType<double>*>(data);
					v = str.NumericCast<double>();
				});
			}

			template<>
			inline void daxia::database::driver::DataType<daxia::string>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<daxia::string>>::SetToString(ORM, [](const void* data)
				{
					const daxia::string& v = static_cast<const daxia::string&>(*reinterpret_cast<const DataType<daxia::string>*>(data));

					daxia::string result = "\'";
					result += v + "\'";

					return result;
				});

				Reflect<DataType<daxia::string>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<daxia::string>& v = *reinterpret_cast<DataType<daxia::string>*>(data);
					v = str;
				});

				Reflect<DataType<daxia::string>>::SetToString(JSON, [](const void* data)
				{
					const daxia::string& v = static_cast<const daxia::string&>(*reinterpret_cast<const DataType<daxia::string>*>(data));

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
			inline void daxia::database::driver::DataType<daxia::buffer>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<daxia::buffer>>::SetToString(ORM, [](const void* data)
				{
					//const daxia::buffer& v = static_cast<const daxia::buffer&>(*reinterpret_cast<const DataType<daxia::buffer>*>(data));

					//daxia::string result = "\'";
					//result += daxia::encode::Hex::ToString(v.GetString(), v.GetLength()) + "\'";

					daxia::string result = "?";

					return result;
				});

				Reflect<DataType<daxia::buffer>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<daxia::buffer>& v = *reinterpret_cast<DataType<daxia::buffer>*>(data);
					//v = daxia::encode::Hex::FromString(str);
					v = static_cast<const daxia::buffer&>(str);
				});

				Reflect<DataType<daxia::buffer>>::SetToString(JSON, [](const void* data)
				{
					const daxia::buffer& v = static_cast<const daxia::buffer&>(*reinterpret_cast<const DataType<daxia::buffer>*>(data));

					daxia::string result = "\"";
					result += daxia::encode::Hex::Marshal(v.GetString(), v.GetLength()) + "\"";

					return result;
				});

				Reflect<DataType<daxia::buffer>>::SetFromString(JSON, [](const daxia::string& str, void* data)
				{
					DataType<daxia::buffer>& v = *reinterpret_cast<DataType<daxia::buffer>*>(data);
					v = daxia::encode::Hex::Unmarshal(str);
				});
			}

			template<>
			inline void daxia::database::driver::DataType<daxia::system::DateTime>::Init()
			{
				using namespace daxia::reflect;

				Reflect<DataType<daxia::system::DateTime>>::SetToString(ORM, [](const void* data)
				{
					const daxia::system::DateTime& v = static_cast<const daxia::system::DateTime&>(*reinterpret_cast<const DataType<daxia::system::DateTime>*>(data));

					daxia::string result = "\'";
					result += v.ToString() + "\'";

					return result;
				});

				Reflect<DataType<daxia::system::DateTime>>::SetFromString(ORM, [](const daxia::string& str, void* data)
				{
					DataType<daxia::system::DateTime>& v = *reinterpret_cast<DataType<daxia::system::DateTime>*>(data);
					v = daxia::system::DateTime(str.GetString());
				});

				Reflect<DataType<daxia::system::DateTime>>::SetToString(JSON, [](const void* data)
				{
					const daxia::system::DateTime& v = static_cast<const daxia::system::DateTime&>(*reinterpret_cast<const DataType<daxia::system::DateTime>*>(data));

					daxia::string result = "\"";
					result += v.ToString() + "\"";

					return result;
				});

				Reflect<DataType<daxia::system::DateTime>>::SetFromString(JSON, [](const daxia::string& str, void* data)
				{
					DataType<daxia::system::DateTime>& v = *reinterpret_cast<DataType<daxia::system::DateTime>*>(data);
					v = daxia::system::DateTime(str.GetString());
				});
			}

			using db_tinyint = DataType < char > ;
			using db_int = DataType < int > ;
			using db_bigint = DataType < long long > ;
			using db_float = DataType < float > ;
			using db_double = DataType < double > ;
			using db_text = DataType < daxia::string > ;
			using db_blob = DataType < daxia::buffer > ;
			using db_datetime = DataType < daxia::system::DateTime > ;

			using ref_db_tinyint = daxia::reflect::Reflect < db_tinyint > ;
			using ref_db_int = daxia::reflect::Reflect < db_int > ;
			using ref_db_bigint = daxia::reflect::Reflect < db_bigint > ;
			using ref_db_float = daxia::reflect::Reflect < db_float > ;
			using ref_db_double = daxia::reflect::Reflect < db_double > ;
			using ref_db_text = daxia::reflect::Reflect < db_text > ;
			using ref_db_blob = daxia::reflect::Reflect < db_blob > ;
			using ref_db_datetime = daxia::reflect::Reflect < db_datetime > ;
		}
	}
}

#undef JSON
#undef ORM
#endif	// !__DAXIA_DATABASE_DATA_TYPE_H
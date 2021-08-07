/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file reflect.hpp
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 * 提供运行时的反射功能：
 * 标签、类型、内存布局
 * 标签格式举例："orm:id(identity) json:id other:myid(attribute1 attribute2=xxxx attribute3)"
 *
 */

#ifndef __DAXIA_REFLECT_REFLECT_HPP
#define __DAXIA_REFLECT_REFLECT_HPP

#include <string>
#include <vector>
#include <sstream>
#include "reflect_base.h" 

namespace daxia
{
	namespace reflect
	{
		template<class ValueType>
		class Reflect : public Reflect_base
		{
		public:
			Reflect()
				: Reflect_base(nullptr)
			{
				init(&this->v_);
			}

			Reflect(const char* tags)
				: Reflect_base(tags)
			{
				init(&this->v_);
			}

			~Reflect(){}
		public:
			Reflect_base& Swap(Reflect_base& r)
			{
				Reflect<ValueType>* instance = dynamic_cast<Reflect<ValueType>*>(&r);
				if (instance != nullptr)
				{
					std::swap(v_, instance->v_);
					this->Reflect_base::Swap(r);
				}

				return *this;
			};

			operator ValueType&()
			{
				return v_;
			}

			operator const ValueType&() const
			{
				return v_;
			}

			Reflect& operator=(const Reflect& v)
			{
				v_ = v.v_;
				return *this;
			}
		public:
			virtual const reflect::Layout& GetLayout() const override { return layout_; }
			static reflect::Layout& GetLayoutFast() { if (layout_.Type() == reflect::Layout::unset){ Reflect<ValueType>(); } return layout_/*直接读取静态变量，不走虚函数表，性能提升巨大*/; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual size_t Size() const override { return sizeof(*this); }
			virtual const std::type_info& Type() const override { return typeid(ValueType); }
			inline virtual daxia::string ToString() const override;
			virtual daxia::string ToStringOfElement(size_t index) const override { throw "don't call this method!"; }

			ValueType& Value(){ return v_; }
			const ValueType& Value() const { return v_; }
		private:
			// 数组信息
			struct ArrayInfo
			{
				std::string firstTag;
				Layout layout;
			};

			static void init(const void* baseaddr);
			static void makeObjectFields(const char* baseaddr, const char* start, const char* end, std::vector<daxia::reflect::Field>& fields);
		private:
			ValueType v_;
			static reflect::Layout layout_;
			class InitHelper
			{
			public:
				InitHelper(const void* baseaddr)
				{
					if (!std::is_class<ValueType>::value
						|| std::is_same<ValueType, std::string>::value
						|| std::is_same<ValueType, std::wstring>::value
						|| std::is_same<ValueType, daxia::string>::value
						|| std::is_same<ValueType, daxia::wstring>::value
						)
						// value
					{
						layout_.Type() = reflect::Layout::value;
						layout_.Size() = sizeof(ValueType);
					}
					else
						// object
					{
						layout_.Type() = reflect::Layout::object;
						layout_.Size() = sizeof(ValueType);
						const char* start = reinterpret_cast<const char*>(baseaddr);
						const char* end = start + sizeof(ValueType);
						makeObjectFields(start, start, end, layout_.Fields());
					}

					// vector
					// 特化处理
				}
			};
		};// class Reflect

		template<class ValueType>
		reflect::Layout Reflect<ValueType>::layout_;

		template<class ValueType> daxia::string daxia::reflect::Reflect<ValueType>::ToString() const { return daxia::string(); }
		template<> daxia::string daxia::reflect::Reflect<char>::ToString() const { return daxia::string::ToString(static_cast<int>(v_)); }
		template<> daxia::string daxia::reflect::Reflect<unsigned char>::ToString() const { return daxia::string::ToString(static_cast<unsigned int>(v_)); }
		template<> daxia::string daxia::reflect::Reflect<int>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<unsigned int>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<long>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<unsigned long>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<long long>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<unsigned long long>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<long double>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<double>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<float>::ToString() const { return daxia::string::ToString(v_); }
		template<> daxia::string daxia::reflect::Reflect<bool>::ToString() const { return v_ ? "true" : "false"; }
		template<> daxia::string daxia::reflect::Reflect<std::string>::ToString() const { daxia::string str; str.Format("\"%s\"", v_.c_str()); return str; }
		template<> daxia::string daxia::reflect::Reflect<std::wstring>::ToString() const { daxia::string str; str.Format("\"%s\"", daxia::wstring(v_).ToAnsi().GetString()); return str; }
		template<> daxia::string daxia::reflect::Reflect<daxia::string>::ToString() const { daxia::string str; str.Format("\"%s\"", v_.GetString()); return str; }
		template<> daxia::string daxia::reflect::Reflect<daxia::wstring>::ToString() const { daxia::string str; str.Format("\"%s\"", v_.ToAnsi().GetString()); return str; }


		template<class ValueType>
		void daxia::reflect::Reflect<ValueType>::init(const void* baseaddr)
		{
			static InitHelper initHelper(baseaddr);
		}

		template<class ValueType>
		void daxia::reflect::Reflect<ValueType>::makeObjectFields(const char* baseaddr, const char* start, const char* end, std::vector<daxia::reflect::Field>& fields)
		{
			fields.clear();
			for (; start < end; ++start)
			{
				const Reflect_base* reflectBase = nullptr;

				// 根据前4个字节判断是不是Reflect_helper
				// 如果省略这一步，MSVC编译器工作正常
				// gcc dynamic_cast 会报segmentation fault
				if (!Reflect_helper::IsValidReflect(start)) continue;

				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(start)); }
				catch (const std::exception&){}

				if (reflectBase == nullptr) continue;

				Field field;
				field.hashcode = reflectBase->Type().hash_code();
				field.offset = reinterpret_cast<size_t>(start)-reinterpret_cast<size_t>(baseaddr);
				field.size = reflectBase->Size();
				fields.push_back(field);

				start += reflectBase->Size() - 1;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 针对std::vector<ValueType>进行特化
		template<class ValueType>
		class Reflect<std::vector<ValueType>> : public Reflect_base
		{
		public:
			Reflect()
				: Reflect_base(nullptr)
			{
				init();
			}

			Reflect(const char* tags)
				: Reflect_base(tags)
			{
				init();
			}

			~Reflect(){}
		public:
			Reflect_base& Swap(Reflect_base& r)
			{
				Reflect<std::vector<ValueType>>* instance = dynamic_cast<Reflect<std::vector<ValueType>>*>(&r);
				if (instance != nullptr)
				{
					std::swap(v_, instance->v_);
					this->Reflect_base::Swap(r);
				}

				return *this;
			};

			operator std::vector<ValueType>&()
			{
				return v_;
			}

			operator const std::vector<ValueType>&() const
			{
				return v_;
			}

			Reflect& operator=(const Reflect& v)
			{
				v_ = v.v_;
				return *this;
			}
		public:
			virtual const reflect::Layout& GetLayout() const override { return layout_; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual size_t Size() const override { return sizeof(*this); }
			virtual const std::type_info& Type() const override { return typeid(std::vector<ValueType>); }
			virtual daxia::string ToString() const override { throw "don't call this method!"; }
			inline virtual daxia::string ToStringOfElement(size_t index) const override;
			std::vector<ValueType>& Value(){ return v_; }
			const std::vector<ValueType>& Value() const { return v_; }
		private:
			// 数组信息
			struct ArrayInfo
			{
				daxia::string firstTag;
				reflect::Layout layout;
			};

			static void init();
		private:
			std::vector<ValueType> v_;
			static reflect::Layout layout_;
			class InitHelper
			{
			public:
				InitHelper()
				{
					// vector 类型保存元素的布局
					layout_ = Reflect<ValueType>::GetLayoutFast();

					// 每个元素的大小
					layout_.ElementSize() = sizeof(ValueType);

					layout_.Type() = daxia::reflect::Layout::vecotr;
					layout_.Size() = sizeof(std::vector<ValueType>);
				}
			};
		};

		template<class ValueType>
		reflect::Layout Reflect<std::vector<ValueType>>::layout_;

		template<class ValueType> daxia::string daxia::reflect::Reflect<std::vector<ValueType>>::ToStringOfElement(size_t index) const { return daxia::string(); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<char>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(static_cast<int>(v_[index])); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<unsigned char>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(static_cast<unsigned int>(v_[index])); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<int>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<unsigned int>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<long>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<unsigned long>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<long long>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<unsigned long long>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<long double>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<double>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<float>>::ToStringOfElement(size_t index) const { return daxia::string::ToString(v_[index]); }
		template<> daxia::string daxia::reflect::Reflect<std::vector<bool>>::ToStringOfElement(size_t index) const { return v_[index] ? "true" : "false"; }
		template<> daxia::string daxia::reflect::Reflect<std::vector<std::string>>::ToStringOfElement(size_t index) const { daxia::string str; str.Format("\"%s\"", v_[index].c_str()); return str; }
		template<> daxia::string daxia::reflect::Reflect<std::vector<std::wstring>>::ToStringOfElement(size_t index) const { daxia::string str; str.Format("\"%s\"", daxia::wstring(v_[index]).ToAnsi().GetString()); return str; }
		template<> daxia::string daxia::reflect::Reflect<std::vector<daxia::string>>::ToStringOfElement(size_t index) const { daxia::string str; str.Format("\"%s\"", v_[index].GetString()); return str; }
		template<> daxia::string daxia::reflect::Reflect<std::vector<daxia::wstring>>::ToStringOfElement(size_t index) const { daxia::string str; str.Format("\"%s\"", v_[index].ToAnsi().GetString()); return str; }

		template<class ValueType>
		void daxia::reflect::Reflect<std::vector<ValueType>>::init()
		{
			static InitHelper initHelper;
		}

		typedef Reflect<bool> Bool;
		typedef Reflect<char> Char;
		typedef Reflect<unsigned char> UChar;
		typedef Reflect<short> Short;
		typedef Reflect<unsigned short> UShort;
		typedef Reflect<int> Int;
		typedef Reflect<unsigned int> UInt;
		typedef Reflect<long> Long;
		typedef Reflect<unsigned long> ULong;
		typedef Reflect<long long> LLong;
		typedef Reflect<unsigned long long> ULLong;
		typedef Reflect<std::string> String;
		template <typename T> class Vector : public Reflect<std::vector<T>>
		{
		public:
			Vector()
				: Reflect<std::vector<T>>()
			{
			}

			Vector(const std::string& tags)
				: Reflect<std::vector<T>>(tags)
			{
			}

			Vector(const char* tags)
				: Reflect<std::vector<T>>(tags)
			{
			}

		};
	}// namespace reflect
}// namespace daxia

#endif // !__DAXIA_REFLECT_REFLECT_HPP


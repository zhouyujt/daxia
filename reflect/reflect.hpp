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
#include <map>
#include <sstream>
#include <functional>
#include "reflect_base.h" 
#include "../string.hpp"

namespace daxia
{
	namespace reflect
	{
		template<typename ValueType>
		class Reflect : public Reflect_base
		{
		public:
			Reflect()
				: Reflect_base(nullptr)
			{
				init(&this->v_);
			}

			explicit Reflect(const char* tags)
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

			Reflect& operator=(const ValueType& v)
			{
				v_ = v;
				return *this;
			}
		public:
			virtual const reflect::Layout& GetLayout() const override { return layout_; }
			static reflect::Layout& GetLayoutFast() { if (layout_.Type() == reflect::Layout::unset){ Reflect<ValueType>(); } return layout_/*直接读取静态变量，不走构造函数以免重新解析tag*/; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual size_t Size() const override { return sizeof(*this); }
			virtual void ResizeArray(size_t count) override { throw "don't call this method!"; }
			virtual const std::type_info& Type() const override { return typeid(ValueType); }
			inline virtual daxia::string ToString(const char* tag, size_t arrayElementIndex = -1) const override;
			inline virtual void FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex = -1) override;

			// 设置跟字符串相互转换的方法
			static void SetToString(const char* tag, std::function<daxia::string(const void*)> func) { tostringFuncs_[tag] = func; GetLayoutFast().Type() = reflect::Layout::value; }
			static void SetFromString(const char* tag, std::function<void(const daxia::string&, void*)> func) { fromstringFuncs_[tag] = func; GetLayoutFast().Type() = reflect::Layout::value; }
		private:
			static void init(const void* baseaddr);
			static void makeObjectFields(const char* baseaddr, const char* start, const char* end, std::vector<daxia::reflect::Field>& fields);
		private:
			ValueType v_;
			static reflect::Layout layout_;
			static std::map<daxia::string, std::function<daxia::string(const void*)>> tostringFuncs_;
			static std::map<daxia::string, std::function<void(const daxia::string&, void*)>> fromstringFuncs_;
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

		template<typename ValueType> reflect::Layout Reflect<ValueType>::layout_;
		template<typename ValueType> std::map<daxia::string, std::function<daxia::string(const void*)>> Reflect<ValueType>::tostringFuncs_;
		template<typename ValueType> std::map<daxia::string, std::function<void(const daxia::string&, void*)>> Reflect<ValueType>::fromstringFuncs_;

		template<typename ValueType> inline daxia::string daxia::reflect::Reflect<ValueType>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string(); }
		template<> inline daxia::string daxia::reflect::Reflect<char>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(static_cast<int>(v_)); }
		template<> inline daxia::string daxia::reflect::Reflect<unsigned char>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(static_cast<unsigned int>(v_)); }
		template<> inline daxia::string daxia::reflect::Reflect<int>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); }return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<unsigned int>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); }return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<long>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<unsigned long>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<long long>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<unsigned long long>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<long double>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<double>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<float>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(v_); }
		template<> inline daxia::string daxia::reflect::Reflect<bool>::ToString(const char* tag, size_t arrayElementIndex) const { auto iter = tostringFuncs_.find(tag); if (iter != tostringFuncs_.end()) { return iter->second(&v_); } return daxia::string::ToString(static_cast<int>(v_)); }

		template<typename ValueType> inline void daxia::reflect::Reflect<ValueType>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; } }
		template<> inline void daxia::reflect::Reflect<char>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<char>(); }
		template<> inline void daxia::reflect::Reflect<unsigned char>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<unsigned char>(); }
		template<> inline void daxia::reflect::Reflect<int>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<int>(); }
		template<> inline void daxia::reflect::Reflect<unsigned int>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<unsigned int>(); }
		template<> inline void daxia::reflect::Reflect<long>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<long>(); }
		template<> inline void daxia::reflect::Reflect<unsigned long>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; } v_ = str.NumericCast<unsigned long>(); }
		template<> inline void daxia::reflect::Reflect<long long>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; } v_ = str.NumericCast<long long>(); }
		template<> inline void daxia::reflect::Reflect<unsigned long long>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<unsigned long long>(); }
		template<> inline void daxia::reflect::Reflect<long double>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<long double>(); }
		template<> inline void daxia::reflect::Reflect<double>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<double>(); }
		template<> inline void daxia::reflect::Reflect<float>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<float>(); }
		template<> inline void daxia::reflect::Reflect<bool>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) { auto iter = fromstringFuncs_.find(tag); if (iter != fromstringFuncs_.end()) { iter->second(str, &v_); return; }  v_ = str.NumericCast<char>() != 0; }


		template<typename ValueType>
		void daxia::reflect::Reflect<ValueType>::init(const void* baseaddr)
		{
			static InitHelper initHelper(baseaddr);
		}

		template<typename ValueType>
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
		template<typename ValueType>
		class Reflect<std::vector<ValueType>> : public Reflect_base
		{
		public:
			Reflect()
				: Reflect_base(nullptr)
			{
				init();
			}

			explicit Reflect(const char* tags)
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

			Reflect& operator=(const std::vector<ValueType>& v)
			{
				v_ = v;
				return *this;
			}
		public:
			virtual const reflect::Layout& GetLayout() const override { return layout_; }
			static reflect::Layout& GetLayoutFast() { if (layout_.Type() == reflect::Layout::unset){ Reflect<std::vector<ValueType>>(); } return layout_/*直接读取静态变量，不走构造函数以免重新解析tag*/; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual size_t Size() const override { return sizeof(*this); }
			virtual void ResizeArray(size_t count) override 
			{ 
				static ValueType tempValue; 
				std::vector<ValueType> temp(count, tempValue); 
				std::swap(temp, v_); 
			}
			virtual const std::type_info& Type() const override { return typeid(std::vector<ValueType>); }
			inline virtual daxia::string ToString(const char* tag, size_t arrayElementIndex = -1) const override;
			inline virtual void FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex = -1) override;
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

		template<typename ValueType> reflect::Layout Reflect<std::vector<ValueType>>::layout_;

		template<typename ValueType> daxia::string daxia::reflect::Reflect<std::vector<ValueType>>::ToString(const char* tag, size_t arrayElementIndex) const
		{
			if (arrayElementIndex < v_.size())
			{
				Reflect<ValueType> temp;
				temp = v_[arrayElementIndex];
				return temp.ToString(tag);
			}

			return daxia::string();
		}

		template<typename ValueType> void daxia::reflect::Reflect<std::vector<ValueType>>::FromString(const char* tag, const daxia::string& str, size_t arrayElementIndex) 
		{
			if (arrayElementIndex < v_.size())
			{
				Reflect<ValueType> temp;
				temp.FromString(tag,str);
				v_[arrayElementIndex] = temp;
			}
		}


		template<typename ValueType>
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
		typedef Reflect<daxia::string> String;
		template <typename T> class Vector : public Reflect<std::vector<T>>
		{
		public:
			Vector()
				: Reflect<std::vector<T>>()
			{
			}

			explicit Vector(const char* tags)
				: Reflect<std::vector<T>>(tags)
			{
			}

		};
	}// namespace reflect
}// namespace daxia

#endif // !__DAXIA_REFLECT_REFLECT_HPP


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

#define REFLECT_LAYOUT_FIELD_HASH "hash_reflect_layout_field"
#define REFLECT_LAYOUT_FIELD_OFFSET "offset_reflect_layout_field"
#define REFLECT_LAYOUT_FIELD_SIZE "size_reflect_layout_field"

namespace daxia
{
	namespace reflect
	{
		template<class ValueType>
		class Reflect : public Reflect_base
		{
		public:
			Reflect()
				: Reflect_base(sizeof(*this), typeid(ValueType))
			{
				init();
			}

			Reflect(const std::string& tags)
				: Reflect_base(sizeof(*this), typeid(ValueType), tags)
			{
				init();
			}

			Reflect(const char* tags)
				: Reflect_base(sizeof(*this), typeid(ValueType), tags)
			{
				init();
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
			virtual const boost::property_tree::ptree& Layout() const override { return layout_; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual bool IsArray() const override { return false; }
			virtual void ResizeArray(size_t count) override {/*do nothing*/ }
			inline virtual daxia::string ToString() const override;
			ValueType& Value(){ return v_; }
			const ValueType& Value() const { return v_; }
		private:
			// 数组信息
			struct ArrayInfo
			{
				std::string firstTag;
				boost::property_tree::ptree layout;
			};

			void init();
			void buildLayout(const char* baseaddr,
				const char* start,
				const char* end,
				boost::property_tree::ptree& rootLayout,
				ArrayInfo* parentArray) const;
		private:
			ValueType v_;
			static boost::property_tree::ptree layout_;
			static std::mutex layoutLocker_;
		};// class Reflect

		template<class ValueType>
		boost::property_tree::ptree Reflect<ValueType>::layout_;
		template<class ValueType>
		std::mutex Reflect<ValueType>::layoutLocker_;

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
		void daxia::reflect::Reflect<ValueType>::init()
		{
			// buildLayout
			if (std::is_class<ValueType>::value 
				&& !std::is_same<ValueType, std::string>::value
				&& !std::is_same<ValueType, std::wstring>::value
				&& !std::is_same<ValueType, daxia::string>::value
				&& !std::is_same<ValueType, daxia::wstring>::value
				)
			{
				layoutLocker_.lock();

				if (layout_.empty())
				{
					const char* start = reinterpret_cast<const char*>(&this->v_);
					const char* end = reinterpret_cast<const char*>(&this->v_) + sizeof(ValueType);
					buildLayout(start, start, end, layout_, nullptr);
				}

				layoutLocker_.unlock();
			}
		}

		template<class ValueType>
		void daxia::reflect::Reflect<ValueType>::buildLayout(const char* baseaddr, const char* start, const char* end, boost::property_tree::ptree& rootLayout, ArrayInfo* parentArray) const
		{
			try
			{
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

					boost::property_tree::ptree childLayout;
					childLayout.put(REFLECT_LAYOUT_FIELD_HASH, reflectBase->Type().hash_code());
					childLayout.put(REFLECT_LAYOUT_FIELD_OFFSET, reinterpret_cast<size_t>(start)-reinterpret_cast<size_t>(baseaddr));
					rootLayout.put_child(reflectBase->Tags().GetString(), childLayout);

					start += reflectBase->Size() - 1;
				}
			}
			catch (boost::property_tree::ptree_error)
			{

			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 针对std::vector<ValueType>进行特化
		template<class ValueType>
		class Reflect<std::vector<ValueType>> : public Reflect_base
		{
		public:
			Reflect()
				: Reflect_base(sizeof(*this), typeid(std::vector<ValueType>))
			{
				init();
			}

			Reflect(const std::string& tags)
				: Reflect_base(sizeof(*this), typeid(std::vector<ValueType>), tags)
			{
				init();
			}

			Reflect(const char* tags)
				: Reflect_base(sizeof(*this), typeid(std::vector<ValueType>), tags)
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
			virtual const boost::property_tree::ptree& Layout() const override { return layout_; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual bool IsArray() const override { return true; }
			virtual void ResizeArray(size_t count) override
			{
				ValueType tempValue;
				std::vector<ValueType> temp(count, tempValue);
				std::swap(temp, v_);
			}
			virtual daxia::string ToString() const override { return daxia::string(); }
			std::vector<ValueType>& Value(){ return v_; }
			const std::vector<ValueType>& Value() const { return v_; }
		private:
			// 数组信息
			struct ArrayInfo
			{
				daxia::string firstTag;
				boost::property_tree::ptree layout;
			};

			void init()
			{
				// buildLayout
				if (std::is_class<ValueType>::value 
					&& !std::is_same<ValueType, std::string>::value
					&& !std::is_same<ValueType, std::wstring>::value
					&& !std::is_same<ValueType, daxia::string>::value
					&& !std::is_same<ValueType, daxia::wstring>::value
					)
				{
					layoutLocker_.lock();

					if (layout_.empty())
					{
						// vector 类型仅仅保存元素的布局
						layout_ = Reflect<ValueType>().Layout();

						// 每个元素的大小
						if (!layout_.empty()) layout_.put(REFLECT_LAYOUT_FIELD_SIZE, sizeof(ValueType));
					}

					layoutLocker_.unlock();
				}
			}

		private:
			std::vector<ValueType> v_;
			static boost::property_tree::ptree layout_;
			static std::mutex layoutLocker_;
		};

		template<class ValueType>
		boost::property_tree::ptree Reflect<std::vector<ValueType>>::layout_;
		template<class ValueType>
		std::mutex Reflect<std::vector<ValueType>>::layoutLocker_;

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


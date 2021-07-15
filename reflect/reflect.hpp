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
 *
 */

#ifndef __DAXIA_REFLECT_REFLECT_HPP
#define __DAXIA_REFLECT_REFLECT_HPP

#include <string>
#include <vector>
#include "reflect_base.h" 

#define HASH "hash"
#define OFFSET "offset"
#define SIZE "size"

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

			Reflect& operator=(const Reflect& r)
			{
				Reflect(r).Swap(*this);
				return *this;
			}

			Reflect& operator=(Reflect&& r)
			{
				r.Swap(*this);
				return *this;
			}

			template<class ValueType>
			Reflect& operator=(ValueType&& v)
			{
				v_ = v;

				return *this;
			}

			operator ValueType()
			{
				return v_;
			}

			operator const ValueType&() const
			{
				return v_;
			}
		public:
			virtual const boost::property_tree::ptree& Layout() const override { return layout_; }
			virtual const void* ValueAddr() const override { return &v_; }
			virtual bool IsArray() const override { return false; }
			virtual void ResizeArray(size_t count) override {/*do nothing*/ }
			ValueType& Value(){ return v_; }
			const ValueType& Value() const { return v_; }
		private:
			// 数组信息
			struct ArrayInfo
			{
				std::string firstTag;
				boost::property_tree::ptree layout;
			};

			void init()
			{
				// buildLayout
				if (std::is_class<ValueType>::value && !std::is_same<ValueType, std::string>::value)
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

			void buildLayout(const char* baseaddr,
				const char* start,
				const char* end,
				boost::property_tree::ptree& rootLayout,
				ArrayInfo* parentArray) const
			{
				try
				{
					for (; start < end; ++start)
					{
						const Reflect_base* reflectBase = nullptr;

						try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(start)); }
						catch (const std::exception&){}

						if (reflectBase == nullptr) continue;

						boost::property_tree::ptree childLayout;
						childLayout.put(HASH, reflectBase->Type().hash_code());
						childLayout.put(OFFSET, reinterpret_cast<unsigned long>(start)-reinterpret_cast<unsigned long>(baseaddr));
						rootLayout.put_child(reflectBase->Tags(), childLayout);

						start += reflectBase->Size() - 1;
					}
				}
				catch (boost::property_tree::ptree_error)
				{

				}
			}

		private:
			ValueType v_;
			static boost::property_tree::ptree layout_;
			static std::mutex layoutLocker_;
		};// class reflect

		template<class ValueType>
		boost::property_tree::ptree Reflect<ValueType>::layout_;
		template<class ValueType>
		std::mutex Reflect<ValueType>::layoutLocker_;

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

			Reflect& operator=(const Reflect& r)
			{
				Reflect(r).Swap(*this);
				return *this;
			}

			Reflect& operator=(Reflect&& r)
			{
				r.Swap(*this);
				return *this;
			}

			template<class ValueType>
			Reflect& operator=(std::vector<ValueType>&& v)
			{
				v_ = v;

				return *this;
			}

			operator std::vector<ValueType>()
			{
				return v_;
			}

			operator std::vector<ValueType>&()
			{
				return v_;
			}

			operator const std::vector<ValueType>&() const
			{
				return v_;
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
			std::vector<ValueType>& Value(){ return v_; }
			const std::vector<ValueType>& Value() const { return v_; }
		private:
			// 数组信息
			struct ArrayInfo
			{
				std::string firstTag;
				boost::property_tree::ptree layout;
			};

			void init()
			{
				// buildLayout
				if (std::is_class<ValueType>::value && !std::is_same<ValueType, std::string>::value)
				{
					layoutLocker_.lock();

					if (layout_.empty())
					{
						// vector类型仅仅保存元素的布局
						layout_ = Reflect<ValueType>().Layout();

						// 每个元素的大小
						if (!layout_.empty()) layout_.put(SIZE, sizeof(ValueType));
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
			Vector(const std::string& tags)
			: Reflect<std::vector<T>>(tags)
			{

			}
		};
	}// namespace reflect
}// namespace daxia

#undef HASH
#undef OFFSET
#undef SIZE

#endif // !__DAXIA_REFLECT_REFLECT_HPP


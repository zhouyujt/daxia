/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file reflect.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ʮ�� 2018
 *
 * �ṩ����ʱ�ķ��书�ܣ�
 * ��ǩ�����͡��ڴ沼��
 * ��ǩ��ʽ������"orm:id(identity) json:id other:myid(attribute1 attribute2=xxxx attribute3)"
 *
 */

#ifndef __DAXIA_REFLECT_REFLECT_HPP
#define __DAXIA_REFLECT_REFLECT_HPP

#include <string>
#include <vector>
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
			// ������Ϣ
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

						// ����ǰ4���ֽ��ж��ǲ���Reflect_helper
						// ���ʡ����һ����MSVC��������������
						// gcc dynamic_cast �ᱨsegmentation fault
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
		// ���std::vector<ValueType>�����ػ�
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
			// ������Ϣ
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
						// vector���ͽ�������Ԫ�صĲ���
						layout_ = Reflect<ValueType>().Layout();

						// ÿ��Ԫ�صĴ�С
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


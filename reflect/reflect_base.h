/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file reflect_base.h
 * \author �콭��Ĵ�Ϻ
 * \date ʮ�� 2018
 *
 * �ṩ����ʱ�ķ��书�ܣ�
 * ��ǩ�����͡��ڴ沼��
 *
 */

#ifndef __DAXIA_REFLECT_REFLECT_BASE_H
#define __DAXIA_REFLECT_REFLECT_BASE_H

#include <map>
#include <vector>
#include "../string.hpp"

namespace daxia
{
	namespace reflect
	{
		// daxia::reflect  md5ֵ 81b55790f56eb5c017cced954d1c97a9
		enum TypeFlag
		{
			TypeFlag0 = 0x81b55790,
			TypeFlag1 = 0xf56eb5c0,
			TypeFlag2 = 0x17cced95,
			TypeFlag3 = 0x4d1c97a9
		};

		struct Field
		{
			size_t hashcode;
			size_t offset;
			size_t size;
			Field() : hashcode(0), offset(0), size(0){}
		};

		class Layout
		{
		public:
			enum type
			{
				value,
				object,
				vecotr,
				unset
			};
		public:
			Layout() : type_(unset), size_(0), elementSize_(0){}
			~Layout(){}
		private:
			std::vector<Field> fields_;
			type type_;
			size_t size_;

			// ������Ϣ
		private:
			size_t elementSize_;	// Ԫ�ش�С
			size_t elementCount_;	// Ԫ�ظ���
		public:
			const std::vector<Field>& Fields() const { return fields_; }
			std::vector<Field>& Fields() { return fields_; }
			type Type() const { return type_; }
			type& Type() { return type_; }
			size_t Size() const { return size_; }
			size_t& Size() { return size_; }
			size_t ElementSize() const { return elementSize_; }
			size_t& ElementSize() { return elementSize_; }
			size_t ElementCount() const { return elementCount_; }
			size_t& ElementCount() { return elementCount_; }
		};

		class Reflect_helper
		{
		protected:
			Reflect_helper();
			virtual ~Reflect_helper();
		public:
			static bool IsValidReflect(const void* addr);
		private:
			int typeflag_[4];
		};// class reflect_helper

		//////////////////////////////////////////////////////////////////////////
		class Reflect_base : public Reflect_helper
		{
		protected:
			Reflect_base(const char* tags);
			virtual ~Reflect_base();
		protected:
			Reflect_base& Swap(Reflect_base& r);
		public:
			virtual const Layout& GetLayout() const = 0;
			virtual const void* ValueAddr() const = 0;
			virtual size_t Size() const = 0;
			virtual const std::type_info& Type() const = 0;
			virtual daxia::string ToString() const = 0;
			virtual daxia::string ToStringOfElement(size_t index) const = 0;
		public:
			const daxia::string& Tags() const { return tagsStr_; }
			daxia::string Tag(const daxia::string& prefix) const;
			std::map<daxia::string,daxia::string> TagAttribute(const daxia::string& prefix) const;
		private:
			void parseTag(const daxia::string& str);
		private:
			daxia::string tagsStr_;
			std::map<daxia::string, daxia::string> tags_;
		};// class reflect_base
	}// namespace reflect
}// namespace daxia
#endif // !__DAXIA_REFLECT_REFLECT_BASE_H
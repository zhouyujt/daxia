/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file reflect_base.h
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 * 提供运行时的反射功能：
 * 标签、类型、内存布局
 *
 */

#ifndef __DAXIA_REFLECT_REFLECT_BASE_H
#define __DAXIA_REFLECT_REFLECT_BASE_H

#include <string>
#include <map>
#include <mutex>
#include <boost/property_tree/ptree.hpp>

namespace daxia
{
	namespace reflect
	{
		class Reflect_helper
		{
		protected:
			Reflect_helper();
			virtual ~Reflect_helper();

		};// class reflect_helper

		//////////////////////////////////////////////////////////////////////////
		class Reflect_base : public Reflect_helper
		{
		protected:
			Reflect_base(size_t size, const type_info& typeinfo);
			Reflect_base(size_t size, const type_info& typeinfo, const std::string& tags);
			virtual ~Reflect_base();
		protected:
			Reflect_base& Swap(Reflect_base& r);
		public:
			virtual const boost::property_tree::ptree& Layout() const = 0;
			virtual const void* ValueAddr() const = 0;
			virtual void ResizeArray(size_t count) = 0;
			virtual bool IsArray() const = 0;
		public:
			size_t Size() const{ return size_; }
			const type_info& Type() const { return typeInfo_; }
			const std::string& Tags() const { return tagsStr_; }
			std::string Tag(const std::string& prefix) const;
		private:
			void parseTag(const std::string& str);
		private:
			size_t size_;
			const std::type_info& typeInfo_;
			std::string tagsStr_;
			std::map<std::string, std::string> tags_;
		};// class reflect_base
	}// namespace reflect
}// namespace daxia
#endif // !__DAXIA_REFLECT_REFLECT_BASE_H
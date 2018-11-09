/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file reflect_base.hpp
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 * 提供运行时的反射功能：
 * 标签、类型、内存布局
 *
 */

#ifndef __DAXIA_REFLECT_REFLECT_BASE_HPP
#define __DAXIA_REFLECT_REFLECT_BASE_HPP

#include <string>
#include <map>
#include <mutex>
#include <boost/property_tree/ptree.hpp>

namespace daxia
{
	namespace reflect
	{
		class reflect_helper
		{
		protected:
			reflect_helper(){}
			virtual ~reflect_helper(){}

		};// class reflect_helper

		//////////////////////////////////////////////////////////////////////////
		class reflect_base : public reflect_helper
		{
		protected:
			reflect_base(size_t size, const type_info& typeinfo)
				: size_(size)
				, typeInfo_(typeinfo)
				, tagsStr_("-")
			{
			}

			reflect_base(size_t size, const type_info& typeinfo, const std::string& tags)
				: size_(size)
				, typeInfo_(typeinfo)
				, tagsStr_("-")
			{
				if (!tags.empty())
				{
					tagsStr_ = tags;
				}
				parseTag(tags);
			}

			virtual ~reflect_base(){}
		protected:
			reflect_base& Swap(reflect_base& r)
			{
				std::swap(size_, r.size_);
				//std::swap(typeinfo_, r.typeinfo_);
				std::swap(tags_, r.tags_);

				return *this;
			}
		public:
			virtual const boost::property_tree::ptree& Layout() const = 0;
			virtual const void* ValueAddr() const = 0;
			virtual void ResizeArray(size_t count) = 0;
			virtual bool IsArray() const = 0;
		public:
			size_t Size() const{ return size_; }

			const type_info& Type() const { return typeInfo_; }

			std::string Tag(const std::string prefix) const
			{
				std::string tag;

				auto iter = tags_.find(prefix);
				if (iter != tags_.end())
				{
					tag = iter->second;
				}

				return tag;
			};

			const std::string& Tags() const { return tagsStr_; }

		private:
			void parseTag(const std::string& str)
			{
				using namespace std;

				string::size_type spacePos = string::npos;
				string::size_type lastPos = 0;
				do
				{
					spacePos = str.find(' ', lastPos);
					string tag = spacePos == string::npos ? str.substr(lastPos) : str.substr(lastPos, spacePos - lastPos);
					string::size_type splitPos = tag.find(':');
					if (splitPos != string::npos)
					{
						string prefix = tag.substr(0, splitPos);
						string suffix = tag.substr(splitPos + 1);
						tags_[prefix] = suffix;
					}

					lastPos = spacePos + 1;
				} while (spacePos != string::npos);
			}

		private:
			size_t size_;
			const std::type_info& typeInfo_;
			std::string tagsStr_;
			std::map<std::string, std::string> tags_;

		};// class reflect_base

	}// namespace reflect
}// namespace daxia

#endif // !__DAXIA_REFLECT_REFLECT_BASE_HPP


#include <map>
#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include "reflect_base.h"

namespace daxia
{
	namespace reflect
	{

		Reflect_helper::Reflect_helper()
		{
			typeflag_[0] = TypeFlag0;
			typeflag_[1] = TypeFlag1;
			typeflag_[2] = TypeFlag2;
			typeflag_[3] = TypeFlag3;
		}

		Reflect_helper::~Reflect_helper()
		{

		}

		bool Reflect_helper::IsValidReflect(const void* addr)
		{
			if (addr != nullptr)
			{
				const Reflect_helper* test = reinterpret_cast<const Reflect_helper*>(addr);
				if (test->typeflag_[0] == TypeFlag0
					&& test->typeflag_[1] == TypeFlag1
					&& test->typeflag_[2] == TypeFlag2
					&& test->typeflag_[3] == TypeFlag3)
				{
					return true;
				}
			}
			
			return false;
		}

		Reflect_base::Reflect_base()
			: size_(0)
			, typeInfo_(typeid(void))
		{
		}

		Reflect_base::Reflect_base(size_t size, const std::type_info& typeinfo)
			: size_(size)
			, typeInfo_(typeinfo)
			, tagsStr_("-")
		{
		}

		Reflect_base::Reflect_base(size_t size, const std::type_info& typeinfo, const daxia::string& tags)
			: size_(size)
			, typeInfo_(typeinfo)
			, tagsStr_("-")
		{
			if (!tags.IsEmpty())
			{
				tagsStr_ = tags;
			}
			parseTag(tags);
		}

		Reflect_base::Reflect_base(size_t size, const std::type_info& typeinfo, const char* tags)
			: size_(size)
			, typeInfo_(typeinfo)
			, tagsStr_("-")
		{
			if (tags != nullptr)
			{
				tagsStr_ = tags;
			}
			parseTag(tags);
		}

		Reflect_base::~Reflect_base(){}

		Reflect_base& Reflect_base::Swap(Reflect_base& r)
		{
			std::swap(size_, r.size_);
			//std::swap(typeinfo_, r.typeinfo_);
			std::swap(tags_, r.tags_);

			return *this;
		}

		daxia::string Reflect_base::Tag(const daxia::string& prefix) const
		{
			daxia::string tag;

			auto iter = tags_.find(prefix);
			if (iter != tags_.end())
			{
				tag = iter->second;
			}

			return tag;
		};

		void Reflect_base::parseTag(const daxia::string& str)
		{
			std::vector<daxia::string> tags;
			str.Split(" ", tags);
			for (const daxia::string& tag : tags)
			{
				size_t pos = 0;
				daxia::string prefix = tag.Tokenize(":", pos);
				daxia::string suffix = tag.Mid(pos, -1);
				tags_[prefix] = suffix;
			}
		}
	}// namespace reflect
}// namespace daxia

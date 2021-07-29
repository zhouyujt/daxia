#include <map>
#include <mutex>
#include <regex>
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
				size_t pos = 0;
				tag = iter->second.Tokenize(" ",pos);
			}

			return tag;
		};

		daxia::string Reflect_base::TagAttribute(const daxia::string& prefix) const
		{
			daxia::string tag;
			daxia::string attribute;

			auto iter = tags_.find(prefix);
			if (iter != tags_.end())
			{
				size_t pos = 0;
				tag = iter->second.Tokenize(" ", pos);
				attribute = iter->second.Mid(pos, -1);
			}

			return attribute;
		}

		void Reflect_base::parseTag(const daxia::string& str)
		{
			daxia::string tagStr(str);
			tagStr.Trim();

			// ’˝‘Ú∆•≈‰‘ –Ì"orm:id(identity) json:id other:id(attribute1 attribute2=param attribute3)"
			std::string pattern = "\\w+:\\w+(\\s*\\(([\\w=]\\s*)*\\))?";
			std::regex express(pattern);

			std::string& s = static_cast<std::string>(tagStr);
			std::regex_token_iterator<std::string::const_iterator> tokenize(s.begin(), s.end(), express);
			for (auto iter = tokenize; iter != std::sregex_token_iterator(); iter++)
			{
				daxia::string tag = iter->str();

				size_t pos = 0;
				daxia::string prefix = tag.Tokenize(":", pos).Trim();
				daxia::string suffix = tag.Mid(pos, -1).Trim();

				pos = 0;
				daxia::string name = suffix.Tokenize("(", pos).Trim();
				daxia::string attribute = suffix.Mid(pos, -1).Trim();

				if (!attribute.IsEmpty())
				{
					// »•≥˝attribute ◊Û”“¡Ω≤‡¿®∫≈
					if (attribute[attribute.GetLength() - 1] == ')')
					{
						attribute.Delete(attribute.GetLength() - 1);
					}

					tags_[prefix] = name + " " + attribute;
				}
				else
				{
					tags_[prefix] = name;
				}
			}
		}
	}// namespace reflect
}// namespace daxia

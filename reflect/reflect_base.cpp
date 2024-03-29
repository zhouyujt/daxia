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


		Reflect_base::Reflect_base(const char* tags)
			 : tagsStr_("-")
		{
			if (tags != nullptr)
			{
				tagsStr_ = tags;
				parseTag(tags);
			}
		}

		Reflect_base::~Reflect_base(){}

		Reflect_base& Reflect_base::Swap(Reflect_base& r)
		{
			std::swap(tagsStr_, r.tagsStr_);
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

		std::map<daxia::string, daxia::string> Reflect_base::TagAttribute(const daxia::string& prefix) const
		{
			daxia::string tag;
			daxia::string attributeString;

			auto iter = tags_.find(prefix);
			if (iter != tags_.end())
			{
				size_t pos = 0;
				tag = iter->second.Tokenize(" ", pos);
				attributeString = iter->second.Mid(pos, -1);
			}
			
			std::vector<daxia::string> attributes;
			attributeString.Split(" ", attributes);
			std::map<daxia::string, daxia::string> result;
			for (size_t i = 0; i < attributes.size(); ++i)
			{
				size_t pos = 0;
				daxia::string key = attributes[i].Tokenize("=", pos);
				daxia::string value = attributes[i].Mid(pos, -1);
				result[key] = value;
			}

			return result;
		}

		void Reflect_base::parseTag(const daxia::string& str)
		{
			daxia::wstring tagStr = str.ToUnicode();
			tagStr.Trim();

			// ����ƥ������"orm:id(identity) json:id other:id(attribute1 attribute2=param attribute3)"
			static const std::wstring pattern = L"\\w+:[\\w-]+(\\s*\\(([\\w=]\\s*)*\\))?";
			static const std::wregex express(pattern);

			const std::wstring& s = static_cast<const std::wstring&>(tagStr);
			std::regex_token_iterator<std::wstring::const_iterator> tokenize(s.begin(), s.end(), express);
			for (auto iter = tokenize; iter != std::wsregex_token_iterator(); iter++)
			{
				daxia::wstring tag = iter->str();

				size_t pos = 0;
				daxia::wstring prefix = tag.Tokenize(L":", pos).Trim();
				daxia::wstring suffix = tag.Mid(pos, -1).Trim();

				pos = 0;
				daxia::wstring name = suffix.Tokenize(L"(", pos).Trim();
				daxia::wstring attribute = suffix.Mid(pos, -1).Trim();

				if (!attribute.IsEmpty())
				{
					// ȥ��attribute ������������
					if (attribute[attribute.GetLength() - 1] == L')')
					{
						attribute.Delete(attribute.GetLength() - 1);
					}

					tags_[prefix.ToAnsi()] = (name + L" " + attribute).ToAnsi();
				}
				else
				{
					tags_[prefix.ToAnsi()] = name.ToAnsi();
				}
			}
		}
	}// namespace reflect
}// namespace daxia

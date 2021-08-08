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
		{
		}

		Reflect_base::~Reflect_base(){}

		daxia::string Reflect_base::tag(const daxia::string& prefix, const std::map<daxia::string, daxia::string>& tags)
		{
			daxia::string result;

			auto iter = tags.find(prefix);
			if (iter != tags.end())
			{
				size_t pos = 0;
				result = iter->second.Tokenize(" ", pos);
			}

			return result;
		}

		std::map<daxia::string, daxia::string> Reflect_base::tagAttribute(const daxia::string& prefix, const std::map<daxia::string, daxia::string>& tags)
		{
			daxia::string tag;
			daxia::string attributeString;

			auto iter = tags.find(prefix);
			if (iter != tags.end())
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

		std::map<daxia::string, daxia::string> Reflect_base::parseTag(const daxia::string& str)
		{
			std::map<daxia::string, daxia::string> result;

			daxia::wstring tagStr = str.ToUnicode();
			tagStr.Trim();

			// ’˝‘Ú∆•≈‰‘ –Ì"orm:id(identity) json:id other:id(attribute1 attribute2=param attribute3)"
			std::wstring pattern = L"\\w+:\\w+(\\s*\\(([\\w=]\\s*)*\\))?";
			std::wregex express(pattern);

			const std::wstring& s = static_cast<std::wstring>(tagStr);
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
					// »•≥˝attribute ◊Û”“¡Ω≤‡¿®∫≈
					if (attribute[attribute.GetLength() - 1] == L')')
					{
						attribute.Delete(attribute.GetLength() - 1);
					}

					result[prefix.ToAnsi()] = (name + L" " + attribute).ToAnsi();
				}
				else
				{
					result[prefix.ToAnsi()] = name.ToAnsi();
				}
			}

			return result;
		}
	}// namespace reflect
}// namespace daxia

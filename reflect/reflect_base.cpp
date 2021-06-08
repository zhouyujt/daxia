#include <string>
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
		}

		Reflect_helper::~Reflect_helper()
		{

		}

		Reflect_base::Reflect_base(size_t size, const type_info& typeinfo)
			: size_(size)
			, typeInfo_(typeinfo)
			, tagsStr_("-")
		{
		}

		Reflect_base::Reflect_base(size_t size, const type_info& typeinfo, const std::string& tags)
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

		Reflect_base::~Reflect_base(){}

		Reflect_base& Reflect_base::Swap(Reflect_base& r)
		{
			std::swap(size_, r.size_);
			//std::swap(typeinfo_, r.typeinfo_);
			std::swap(tags_, r.tags_);

			return *this;
		}

		std::string Reflect_base::Tag(const std::string& prefix) const
		{
			std::string tag;

			auto iter = tags_.find(prefix);
			if (iter != tags_.end())
			{
				tag = iter->second;
			}

			return tag;
		};

		void Reflect_base::parseTag(const std::string& str)
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
	}// namespace reflect
}// namespace daxia

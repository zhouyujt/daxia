#define BOOST_SPIRIT_THREADSAFE

#include <string>
#include <memory>
#include "json.h"

#define JSON "json"

namespace daxia
{
	namespace encode
	{
		// 使用内存布局缓存进行编码
		void Json::marshal(const char* baseaddr, const daxia::reflect::Layout& layout, boost::property_tree::ptree& root)
		{
			using namespace std;
			using namespace boost::property_tree;
			using namespace daxia::reflect;

			std::shared_ptr<boost::property_tree::ptree> arrayRoot;
			if (layout.ElementCount() > 0)
			{
				arrayRoot = std::shared_ptr<boost::property_tree::ptree>(new boost::property_tree::ptree);
			}

			for (size_t elementIndex = 0; arrayRoot ? elementIndex < layout.ElementCount() : elementIndex < 1; ++elementIndex)
			{
				for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
				{
					if (iter->hashcode == 0) continue;

					const Reflect_base* reflectBase = nullptr;
					try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(baseaddr + elementIndex * layout.ElementSize() + iter->offset)); }
					catch (const std::exception&){}
					if (reflectBase == nullptr) continue;

					daxia::string tag = reflectBase->Tag(JSON);
					if (tag.IsEmpty()) continue;

					const Layout& layout = reflectBase->GetLayout();
					if (layout.Type() == Layout::value)
					{
						putValue(reflectBase, tag, arrayRoot ? *arrayRoot : root);
					}
					else if (layout.Type() == Layout::object)
					{
						putObject(reflectBase, tag, layout, arrayRoot ? *arrayRoot : root);
					}
					else if (layout.Type() == Layout::vecotr)
					{
						if (layout.Fields().empty())
							// value
						{
							putValueElement(reflectBase, tag, arrayRoot ? *arrayRoot : root);
						}
						else
							// object
						{
							putObjectElement(reflectBase, tag, arrayRoot ? *arrayRoot : root);
						}
					}
				}

				// 设置数组辅助信息
				if (arrayRoot)
				{
					// 保存已经解析完毕的元素
					root.push_back(std::make_pair("", *arrayRoot));
					// 清空元素
					arrayRoot->clear();
				}
			}
		}

		void Json::putValue(const daxia::reflect::Reflect_base* reflectBase, const daxia::string& tag, boost::property_tree::ptree &root)
		{
			root.put(static_cast<std::string>(tag), static_cast<std::string>(reflectBase->ToString()));
		}

		void Json::putObject(const daxia::reflect::Reflect_base* reflectBase, const daxia::string& tag, const daxia::reflect::Layout& layout, boost::property_tree::ptree& root)
		{
			boost::property_tree::ptree child;
			marshal(reinterpret_cast<const char*>(reflectBase->ValueAddr()), layout, child);

			root.put_child(static_cast<std::string>(tag), child);
		}

		void Json::putValueElement(const daxia::reflect::Reflect_base* reflectBase, const daxia::string& tag, boost::property_tree::ptree& root)
		{
			using daxia::reflect::Reflect;
			typedef char unknow;
			const Reflect<std::vector<unknow>>* array = reinterpret_cast<const Reflect<std::vector<unknow>>*>(reflectBase);

			boost::property_tree::ptree child;
			if (array->Value().empty())
			{
				boost::property_tree::ptree tr;
				child.push_back(make_pair("", tr));
			}
			else
			{
				int index = 0;
				const unknow* begin = reinterpret_cast<const char*>(&(*(array->Value().begin())));
				const unknow* end = begin + array->Value().size();
				for (const unknow* iter = begin;
					iter != end;
					iter += reflectBase->GetLayout().ElementSize(), ++index)
				{
					boost::property_tree::ptree tr;

					tr.put_value(static_cast<std::string>(reflectBase->ToStringOfElement(index)));

					child.push_back(make_pair("", tr));
				}
			}

			root.put_child(static_cast<std::string>(tag), child);
		}

		void Json::putObjectElement(const daxia::reflect::Reflect_base* reflectBase, const std::string& tag, boost::property_tree::ptree& root)
		{
			typedef char unknow;
			using daxia::reflect::Reflect;

			const Reflect<std::vector<unknow>>* array = reinterpret_cast<const Reflect<std::vector<unknow>>*>(reflectBase);

			// 获取数组布局
			daxia::reflect::Layout layout = reflectBase->GetLayout();

			// 计算元素个数
			if (layout.ElementSize())
			{
				typedef char unknow;
				using daxia::reflect::Reflect;
				const Reflect<std::vector<unknow>>* array = reinterpret_cast<const Reflect<std::vector<unknow>>*>(reflectBase);
				layout.ElementCount() = array->Value().size() / layout.ElementSize();
			}

			boost::property_tree::ptree child;
			if (!array->Value().empty())
			{
				const char* baseaddr = reinterpret_cast<const char*>(&(*(array->Value().begin())));
				if (baseaddr != nullptr)
				{
					marshal(baseaddr, layout, child);
				}
			}
			else
			{
				child.push_back(std::make_pair("", boost::property_tree::ptree()));
			}

			root.put_child(tag, child);
		}

		// 使用内存布局缓存进行解码
		void Json::ummarshal(char* baseaddr, const daxia::reflect::Layout& layout, const boost::property_tree::ptree& root, bool utf8)
		{
			using namespace daxia::reflect;

			bool isVector = layout.ElementCount() > 0;

			boost::property_tree::ptree::const_iterator rootIter = root.begin();
			for (size_t elementIndex = 0; isVector ? elementIndex < layout.ElementCount() : elementIndex < 1; ++elementIndex)
			{
				if (elementIndex) ++rootIter;
				for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
				{
					if (iter->hashcode == 0) continue;

					Reflect_base* reflectBase = nullptr;
					try{ reflectBase = dynamic_cast<Reflect_base*>(reinterpret_cast<Reflect_helper*>(baseaddr + elementIndex * layout.ElementSize() + iter->offset)); }
					catch (const std::exception&){}

					if (reflectBase == nullptr) continue;

					daxia::string tag = reflectBase->Tag(JSON);
					if (tag.IsEmpty()) continue;

					const Layout& layout = reflectBase->GetLayout();
					if (layout.Type() == Layout::value)
					{
						if (!isVector)
						{
							getValue(reflectBase, tag, root, utf8);
						}
						else
						{
							if (!root.empty())
							{
								daxia::string str(rootIter->second.get<std::string>(static_cast<std::string>(tag)));
								str.Utf8() = true;
								if (!utf8) str = str.ToAnsi();
								reflectBase->FromString(str);
							}
						}
					}
					else if (layout.Type() == Layout::object)
					{
						getObject(reflectBase, tag, isVector ? rootIter->second : root, utf8);
					}
					else if (layout.Type() == Layout::vecotr)
					{
						if (layout.Fields().empty())
							// value
						{
							getValueElement(reflectBase, isVector ? rootIter->second.get_child(static_cast<std::string>(tag)) : root.get_child(static_cast<std::string>(tag)), utf8);
						}
						else
							// object
						{
							getObjectElement(reflectBase, isVector ? rootIter->second.get_child(static_cast<std::string>(tag)) : root.get_child(static_cast<std::string>(tag)), utf8);
						}
					}
				}
			}
		}

		void Json::getValue(daxia::reflect::Reflect_base* reflectBase, daxia::string tag, const boost::property_tree::ptree &root, bool utf8)
		{
			daxia::string str(root.get<std::string>(static_cast<std::string>(tag)));
			str.Utf8() = true;
			if (!utf8) str = str.ToAnsi();
			reflectBase->FromString(str);
		}

		void Json::getObject(daxia::reflect::Reflect_base* reflectBase, daxia::string tag, const boost::property_tree::ptree &root, bool utf8)
		{
			ummarshal(reinterpret_cast<char*>(const_cast<void*>(reflectBase->ValueAddr())), reflectBase->GetLayout(), root.get_child(static_cast<std::string>(tag)), utf8);
		}

		void Json::getValueElement(daxia::reflect::Reflect_base* reflectBase, const boost::property_tree::ptree& root, bool utf8)
		{
			using namespace daxia::reflect;
			typedef char unknow;
			Reflect<std::vector<unknow>>* array = reinterpret_cast<Reflect<std::vector<unknow>>*>(reflectBase);

			for (auto iter = root.begin(); iter != root.end(); ++iter)
			{
				daxia::string str(iter->second.data());
				str.Utf8() = true;
				if (!utf8) str = str.ToAnsi();
				reflectBase->FromStringOfElement(str);
			}
		}

		void Json::getObjectElement(daxia::reflect::Reflect_base* reflectBase, const boost::property_tree::ptree& root, bool utf8)
		{
			if (root.empty()) return;

			typedef char unknow;
			using namespace daxia::reflect;

			Reflect<std::vector<unknow>>* array = reinterpret_cast<Reflect<std::vector<unknow>>*>(reflectBase);
			reflectBase->ResizeArray(root.size());

			// 获取数组布局
			Layout layout = reflectBase->GetLayout();
			layout.ElementCount() = root.size();

			ummarshal(reinterpret_cast<char*>(&(*(array->Value().begin()))), layout, root, utf8);
		}
	}// namespace encode
}// namespace daxia

#undef JSON

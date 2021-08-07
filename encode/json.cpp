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

					auto layout = reflectBase->GetLayout();
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
			makeElementCount(reflectBase, layout);

			boost::property_tree::ptree child;
			if (!array->Value().empty())
			{
				const char* baseaddr = reinterpret_cast<const char*>(&(*(array->Value().begin())));
				if (baseaddr != nullptr)
				{
					marshal(baseaddr, layout, child);
				}
			}

			root.put_child(tag, child);
		}

		// 使用内存布局缓存进行解码
		void Json::ummarshal(char* baseaddr,
			const daxia::reflect::Layout& layout,
			const boost::property_tree::ptree& root,
			ArrayInfo* parentArray)
		{
			using namespace daxia::reflect;

			for (auto iter = layout.Fields().begin(); iter != layout.Fields().end(); ++iter)
			{
				if (iter->hashcode == 0) continue;

				Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<Reflect_base*>(reinterpret_cast<Reflect_helper*>(baseaddr + iter->offset)); }
				catch (const std::exception&){}

				if (reflectBase == nullptr) continue;

				if (tryGetValue<bool>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<char>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<unsigned char>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<short>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<unsigned short>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<int>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<unsigned int>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<long>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<unsigned long>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<long long>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<unsigned long long>(reflectBase, root, parentArray)) continue;
				if (tryGetValue<std::string>(reflectBase, root, parentArray)) continue;

				// object or array
				std::string tag = reflectBase->Tag(JSON);
				if (!tag.empty())
				{
					if (layout.Type() == Layout::vecotr)	// array
					{
						const boost::property_tree::ptree* ptree = nullptr;
						if (parentArray == nullptr)
						{
							ptree = &root.get_child(tag);
						}
						else
						{
							auto elemetIter = parentArray->ptree.begin();
							ptree = &elemetIter->second.get_child(tag);
						}

						const boost::property_tree::ptree& child = *ptree;
						if (reflectBase->GetLayout().Fields().empty())
						{
							// array's element
							if (tryGetElement<bool>(reflectBase, child)) continue;
							if (tryGetElement<char>(reflectBase, child)) continue;
							if (tryGetElement<unsigned char>(reflectBase, child)) continue;
							if (tryGetElement<short>(reflectBase, child)) continue;
							if (tryGetElement<unsigned short>(reflectBase, child)) continue;
							if (tryGetElement<int>(reflectBase, child)) continue;
							if (tryGetElement<unsigned int>(reflectBase, child)) continue;
							if (tryGetElement<long>(reflectBase, child)) continue;
							if (tryGetElement<unsigned long>(reflectBase, child)) continue;
							if (tryGetElement<long long>(reflectBase, child)) continue;
							if (tryGetElement<unsigned long long>(reflectBase, child)) continue;
							if (tryGetElement<std::string>(reflectBase, child)) continue;
						}
						else
						{
							getArray(reflectBase, child);
						}
					}
					else // object
					{
						if (parentArray == nullptr)
						{
							ummarshal(reinterpret_cast<char*>(const_cast<void*>(reflectBase->ValueAddr())), reflectBase->GetLayout(), root.get_child(tag), nullptr);
						}
						else
						{
							auto elemetIter = parentArray->ptree.begin();
							ummarshal(reinterpret_cast<char*>(const_cast<void*>(reflectBase->ValueAddr())), reflectBase->GetLayout(), elemetIter->second.get_child(tag), nullptr);
						}
					}
				}

			}
		}

		void Json::wptree2ptree(const boost::property_tree::wptree& wptree, boost::property_tree::ptree& ptree)
		{
			for (auto iter = wptree.begin(); iter != wptree.end(); ++iter)
			{
				std::string first = Strconv::Unicode2Ansi(iter->first.c_str());
				if (iter->second.empty()) // value
				{
					const std::wstring& val = iter->second.get_value<std::wstring>();
					std::string val2 = Strconv::Unicode2Ansi(val.c_str());
					if (first.empty())
					{
						boost::property_tree::ptree child;
						child.put_value(val2);
						ptree.push_back(make_pair("", child));
					}
					else
					{
						ptree.put(first, val2);
					}
				}
				else // object or array
				{
					boost::property_tree::ptree child;
					wptree2ptree(iter->second, child);
					if (!first.empty())
					{
						ptree.put_child(first, child);
					}
					else
					{
						ptree.push_back(make_pair("", child));
					}
				}
			}
		}

		void Json::getArray(daxia::reflect::Reflect_base* reflectBase,
			const boost::property_tree::ptree& root)
		{
			if (root.empty()) return;

			typedef char unknow;
			using namespace daxia::reflect;

			Reflect<std::vector<unknow>>* array = reinterpret_cast<Reflect<std::vector<unknow>>*>(reflectBase);
			//reflectBase->ResizeArray(root.size());

			// 获取数组布局
			Layout layout = reflectBase->GetLayout();
			makeElementCount(reflectBase, layout);

			ArrayInfo ai;
			ai.ptree = root;

			ummarshal(reinterpret_cast<char*>(&(*(array->Value().begin()))), layout, root, &ai);
		}

		void Json::makeElementCount(const daxia::reflect::Reflect_base* reflectBase, daxia::reflect::Layout& layout)
		{
			typedef char unknow;
			using daxia::reflect::Reflect;

			// 计算元素个数
			if (layout.ElementSize())
			{
				const Reflect<std::vector<unknow>>* array = reinterpret_cast<const Reflect<std::vector<unknow>>*>(reflectBase);
				layout.ElementCount() = array->Value().size() / layout.ElementSize();
			}
		}
	}// namespace encode
}// namespace daxia

#undef JSON

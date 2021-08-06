#define BOOST_SPIRIT_THREADSAFE

#include <string>
#include <functional>
#include "json.h"

#define JSON "json"

namespace daxia
{
	namespace encode
	{
		// 使用内存布局缓存进行编码
		void Json::marshal(const char* baseaddr,
			const boost::property_tree::ptree& layout,
			boost::property_tree::ptree& root,
			ArrayInfo* parentArray)
		{
			using namespace std;
			using namespace boost::property_tree;
			using daxia::reflect::Reflect_helper;
			using daxia::reflect::Reflect_base;
			using daxia::reflect::Reflect;

			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				size_t hashcode = iter->second.get<size_t>(REFLECT_LAYOUT_FIELD_HASH, 0);
				size_t offset = iter->second.get<size_t>(REFLECT_LAYOUT_FIELD_OFFSET, 0);

				if (hashcode == 0) continue;

				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(baseaddr + offset)); }
				catch (const std::exception&){}
				if (reflectBase == nullptr) continue;

				daxia::string tag = reflectBase->Tag(JSON);
				if (tag.IsEmpty()) continue;

				auto layout = reflectBase->Layout();
				if (!reflectBase->IsArray())
				{
					if (layout.empty())
					// value
					{
						// 设置数组辅助信息
						if (parentArray)
						{
							if (parentArray->firstTag.IsEmpty())
							{
								// 设置元素第一个字段的tag
								parentArray->firstTag = tag;
							}
							else if (parentArray->firstTag == tag)
							{
								// 保存已经解析完毕的元素
								root.push_back(std::make_pair("", parentArray->ptree));

								// 清空元素
								parentArray->ptree.clear();
							}
						}

						boost::property_tree::ptree& ptree = parentArray ? parentArray->ptree : root;
						ptree.put(static_cast<std::string>(tag), static_cast<std::string>(reflectBase->ToString()));

					}
					else
					// object
					{
						ptree child;
						marshal(reinterpret_cast<const char*>(reflectBase->ValueAddr()), layout, child, nullptr);

						boost::property_tree::ptree& ptree = parentArray ? parentArray->ptree : root;
						ptree.put_child(static_cast<std::string>(tag), child);
					}
				}
				else
				{
					if (layout.empty())
					// value
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
								iter += reflectBase->SizeOfElement(), ++index)
							{
								boost::property_tree::ptree tr;

								tr.put_value(static_cast<std::string>(reflectBase->ToStringOfElement(index)));

								child.push_back(make_pair("", tr));
							}
						}

						boost::property_tree::ptree& ptree = parentArray ? parentArray->ptree : root;
						ptree.put_child(static_cast<std::string>(tag), child);
					}
					else
					// object
					{
						putArray(reflectBase, tag, root, parentArray);
					}
				}
			}
		}

		// 使用内存布局缓存进行解码
		void Json::ummarshal(char* baseaddr,
			const boost::property_tree::ptree& layout,
			const boost::property_tree::ptree& root,
			ArrayInfo* parentArray)
		{
			//std::stringstream ss1;
			//std::string s1;
			//write_json(ss1, layout, true);
			//s1 = ss1.str();
			//std::cout << s1;

			using daxia::reflect::Reflect_helper;
			using daxia::reflect::Reflect_base;

			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				//std::stringstream ss2;
				//std::string s2;
				//write_json(ss2, root, true);
				//s2 = ss2.str();
				//std::cout << s2;

				size_t hashcode = iter->second.get<size_t>(REFLECT_LAYOUT_FIELD_HASH, 0);
				size_t offset = iter->second.get<size_t>(REFLECT_LAYOUT_FIELD_OFFSET, 0);

				if (hashcode == 0) continue;

				Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<Reflect_base*>(reinterpret_cast<Reflect_helper*>(baseaddr + offset)); }
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
					if (reflectBase->IsArray())	// array
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
						if (reflectBase->Layout().empty())
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
							ummarshal(reinterpret_cast<char*>(const_cast<void*>(reflectBase->ValueAddr())), reflectBase->Layout(), root.get_child(tag), nullptr);
						}
						else
						{
							auto elemetIter = parentArray->ptree.begin();
							ummarshal(reinterpret_cast<char*>(const_cast<void*>(reflectBase->ValueAddr())), reflectBase->Layout(), elemetIter->second.get_child(tag), nullptr);
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

		void Json::putArray(const daxia::reflect::Reflect_base* reflectBase,
			const std::string& tag,
			boost::property_tree::ptree& root,
			ArrayInfo* parentArray)
		{
			typedef char unknow;
			using daxia::reflect::Reflect;

			const Reflect<std::vector<unknow>>* array = reinterpret_cast<const Reflect<std::vector<unknow>>*>(reflectBase);

			// 获取数组布局
			boost::property_tree::ptree layout = reflectBase->Layout();
			extendArrayLayout(reflectBase, layout);

			ArrayInfo ai;
			boost::property_tree::ptree child;
			if (!array->Value().empty())
			{
				const char* baseaddr = reinterpret_cast<const char*>(&(*(array->Value().begin())));
				if (baseaddr != nullptr)
				{
					marshal(baseaddr, layout, child, &ai);
				}
			}

			// 保存解析完毕的元素
			child.push_back(std::make_pair("", ai.ptree));

			boost::property_tree::ptree& ptree = parentArray ? parentArray->ptree : root;

			ptree.put_child(tag, child);
		}

		void Json::getArray(daxia::reflect::Reflect_base* reflectBase,
			const boost::property_tree::ptree& root)
		{
			if (root.empty()) return;

			typedef char unknow;
			using daxia::reflect::Reflect;

			Reflect<std::vector<unknow>>* array = reinterpret_cast<Reflect<std::vector<unknow>>*>(reflectBase);
			reflectBase->ResizeArray(root.size());

			// 获取数组布局
			boost::property_tree::ptree layout = reflectBase->Layout();
			extendArrayLayout(reflectBase, layout);

			ArrayInfo ai;
			ai.ptree = root;

			ummarshal(reinterpret_cast<char*>(&(*(array->Value().begin()))), layout, root, &ai);
		}

		void Json::extendArrayLayout(const daxia::reflect::Reflect_base* reflectBase, boost::property_tree::ptree& layout)
		{
			typedef char unknow;
			using daxia::reflect::Reflect;

			// 计算元素个数
			size_t size = layout.get<size_t>(REFLECT_LAYOUT_FIELD_SIZE);
			const Reflect<std::vector<unknow>>* array = reinterpret_cast<const Reflect<std::vector<unknow>>*>(reflectBase);
			size_t count = array->Value().size() / size;

			// 为所有元素扩展布局
			layout.erase(REFLECT_LAYOUT_FIELD_SIZE);
			auto attribleCount = layout.size();
			for (size_t i = 1; i < count; ++i)
			{
				auto iter = layout.begin();
				for (size_t j = 0; j < attribleCount; ++j, ++iter)
				{
					boost::property_tree::ptree child;

					size_t hashcode = iter->second.get<size_t>(REFLECT_LAYOUT_FIELD_HASH, 0);
					size_t offset = iter->second.get<size_t>(REFLECT_LAYOUT_FIELD_OFFSET, 0) + size * i;

					child.put(REFLECT_LAYOUT_FIELD_HASH, hashcode);
					child.put(REFLECT_LAYOUT_FIELD_OFFSET, offset);

					layout.add_child(iter->first, child);
				}
			}
		}

	}// namespace encode
}// namespace daxia

#undef JSON

#define BOOST_SPIRIT_THREADSAFE

#include <string>
#include <functional>
#include "json.h"

#define JSON "json"
#define HASH "hash"
#define OFFSET "offset"
#define SIZE "size"

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
			//std::stringstream ss1;
			//std::string s1;
			//write_json(ss1, layout, true);
			//s1 = ss1.str();
			//std::cout << s1;

			using namespace std;
			using namespace boost::property_tree;
			using daxia::reflect::Reflect_helper;
			using daxia::reflect::Reflect_base;
			using daxia::reflect::Reflect;

			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				size_t hashcode = iter->second.get<size_t>(HASH, 0);
				unsigned long offset = iter->second.get<unsigned long>(OFFSET, 0);

				if (hashcode == 0) continue;

				const Reflect_base* reflectBase = nullptr;
				try{ reflectBase = dynamic_cast<const Reflect_base*>(reinterpret_cast<const Reflect_helper*>(baseaddr + offset)); }
				catch (const std::exception&){}

				if (reflectBase == nullptr) continue;

				// value
				if (tryPutValue<bool>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<char>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<unsigned char>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<short>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<unsigned short>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<int>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<unsigned int>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<long>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<unsigned long>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<long long>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<unsigned long long>(baseaddr, reflectBase, root, parentArray)) continue;
				if (tryPutValue<string>(baseaddr, reflectBase, root, parentArray)) continue;

				// object or array
				string tag = reflectBase->Tag(JSON);
				if (!tag.empty())
				{
					if (reflectBase->IsArray())	// array
					{
						if (reflectBase->Layout().empty())
						{
							// array's element
							if (tryPutElement<bool>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<char>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<unsigned char>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<short>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<unsigned short>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<int>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<unsigned int>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<long>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<unsigned long>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<long long>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<unsigned long long>(baseaddr, reflectBase, tag, root, parentArray)) continue;
							if (tryPutElement<string>(baseaddr, reflectBase, tag, root, parentArray)) continue;
						}
						else
						{
							putArray(reflectBase, tag, root, parentArray);
						}
					}
					else // object
					{
						ptree child;
						marshal(reinterpret_cast<const char*>(reflectBase->ValueAddr()), reflectBase->Layout(), child, nullptr);
						root.put_child(tag, child);
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

			using namespace std;
			using daxia::reflect::Reflect_helper;
			using daxia::reflect::Reflect_base;

			for (auto iter = layout.begin(); iter != layout.end(); ++iter)
			{
				//std::stringstream ss2;
				//std::string s2;
				//write_json(ss2, root, true);
				//s2 = ss2.str();
				//std::cout << s2;

				size_t hashcode = iter->second.get<size_t>(HASH, 0);
				unsigned long offset = iter->second.get<unsigned long>(OFFSET, 0);

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
				if (tryGetValue<string>(reflectBase, root, parentArray)) continue;

				// object or array
				string tag = reflectBase->Tag(JSON);
				if (!tag.empty())
				{
					if (reflectBase->IsArray())	// array
					{
						const boost::property_tree::ptree& child = root.get_child(tag);
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
							if (tryGetElement<string>(reflectBase, child)) continue;
						}
						else
						{
							getArray(reflectBase, child);
						}
					}
					else // object
					{
						ummarshal(reinterpret_cast<char*>(const_cast<void*>(reflectBase->ValueAddr())), reflectBase->Layout(), root.get_child(tag), nullptr);
					}
				}

			}
		}

		void Json::wptree2ptree(const boost::property_tree::wptree& wptree, boost::property_tree::ptree& ptree)
		{
			using namespace std;

			for (auto iter = wptree.begin(); iter != wptree.end(); ++iter)
			{
				string first = Strconv::Unicode2Ansi(iter->first.c_str());
				if (iter->second.empty()) // value
				{
					const wstring& val = iter->second.get_value<wstring>();
					string val2 = Strconv::Unicode2Ansi(val.c_str());
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
			const char* baseaddr = (*array)().begin()._Ptr;
			marshal(baseaddr, layout, child, &ai);

			// 保存解析完毕的元素
			child.push_back(std::make_pair("", ai.ptree));

			boost::property_tree::ptree& ptree = parentArray ? parentArray->ptree : root;

			ptree.put_child(tag, child);
		}

		void Json::getArray(daxia::reflect::Reflect_base* reflectBase,
			const boost::property_tree::ptree& root)
		{
			typedef char unknow;
			using daxia::reflect::Reflect;

			Reflect<std::vector<unknow>>* array = reinterpret_cast<Reflect<std::vector<unknow>>*>(reflectBase);
			reflectBase->ResizeArray(root.size());

			// 获取数组布局
			boost::property_tree::ptree layout = reflectBase->Layout();
			extendArrayLayout(reflectBase, layout);

			ArrayInfo ai;
			ai.ptree = root;

			ummarshal((*array)().begin()._Ptr, layout, root, &ai);
		}

		void Json::extendArrayLayout(const daxia::reflect::Reflect_base* reflectBase, boost::property_tree::ptree& layout)
		{
			typedef char unknow;
			using daxia::reflect::Reflect;

			// 计算元素个数
			size_t size = layout.get<size_t>(SIZE);
			const Reflect<std::vector<unknow>>* array = reinterpret_cast<const Reflect<std::vector<unknow>>*>(reflectBase);
			size_t count = (*array)().size() / size;

			// 为所有元素扩展布局
			layout.erase(SIZE);
			auto attribleCount = layout.size();
			for (size_t i = 1; i < count; ++i)
			{
				auto iter = layout.begin();
				for (size_t j = 0; j < attribleCount; ++j, ++iter)
				{
					boost::property_tree::ptree child;

					size_t hashcode = iter->second.get<size_t>(HASH, 0);
					unsigned long offset = iter->second.get<unsigned long>(OFFSET, 0) + size * i;

					child.put(HASH, hashcode);
					child.put(OFFSET, offset);

					layout.add_child(iter->first, child);
				}
			}
		}

	}// namespace encode
}// namespace daxia

#undef JSON
#undef HASH
#undef OFFSET
#undef SIZE

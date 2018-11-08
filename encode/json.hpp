/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file json.hpp
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 * 将对象序列化成json格式字符串以及反序列化
 *
 */

#ifndef __DAXIA_ENCODE_JSON_HPP
#define __DAXIA_ENCODE_JSON_HPP

#define BOOST_SPIRIT_THREADSAFE

#include <string>
#include <stack>
#include <functional>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <daxia/reflect/reflect.hpp>
#include <daxia/encode/strconv.hpp>

#define JSON "json"
#define HASH "hash"
#define OFFSET "offset"
#define SIZE "size"

namespace daxia
{
	namespace encode
	{
		class json
		{
		public:
			json() = delete;
			~json(){}
		public:
			template<class ValueType>
			static std::string Marshal(const ValueType& v, bool pretty = false)
			{
				using namespace std;
				using namespace boost::property_tree;
				using daxia::reflect::reflect;

				// 获取内存布局
				const ptree& layout =  reflect<ValueType>().Layout();

				ptree root;
				stringstream ss;
				const char* baseaddr = reinterpret_cast<const char*>(&v);
				marshal(baseaddr, layout, root, nullptr);
				write_json(ss, root, pretty);

				return ss.str();
			}

			template<class ValueType>
			static bool Unmarshal(const std::wstring& jsonStr, ValueType& v)
			{
				using namespace std;
				using namespace boost::property_tree;
				using daxia::reflect::reflect;

				// 获取内存布局
				const ptree& layout = reflect<ValueType>().Layout();

				try
				{
					wptree root;
					wstringstream ss(jsonStr);
					json_parser::read_json(ss, root);

					// wptree => ptree
					ptree root2;
					wptree2ptree(root, root2);

					//std::stringstream ss1;
					//std::string s;
					//write_json(ss1, root2, true);
					//s = ss1.str();
					//std::cout << s;

					ummarshal(reinterpret_cast<char*>(&v), layout, root2,nullptr);
				}
				catch (const boost::property_tree::ptree_error&)
				{
					return false;
				}
				
				return true;
			}

			template<class ValueType>
			static bool Unmarshal(const std::string& jsonStr, ValueType& v)
			{
				std::wstring str = Ansi2Unicode(jsonStr.c_str());
				return Unmarshal(str,v);
			}

			// 数组信息
			struct ArrayInfo
			{
				std::string firstTag;
				boost::property_tree::ptree ptree;
			};
		private:
			// 使用内存布局缓存进行编码
			static void marshal(const char* baseaddr,
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
				using daxia::reflect::reflect_helper;
				using daxia::reflect::reflect_base;
				using daxia::reflect::reflect;

				for (auto iter = layout.begin(); iter != layout.end(); ++iter)
				{
					size_t hashcode = iter->second.get<size_t>(HASH, 0);
					unsigned long offset = iter->second.get<unsigned long>(OFFSET, 0);

					if (hashcode == 0) continue;

					const reflect_base* reflectBase = nullptr;
					try{ reflectBase = dynamic_cast<const reflect_base*>(reinterpret_cast<const reflect_helper*>(baseaddr + offset)); }
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
			static void ummarshal(char* baseaddr,
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
				using daxia::reflect::reflect_helper;
				using daxia::reflect::reflect_base;

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

					reflect_base* reflectBase = nullptr;
					try{ reflectBase = dynamic_cast<reflect_base*>(reinterpret_cast<reflect_helper*>(baseaddr + offset)); }
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

			static void wptree2ptree(const boost::property_tree::wptree& wptree, boost::property_tree::ptree& ptree)
			{
				using namespace std;

				for (auto iter = wptree.begin(); iter != wptree.end(); ++iter)
				{
					string first = Unicode2Ansi(iter->first.c_str());
					if (iter->second.empty()) // value
					{
						const wstring& val = iter->second.get_value<wstring>();
						string val2 = Unicode2Ansi(val.c_str());
						if (first.empty())
						{
							boost::property_tree::ptree child;
							child.put_value(val2);
							ptree.push_back(make_pair("", child));
						}
						else
						{
							ptree.put(first,val2);
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
							ptree.push_back(make_pair("",child));
						}
					}
				}
			}

			template<class T>
			static bool tryPutValue(const char* baseaddr,
				const daxia::reflect::reflect_base* reflectBase,
				boost::property_tree::ptree& root,
				ArrayInfo* parentArray)
			{
				using daxia::reflect::reflect;

				const reflect_base* impl = nullptr;

				size_t hash = 0;

				try
				{
					impl = dynamic_cast<const reflect<T>*>(reflectBase);
				}
				catch (const std::exception&) {}

				if (impl == nullptr) return false;

				try
				{
					const ::reflect<T>& r = *(dynamic_cast<const ::reflect<T>*>(impl));
					std::string tag = r.Tag(JSON);
					if (!tag.empty())
					{
						// 设置数组辅助信息
						if (parentArray)
						{
							if (parentArray->firstTag.empty())
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

						if (typeid(T) == typeid(std::string))
						{
							std::string str = "\"";
							str += reinterpret_cast<const std::string&>(r());
							str += "\"";
							ptree.put(tag, str);
						}
						else
						{
							ptree.put(tag, r());
						}
					}
				}
				catch (const boost::property_tree::ptree_error&)
				{
					return false;
				}

				return true;
			}

			template<class T>
			static bool tryGetValue(daxia::reflect::reflect_base* reflectBase,
				const boost::property_tree::ptree& root,
				ArrayInfo* parentArray)
			{
				using daxia::reflect::reflect;

				const reflect_base* impl = nullptr;

				size_t hash = 0;

				try
				{
					impl = dynamic_cast<const reflect<T>*>(reflectBase);
				}
				catch (const std::exception&) {}

				if (impl == nullptr) return false;

				try
				{
					string tag = impl->Tag(JSON);
					if (!tag.empty())
					{
						T* p = static_cast<T*>(const_cast<void*>(impl->ValueAddr()));
						if (parentArray)
						{
							if (parentArray->firstTag.empty())
							{
								// 设置元素第一个字段的tag
								parentArray->firstTag = tag;
							}
							else if (parentArray->firstTag == tag)
							{
								// 删除已经保存过的元素
								parentArray->ptree.erase(parentArray->ptree.begin());
							}

							if (!parentArray->ptree.empty())
							{
								*p = parentArray->ptree.begin()->second.get<T>(tag);
							}
						}
						else
						{
							*p = root.get<T>(tag);
						}
					}
				}
				catch (const boost::property_tree::ptree_error&)
				{
					return false;
				}

				return true;
			}

			template<class T>
			static bool tryPutElement(const void* baseaddr,
				const daxia::reflect::reflect_base* reflectBase,
				const std::string& tag,
				boost::property_tree::ptree& root,
				ArrayInfo* parentArray)
			{
				using daxia::reflect::reflect;

				const reflect<std::vector<T>>* array = nullptr;

				try
				{
					array = dynamic_cast<const reflect<std::vector<T>>*>(reflectBase);
				}
				catch (const std::exception&)
				{
					return false;
				}

				if (array == nullptr)
				{
					return false;
				}

				boost::property_tree::ptree child;

				try
				{
					if ((*array)().empty())
					{
						boost::property_tree::ptree tr;
						child.push_back(make_pair("", tr));
					}
					else
					{
						for (auto iter = (*array)().begin(); iter != (*array)().end(); ++iter)
						{
							boost::property_tree::ptree tr;

							if (typeid(T) == typeid(std::string))
							{
								std::string str = "\"";
								const T& v = *iter;
								str += reinterpret_cast<const std::string&>(v);
								str += "\"";
								tr.put_value(str);
							}
							else
							{
								tr.put_value(*iter);
							}

							child.push_back(make_pair("", tr));
						}
					}
				}
				catch (const boost::property_tree::ptree_error&)
				{
					return false;
				}

				boost::property_tree::ptree& ptree = parentArray ? parentArray->ptree : root;
				ptree.put_child(tag, child);

				return true;
			}

			template<class T>
			static bool tryGetElement(daxia::reflect::reflect_base* reflectBase,
				const boost::property_tree::ptree& root)
			{
				using daxia::reflect::reflect;

				reflect<std::vector<T>>* array = nullptr;

				try
				{
					array = dynamic_cast<reflect<std::vector<T>>*>(reflectBase);
				}
				catch (const std::exception&)
				{
					return false;
				}

				if (array == nullptr)
				{
					return false;
				}

				for (auto iter = root.begin(); iter != root.end(); ++iter)
				{
					(*array)().push_back(boost::lexical_cast<T>(iter->second.data()));
				}

				return true;
			}

			static void putArray(const daxia::reflect::reflect_base* reflectBase,
				const std::string& tag,
				boost::property_tree::ptree& root,
				ArrayInfo* parentArray)
			{
				typedef char unknow;
				using daxia::reflect::reflect;

				const reflect<std::vector<unknow>>* array = reinterpret_cast<const reflect<std::vector<unknow>>*>(reflectBase);

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

			static void getArray(daxia::reflect::reflect_base* reflectBase,
				const boost::property_tree::ptree& root)
			{
				typedef char unknow;
				using daxia::reflect::reflect;

				reflect<std::vector<unknow>>* array = reinterpret_cast<reflect<std::vector<unknow>>*>(reflectBase);
				reflectBase->ResizeArray(root.size());

				// 获取数组布局
				boost::property_tree::ptree layout = reflectBase->Layout();
				extendArrayLayout(reflectBase, layout);

				ArrayInfo ai;
				ai.ptree = root;

				ummarshal((*array)().begin()._Ptr, layout, root, &ai);
			}

			static void extendArrayLayout(const daxia::reflect::reflect_base* reflectBase, boost::property_tree::ptree& layout)
			{
				typedef char unknow;
				using daxia::reflect::reflect;

				// 计算元素个数
				size_t size = layout.get<size_t>(SIZE);
				const reflect<std::vector<unknow>>* array = reinterpret_cast<const reflect<std::vector<unknow>>*>(reflectBase);
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

			// 由于boost::property_tree::write_json会将所有数据类型视为字符串类型，所以重写相关方法使之能区分不同类型
		private:
			template<class Ptree>
			static void write_json(std::basic_ostream <
				typename Ptree::key_type::value_type
			> &stream,
			const Ptree &pt,
			bool pretty = true)
			{
				write_json_internal(stream, pt, std::string(), pretty);
			}

			template<class Ptree>
			static void write_json_internal(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
				const Ptree &pt,
				const std::string &filename,
				bool pretty)
			{
				if (!boost::property_tree::json_parser::verify_json(pt, 0))
					BOOST_PROPERTY_TREE_THROW(boost::property_tree::json_parser::json_parser_error("ptree contains data that cannot be represented in JSON format", filename, 0));
				write_json_helper(stream, pt, 0, pretty);
				stream << std::endl;
				if (!stream.good())
					BOOST_PROPERTY_TREE_THROW(boost::property_tree::json_parser::json_parser_error("write error", filename, 0));
			}

			template<class Ptree>
			static void write_json_helper(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
				const Ptree &pt,
				int indent, bool pretty)
			{
				using namespace boost::property_tree::json_parser;

				typedef typename Ptree::key_type::value_type Ch;
				typedef typename std::basic_string<Ch> Str;

				// Value or object or array
				if (indent > 0 && pt.empty())
				{
					// Write value
					Str data = create_escapes(pt.template get_value<Str>());
					//stream << Ch('"') << data << Ch('"');
					stream << data;
				}
				else if (indent > 0 && pt.count(Str()) == pt.size())
				{
					// Write array
					stream << Ch('[');
					if (pretty) stream << Ch('\n');
					typename Ptree::const_iterator it = pt.begin();
					for (; it != pt.end(); ++it)
					{
						if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
						write_json_helper(stream, it->second, indent + 1, pretty);
						if (boost::next(it) != pt.end())
							stream << Ch(',');
						if (pretty) stream << Ch('\n');
					}
					if (pretty) stream << Str(4 * indent, Ch(' '));
					stream << Ch(']');

				}
				else
				{
					// Write object
					stream << Ch('{');
					if (pretty) stream << Ch('\n');
					typename Ptree::const_iterator it = pt.begin();
					for (; it != pt.end(); ++it)
					{
						if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
						stream << Ch('"') << create_escapes(it->first) << Ch('"') << Ch(':');
						if (pretty) stream << Ch(' ');
						write_json_helper(stream, it->second, indent + 1, pretty);
						if (boost::next(it) != pt.end())
							stream << Ch(',');
						if (pretty) stream << Ch('\n');
					}
					if (pretty) stream << Str(4 * indent, Ch(' '));
					stream << Ch('}');
				}

			}

			// Create necessary escape sequences from illegal characters
			template<class Ch>
			static std::basic_string<Ch> create_escapes(const std::basic_string<Ch> &s)
			{
				std::basic_string<Ch> result;
				typename std::basic_string<Ch>::const_iterator b = s.begin();
				typename std::basic_string<Ch>::const_iterator e = s.end();
				while (b != e)
				{
					typedef typename make_unsigned<Ch>::type UCh;
					UCh c(*b);
					// This assumes an ASCII superset. But so does everything in PTree.
					// We escape everything outside ASCII, because this code can't
					// handle high unicode characters.
					if (c == 0x20 || c == 0x21 || (c >= 0x23 && c <= 0x2E) ||
						(c >= 0x30 && c <= 0x5B) || (c >= 0x5D && c <= 0xFF))
						result += *b;
					else if (*b == Ch('\b')) result += Ch('\\'), result += Ch('b');
					else if (*b == Ch('\f')) result += Ch('\\'), result += Ch('f');
					else if (*b == Ch('\n')) result += Ch('\\'), result += Ch('n');
					else if (*b == Ch('\r')) result += Ch('\\'), result += Ch('r');
					else if (*b == Ch('\t')) result += Ch('\\'), result += Ch('t');
					else if (*b == Ch('/')) result += Ch('\\'), result += Ch('/');
					//else if (*b == Ch('"'))  result += Ch('\\'), result += Ch('"');
					else if (*b == Ch('"'))  result += Ch('"');
					else if (*b == Ch('\\')) result += Ch('\\'), result += Ch('\\');
					else
					{
						const char *hexdigits = "0123456789ABCDEF";
						unsigned long u = (std::min)(static_cast<unsigned long>(
							static_cast<UCh>(*b)),
							0xFFFFul);
						unsigned long d1 = u / 4096; u -= d1 * 4096;
						unsigned long d2 = u / 256; u -= d2 * 256;
						unsigned long d3 = u / 16; u -= d3 * 16;
						unsigned long d4 = u;
						result += Ch('\\'); result += Ch('u');
						result += Ch(hexdigits[d1]); result += Ch(hexdigits[d2]);
						result += Ch(hexdigits[d3]); result += Ch(hexdigits[d4]);
					}
					++b;
				}
				return result;
			}
		};// class json
	}// namespace encode
}// namespace daxia

#undef JSON
#undef HASH
#undef OFFSET
#undef SIZE

#endif // !__DAXIA_ENCODE_JSON_HPP

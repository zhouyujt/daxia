/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file json.h
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 * 将对象序列化成json格式字符串以及反序列化
 *
 */

#ifndef __DAXIA_ENCODE_JSON_H
#define __DAXIA_ENCODE_JSON_H

#define BOOST_SPIRIT_THREADSAFE

#include <string>
#include <functional>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include "../reflect/reflect.hpp"
#include "../encode/strconv.h"
#include "../string.hpp"

#define JSON "json"
#define HASH "hash"
#define OFFSET "offset"
#define SIZE "size"

namespace daxia
{
	namespace encode
	{
		class Json
		{
		public:
			Json() = delete;
			~Json(){}
		public:
			template<class ValueType>
			static daxia::string Marshal(const ValueType& v, bool pretty = false)
			{
				using namespace std;
				using namespace boost::property_tree;
				using daxia::reflect::Reflect;

				// 获取内存布局
				const ptree& layout = Reflect<ValueType>().Layout();

				ptree root;
				stringstream ss;
				const char* baseaddr = reinterpret_cast<const char*>(&v);
				marshal(baseaddr, layout, root, nullptr);
				write_json(ss, root, pretty);

				return ss.str();
			}

			template<class ValueType>
			static bool Unmarshal(const daxia::wstring& jsonStr, ValueType& v)
			{
				using namespace std;
				using namespace boost::property_tree;
				using daxia::reflect::Reflect;

				// 获取内存布局
				const ptree& layout = Reflect<ValueType>().Layout();

				try
				{
					wptree root;
					wstringstream ss(jsonStr.GetString());
					json_parser::read_json(ss, root);

					// wptree => ptree
					ptree root2;
					wptree2ptree(root, root2);

					//std::stringstream ss1;
					//std::string s;
					//write_json(ss1, root2, true);
					//s = ss1.str();
					//std::cout << s;

					ummarshal(reinterpret_cast<char*>(&v), layout, root2, nullptr);
				}
				catch (const boost::property_tree::ptree_error&)
				{
					return false;
				}

				return true;
			}

			template<class ValueType>
			static bool Unmarshal(const daxia::string& jsonStr, ValueType& v)
			{
				return Unmarshal(jsonStr.ToUnicode(), v);
			}

			// 数组信息
			struct ArrayInfo
			{
				std::string firstTag;	// 数组元素第一个变量名称，用于判断一个元素填充完毕
				boost::property_tree::ptree ptree;
			};
		private:
			template<class T>
			static bool tryPutValue(const char* baseaddr,
				const daxia::reflect::Reflect_base* reflectBase,
				boost::property_tree::ptree& root,
				ArrayInfo* parentArray)
			{
				using daxia::reflect::Reflect_base;
				using daxia::reflect::Reflect;

				const Reflect_base* impl = nullptr;

				size_t hash = 0;

				try
				{
					impl = dynamic_cast<const Reflect<T>*>(reflectBase);
				}
				catch (const std::exception&) {}

				if (impl == nullptr) return false;

				try
				{
					const Reflect<T>& r = *(dynamic_cast<const Reflect<T>*>(impl));
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
							daxia::string temp(reinterpret_cast<const std::string&>(r.Value()));
							temp.Replace("\\", "\\\\");
							temp.Replace("\"", "\\\"");

							std::string str = "\"";
							str += temp.GetString();
							str += "\"";
							ptree.put(tag, str);
						}
						else
						{
							ptree.put(tag, r.Value());
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
			static bool tryGetValue(daxia::reflect::Reflect_base* reflectBase,
				const boost::property_tree::ptree& root,
				ArrayInfo* parentArray)
			{
				using daxia::reflect::Reflect_base;
				using daxia::reflect::Reflect;

				const Reflect_base* impl = nullptr;

				size_t hash = 0;

				try
				{
					impl = dynamic_cast<const Reflect<T>*>(reflectBase);
				}
				catch (const std::exception&) {}

				if (impl == nullptr) return false;

				try
				{
					std::string tag = impl->Tag(JSON);
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
				const daxia::reflect::Reflect_base* reflectBase,
				const std::string& tag,
				boost::property_tree::ptree& root,
				ArrayInfo* parentArray)
			{
				using daxia::reflect::Reflect;

				const Reflect<std::vector<T>>* array = nullptr;

				try
				{
					array = dynamic_cast<const Reflect<std::vector<T>>*>(reflectBase);
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
					if (array->Value().empty())
					{
						boost::property_tree::ptree tr;
						child.push_back(make_pair("", tr));
					}
					else
					{
						for (auto iter = array->Value().begin(); iter != array->Value().end(); ++iter)
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
			static bool tryGetElement(daxia::reflect::Reflect_base* reflectBase,
				const boost::property_tree::ptree& root)
			{
				using daxia::reflect::Reflect;

				Reflect<std::vector<T>>* array = nullptr;

				try
				{
					array = dynamic_cast<Reflect<std::vector<T>>*>(reflectBase);
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
					array->Value().push_back(boost::lexical_cast<T>(iter->second.data()));
				}

				return true;
			}

		private:
			// 使用内存布局缓存进行编码
			static void marshal(const char* baseaddr,
				const boost::property_tree::ptree& layout,
				boost::property_tree::ptree& root,
				ArrayInfo* parentArray);

			// 使用内存布局缓存进行解码
			static void ummarshal(char* baseaddr,
				const boost::property_tree::ptree& layout,
				const boost::property_tree::ptree& root,
				ArrayInfo* parentArray);

			static void wptree2ptree(const boost::property_tree::wptree& wptree, boost::property_tree::ptree& ptree);

			static void putArray(const daxia::reflect::Reflect_base* reflectBase,
				const std::string& tag,
				boost::property_tree::ptree& root,
				ArrayInfo* parentArray);


			static void getArray(daxia::reflect::Reflect_base* reflectBase,
				const boost::property_tree::ptree& root);

			static void extendArrayLayout(const daxia::reflect::Reflect_base* reflectBase, boost::property_tree::ptree& layout);

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
					typedef typename boost::make_unsigned<Ch>::type UCh;
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
					else if (*b == Ch('/'))/* result += Ch('\\'), */result += Ch('/');
					else if (*b == Ch('"')) /* result += Ch('\\'), */result += Ch('"');
					else if (*b == Ch('\\'))/* result += Ch('\\'),*/ result += Ch('\\');
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

#endif // !__DAXIA_ENCODE_JSON_H

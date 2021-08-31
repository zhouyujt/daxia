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
#include <boost/property_tree/json_parser.hpp>
#include "../reflect/reflect.hpp"
#include "../string.hpp"

namespace daxia
{
	namespace encode
	{
		class Json
		{
		public:
			Json() = delete;
			~Json(){}
			// 调用本类任何方法前需调用且仅调用一次初始化
			static void Init();
		public:
			// 将指定的对象编码为json字符串
			// 字符串编码类型（ utf8 或 ansi ）跟对象内部使用的编码一致
			template<typename ValueType>
			static daxia::string Marshal(const ValueType& v, bool pretty = false/*是否换行*/)
			{
				using namespace std;
				using namespace boost::property_tree;
				using daxia::reflect::Reflect;

				// 获取内存布局
				const reflect::Layout& layout = Reflect<ValueType>::GetLayoutFast();

				ptree root;
				stringstream ss;
				const char* baseaddr = reinterpret_cast<const char*>(&v);
				marshal(baseaddr, layout, root);
				write_json(ss, root, pretty);

				return ss.str();
			}

			// 将json字符串解码到对象
			// 对象内字符串编码跟json字符串编码一致
			template<typename ValueType>
			static bool Unmarshal(const daxia::string& json, ValueType& v)
			{
				using namespace std;
				using namespace boost::property_tree;
				using daxia::reflect::Reflect;

				// 获取内存布局
				const reflect::Layout& layout = Reflect<ValueType>::GetLayoutFast();

				try
				{
					ptree root;
					stringstream ss(json.Utf8() ? static_cast<const std::string&>(json) : static_cast<const std::string&>(json.ToUtf8()));
					json_parser::read_json(ss, root);

					ummarshal(reinterpret_cast<char*>(&v), layout, root, json.Utf8());
				}
				catch (const boost::property_tree::ptree_error&)
				{
					return false;
				}

				return true;
			}

			template<typename ValueType>
			static bool Unmarshal(const void* json, size_t len, ValueType& v)
			{
				return Unmarshal(daxia::string(static_cast<const char*>(json), len), v);
			}
		private:
			// 使用内存布局缓存进行编码
			static void marshal(const char* baseaddr, const daxia::reflect::Layout& layout, boost::property_tree::ptree& root);
			inline static void putValue(const daxia::reflect::Reflect_base* reflectBase, const daxia::string& tag, boost::property_tree::ptree &root);
			inline static void putObject(const daxia::reflect::Reflect_base* reflectBase, const daxia::string& tag, const daxia::reflect::Layout& layout, boost::property_tree::ptree& root);
			inline static void putValueElement(const daxia::reflect::Reflect_base* reflectBase, const daxia::string& tag, boost::property_tree::ptree& root);
			inline static void putObjectElement(const daxia::reflect::Reflect_base* reflectBase, const std::string& tag, boost::property_tree::ptree& root);

			// 使用内存布局缓存进行解码
			static void ummarshal(char* baseaddr, const daxia::reflect::Layout& layout, const boost::property_tree::ptree& root, bool utf8);
			inline static void getValue(daxia::reflect::Reflect_base* reflectBase, daxia::string tag, const boost::property_tree::ptree &root, bool utf8);
			inline static void getObject(daxia::reflect::Reflect_base* reflectBase, daxia::string tag, const boost::property_tree::ptree &root, bool utf8);
			inline static void getValueElement(daxia::reflect::Reflect_base* reflectBase, const boost::property_tree::ptree& root, bool utf8);
			inline static void getObjectElement(daxia::reflect::Reflect_base* reflectBase, const boost::property_tree::ptree& root, bool utf8);
		private:
			class InitHelper
			{
			public:
				InitHelper();
			};
			// 由于boost::property_tree::write_json会将所有数据类型视为字符串类型，所以重写相关方法使之能区分不同类型
		private:
			template<typename Ptree>
			static void write_json(std::basic_ostream <
				typename Ptree::key_type::value_type
			> &stream,
			const Ptree &pt,
			bool pretty = true)
			{
				write_json_internal(stream, pt, std::string(), pretty);
			}

			template<typename Ptree>
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

			template<typename Ptree>
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
			template<typename Ch>
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

#endif // !__DAXIA_ENCODE_JSON_H

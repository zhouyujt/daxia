/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file sigleton.hpp
* \author 漓江里的大虾
* \date 七月 2021
*
* 封装std::string、std::wstring，提供一些高级方法以及VC里CString的一些方法
*
*/

#ifndef __DAXIA_STRING_HPP
#define __DAXIA_STRING_HPP
#include <string>
#include <vector>
#include <stdarg.h>
#include <ostream>
#include "encode/strconv.h"

#ifndef _T
#	ifdef UNICODE
#		define _T(txt) L##txt
#	else
#		define _T(txt) txt
#	endif
#endif

namespace daxia
{
	// ansi: sizeof(__utf8Test) == 24 
	// utf8: sizeof(__utf8Test) == 30
	const static char __utf8Test[] = "powered by 漓江里的大虾";

	template<class Elem, class Traits, class Alloc>
	class String_base
	{
	public:
		String_base();
		String_base(const Elem* str);
		String_base(const Elem* str, size_t count);
		String_base(const std::basic_string<Elem, Traits, Alloc>& str);
		String_base(const String_base& str);

		// VC里CString的功能
	public:
		size_t GetLength() const;
		const Elem* GetString() const;
		Elem* GetBuffer(size_t length = 0);
		void ReleaseBuffer(size_t maxCount = -1);
		void Empty();
		bool IsEmpty() const;
		size_t Find(Elem ch, size_t start = 0) const;
		size_t Find(const Elem* str, size_t start = 0) const;
		size_t Find(const void* str, size_t len, size_t start = 0) const;
		void Format(const Elem* format, ...);
		void FormatV(const char* format, va_list valist);
		void FormatV(const wchar_t* format, va_list valist);
		String_base Left(size_t count) const;
		String_base Right(size_t count) const;
		String_base Mid(size_t start, size_t count) const;
		String_base Tokenize(const Elem* sub, size_t& start) const;
		String_base Tokenize(const void* sub, size_t len, size_t& start) const;
		int Replace(Elem oldch, Elem newch);
		int Replace(const Elem* oldstr, const Elem* newstr);
		String_base& MakeLower();
		String_base& MakeUpper();
		String_base& Trim();
		String_base& TrimLeft();
		String_base& TrimRight();
		int Compare(const Elem* str) const;
		int CompareNoCase(const Elem* str) const;
		void Append(const Elem* str, size_t len);
		void Append(const Elem* str);
		size_t Delete(size_t start, size_t count = 1);
		// 其他方法
	public:
		// 获取指定位置的字符
		Elem& At(size_t pos);
		const Elem& At(size_t pos) const;
		// 获取哈希值
		std::size_t Hash() const;
		// 分割字符串
		void Split(const Elem* sub, std::vector<String_base>& strings) const;
		void Split(const void* sub, size_t len, std::vector<String_base>& strings) const;

		// 交换
		String_base& Swap(String_base& str);
		String_base& Swap(std::basic_string<Elem, Traits, Alloc>& str);

		// 字符集编码转换
		bool& Utf8() { return utf8_; }
		bool Utf8() const { return utf8_; }
		inline String_base< wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > ToUnicode() const;
		inline String_base< char, std::char_traits<char>, std::allocator<char> > ToAnsi() const;
		inline String_base< char, std::char_traits<char>, std::allocator<char> > ToUtf8() const;

		// 数字转换
		template<class T> static String_base ToString(T v);
		template<class T> T NumericCast() const;
	public:
		String_base& operator=(Elem ch)
		{
			v_.assign(1, ch);
			return *this;
		}

		String_base& operator=(const Elem* str)
		{
			v_.assign(str);
			return *this;
		}

		String_base& operator=(const std::basic_string<Elem, Traits, Alloc>& str)
		{
			v_.assign(str);
			return *this;
		}

		String_base& operator=(const String_base& str)
		{
			v_ = str.v_;
			utf8_ = str.utf8_;
			return *this;
		}

		String_base& operator+=(Elem ch)
		{
			v_.append(1, ch);
			return *this;
		}

		String_base& operator+=(const Elem* str)
		{
			v_.append(str);
			return *this;
		}

		String_base& operator+=(const String_base& str)
		{
			v_.append(str.v_, 0, -1);
			return *this;
		}

		String_base operator+(Elem ch) const
		{
			String_base s(*this);
			s.v_.append(1, ch);

			return s;
		}

		String_base operator+(const Elem* str) const
		{
			String_base s(*this);
			s.v_.append(str);

			return s;
		}

		String_base operator+(const String_base& str) const
		{
			String_base s(*this);
			s.v_.append(str.v_, 0, -1);

			return s;
		}

		bool operator>(const Elem* str) const
		{
			return Compare(str) > 0;
		}

		bool operator>(const String_base& str) const
		{
			return Compare(str.GetString()) > 0;
		}

		bool operator>=(const Elem* str) const
		{
			return !(Compare(str) < 0);
		}

		bool operator>=(const String_base& str) const
		{
			return !(Compare(str.GetString()) < 0);
		}

		bool operator<(const Elem* str) const
		{
			return Compare(str) < 0;
		}

		bool operator<(const String_base& str) const
		{
			return Compare(str.GetString()) < 0;
		}

		bool operator<=(const Elem* str) const
		{
			return !(Compare(str) > 0);
		}

		bool operator<=(const String_base& str) const
		{
			return !(Compare(str.GetString()) > 0);
		}

		bool operator==(const Elem* str) const
		{
			return Compare(str) == 0;
		}

		bool operator==(const String_base& str) const
		{
			return Compare(str.GetString()) == 0;
		}

		bool operator!=(const Elem* str) const
		{
			return Compare(str) != 0;
		}

		bool operator!=(const String_base& str) const
		{
			return Compare(str.GetString()) != 0;
		}

		const Elem& operator[](size_t pos) const
		{
			return v_[pos];
		}

		Elem& operator[](size_t pos)
		{
			return v_[pos];
		}

		operator const std::basic_string<Elem, Traits, Alloc>&() const
		{
			return v_;
		}

		operator std::basic_string<Elem, Traits, Alloc>&()
		{
			return v_;
		}

		friend std::basic_ostream<Elem>& operator<<(std::basic_ostream<Elem>& os, const String_base& s)
		{
			os << s.v_;
			return os;
		}
	private:
		size_t strlen(size_t maxCount = -1) const;
		void cast(char& v) const;
		void cast(unsigned char& v) const;
		void cast(int& v) const;
		void cast(unsigned int& v) const;
		void cast(long& v) const;
		void cast(unsigned long& v) const;
		void cast(long long& v) const;
		void cast(unsigned long long& v) const;
		void cast(float& v) const;
		void cast(double& v) const;
		void cast(long double& v) const;
	private:
		std::basic_string<Elem, Traits, Alloc> v_;
		bool utf8_;
	private:
		template <size_t>
		struct size{};

		template<typename ...>
		struct utf8
		{
			const static bool value = false;
		};

		template<class T>
		struct utf8 <T, size<30> >
		{
			const static bool value = true;
		};
	};

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base()
		: utf8_(utf8<void,size<sizeof(__utf8Test)>>::value)
	{
	}

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base(const Elem* str)
		: utf8_(utf8<void,size<sizeof(__utf8Test)>>::value)
	{
		if (str)
		{
			v_.append(str);
		}
	}

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base(const Elem* str, size_t count)
		: utf8_(utf8<void,size<sizeof(__utf8Test)>>::value)
	{
		if (str)
		{
			v_.append(str, count);
		}
	}

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base(const std::basic_string<Elem, Traits, Alloc>& str)
		: utf8_(utf8<void,size<sizeof(__utf8Test)>>::value)
	{
		v_.append(str, 0, -1);
	}

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base(const daxia::String_base<Elem, Traits, Alloc>& str)
		: utf8_(str.utf8_)
	{
		v_ = str.v_;
	}

	template<class Elem, class Traits, class Alloc>
	size_t daxia::String_base<Elem, Traits, Alloc>::GetLength() const
	{
		return v_.length();
	}

	template<class Elem, class Traits, class Alloc>
	const Elem* daxia::String_base<Elem, Traits, Alloc>::GetString() const
	{
		return v_.c_str();
	}

	template<class Elem, class Traits, class Alloc>
	Elem* daxia::String_base<Elem, Traits, Alloc>::GetBuffer(size_t length)
	{
		if (length == 0)
		{
			return const_cast<Elem*>(GetString());
		}

		size_t newLength = v_.size();
		if (newLength > 1024 * 1024 * 1024)
		{
			newLength += 1024 * 1024;
		}
		else
		{
			newLength += newLength / 2;
		}

		if (newLength < length)
		{
			newLength = length;
		}

		v_.resize(newLength);

		return const_cast<Elem*>(v_.c_str());
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::ReleaseBuffer(size_t maxCount)
	{
		if (maxCount == -1 || maxCount > v_.size())
		{
			maxCount = v_.size();
		}

		v_.resize(strlen(maxCount));
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::Empty()
	{
		v_.clear();
	}

	template<class Elem, class Traits, class Alloc>
	bool daxia::String_base<Elem, Traits, Alloc>::IsEmpty() const
	{
		return v_.empty();
	}

	template<class Elem, class Traits, class Alloc>
	size_t daxia::String_base<Elem, Traits, Alloc>::Find(Elem ch, size_t start /*= 0*/) const
	{
		return v_.find(ch, start);
	}

	template<class Elem, class Traits, class Alloc>
	size_t daxia::String_base<Elem, Traits, Alloc>::Find(const Elem* str, size_t start /*= 0*/) const
	{
		return v_.find(str, start);
	}

	template<class Elem, class Traits, class Alloc>
	size_t daxia::String_base<Elem, Traits, Alloc>::Find(const void* str, size_t len, size_t start /*= 0*/) const
	{
		return v_.find(static_cast<const Elem*>(str), start, len);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::Format(const Elem* format, ...)
	{
		va_list valist;
		va_start(valist, format);
		FormatV(format, valist);
		va_end(valist);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::FormatV(const char* format, va_list valist)
	{
		char buffer[1024];

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4996)
#endif // _MSC_VER

		vsnprintf(buffer, sizeof(buffer) / sizeof(char), format, valist);

#ifdef _MSC_VER
#	pragma warning(pop)
#endif // _MSC_VER

		*this = buffer;
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::FormatV(const wchar_t* format, va_list valist)
	{
		wchar_t buffer[1024];

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4996)
#endif // _MSC_VER

		vswprintf(buffer, sizeof(buffer) / sizeof(wchar_t), format, valist);

#ifdef _MSC_VER
#	pragma warning(pop)
#endif // _MSC_VER

		*this = buffer;
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Left(size_t count) const
	{
		return String_base<Elem, Traits, Alloc>(v_.substr(0, count));
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Right(size_t count) const
	{
		return String_base<Elem, Traits, Alloc>(v_.substr(v_.size() - count, count));
	}


	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Mid(size_t start, size_t count) const
	{
		if (start >= v_.size())
		{
			return String_base<Elem, Traits, Alloc>();
		}

		return String_base<Elem, Traits, Alloc>(v_.substr(start, count));
	}


	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Tokenize(const Elem* sub, size_t& start /*= 0*/) const
	{
		if (start >= v_.size())
		{
			start = -1;
			return String_base<Elem, Traits, Alloc>();
		}

		size_t pos = Find(sub, start);
		if (pos != -1)
		{
			size_t from = start;
			size_t count = pos - start;
			start = pos + 1;
			return Mid(from, count);
		}
		else
		{
			size_t from = start;
			start = v_.size() + 1;
			return Mid(from, -1);
		}
	}


	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Tokenize(const void* sub, size_t len, size_t& start) const
	{
		if (start >= v_.size())
		{
			start = -1;
			return String_base<Elem, Traits, Alloc>();
		}

		size_t pos = Find(sub, len, start);
		if (pos != -1)
		{
			size_t from = start;
			size_t count = pos - start;
			start = pos + 1;
			return Mid(from, count);
		}
		else
		{
			size_t from = start;
			start = v_.size() + 1;
			return Mid(from, -1);
		}
	}


	template<class Elem, class Traits, class Alloc>
	int daxia::String_base<Elem, Traits, Alloc>::Replace(Elem oldch, Elem newch)
	{
		int count = 0;

		if (oldch != newch)
		{
			Elem* buffer = GetBuffer();

			int length = GetLength();
			for (; count < length; ++count)
			{
				if (buffer[count] == oldch)
				{
					buffer[count] = newch;
				}
			}
		}

		return count;
	}

	template<class Elem, class Traits, class Alloc>
	int daxia::String_base<Elem, Traits, Alloc>::Replace(const Elem* oldstr, const Elem* newstr)
	{
		if (IsEmpty()) return 0;

		String_base<Elem, Traits, Alloc> buffer;
		size_t replaceLen = String_base<Elem, Traits, Alloc>(oldstr).GetLength();

		size_t pos;
		int count = 0;
		size_t start = 0;
		while ((pos = Find(oldstr, start)) != -1)
		{
			++count;
			buffer += Mid(start, pos - start);
			buffer += newstr;
			start = pos + replaceLen;
		}
		buffer += Mid(start, -1);

		v_.swap(buffer.v_);

		return count;
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc>& daxia::String_base<Elem, Traits, Alloc>::MakeLower()
	{
		v_ = daxia::encode::Strconv::MakeLower(v_);
		return *this;
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc>& daxia::String_base<Elem, Traits, Alloc>::MakeUpper()
	{
		v_ = daxia::encode::Strconv::MakeUpper(v_);
		return *this;
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc>& daxia::String_base<Elem, Traits, Alloc>::Trim()
	{
		TrimLeft();
		return TrimRight();
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc>& daxia::String_base<Elem, Traits, Alloc>::TrimLeft()
	{
		while (true)
		{
			if (v_.empty() || v_.front() != ' ') break;
			v_.erase(v_.begin());
		}

		return *this;
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc>& daxia::String_base<Elem, Traits, Alloc>::TrimRight()
	{
		while (true)
		{
			if (v_.empty() || v_.back() != ' ') break;
			v_.pop_back();
		}

		return *this;
	}

	template<class Elem, class Traits, class Alloc>
	int daxia::String_base<Elem, Traits, Alloc>::Compare(const Elem* str) const
	{
		return v_.compare(str);
	}

	template<class Elem, class Traits, class Alloc>
	int daxia::String_base<Elem, Traits, Alloc>::CompareNoCase(const Elem* str) const
	{
		String_base<Elem, Traits, Alloc> temp1(v_);
		String_base<Elem, Traits, Alloc> temp2(str);
		temp1.MakeLower();
		temp2.MakeLower();

		return temp1.Compare(temp2.GetString());
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::Append(const Elem* str, size_t len)
	{
		v_.append(str, len);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::Append(const Elem* str)
	{
		v_.append(str);
	}

	template<class Elem, class Traits, class Alloc>
	size_t daxia::String_base<Elem, Traits, Alloc>::Delete(size_t start, size_t count)
	{
		size_t oldLength = GetLength();
		if (start > oldLength) return oldLength;

		if (start + count > oldLength)
		{
			count = oldLength - start;
		}

		if (count > 0)
		{
			size_t newLength = oldLength - count;
			Elem* buff = GetBuffer();
			int copied = oldLength - (start + count);
			memmove(buff + start, buff + start + count, copied * sizeof(Elem));
			ReleaseBuffer(newLength);
		}

		return GetLength();
	}

	template<class Elem, class Traits, class Alloc>
	Elem& daxia::String_base<Elem, Traits, Alloc>::At(size_t pos)
	{
		return v_.at(pos);
	}

	template<class Elem, class Traits, class Alloc>
	const Elem& daxia::String_base<Elem, Traits, Alloc>::At(size_t pos) const
	{
		return v_.at(pos);
	}

	template<class Elem, class Traits, class Alloc>
	std::size_t daxia::String_base<Elem, Traits, Alloc>::Hash() const
	{
		return std::hash<std::basic_string<Elem, Traits, Alloc>>()(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::Split(const Elem* sub, std::vector<String_base>& strings) const
	{
		strings.clear();
		size_t pos = 0;
		while (pos != -1)
		{
			String_base<Elem, Traits, Alloc> str = Tokenize(sub, pos);
			if (!str.IsEmpty())
			{
				strings.push_back(str);
			}
		}
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::Split(const void* sub, size_t len, std::vector<String_base>& strings) const
	{
		strings.clear();
		size_t pos = 0;
		while (pos != -1)
		{
			String_base<Elem, Traits, Alloc> str = Tokenize(sub, len, pos);
			if (!str.IsEmpty())
			{
				strings.push_back(str);
			}
		}
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc>& daxia::String_base<Elem, Traits, Alloc>::Swap(String_base<Elem, Traits, Alloc>& str)
	{
		v_.swap(str.v_);
		return *this;
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc>& daxia::String_base<Elem, Traits, Alloc>::Swap(std::basic_string<Elem, Traits, Alloc>& str)
	{
		v_.swap(str);
		return *this;
	}

	template<>
	inline daxia::String_base< wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > daxia::String_base<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::ToUnicode() const
	{
		return *this;
	}

	template<>
	inline daxia::String_base< wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > daxia::String_base<char, std::char_traits<char>, std::allocator<char>>::ToUnicode() const
	{
		if (utf8_)
		{
			return daxia::encode::Strconv::Utf82Unicode(reinterpret_cast<const char*>(v_.c_str()));
		}
		else
		{
			return daxia::encode::Strconv::Ansi2Unicode(reinterpret_cast<const char*>(v_.c_str()));
		}
	}

	template<>
	inline daxia::String_base< char, std::char_traits<char>, std::allocator<char> > daxia::String_base<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::ToAnsi() const
	{
		daxia::String_base< char, std::char_traits<char>, std::allocator<char> > result = daxia::encode::Strconv::Unicode2Ansi(reinterpret_cast<const wchar_t*>(v_.c_str()));
		result.Utf8() = false;
		return result;
	}

	template<>
	inline daxia::String_base< char, std::char_traits<char>, std::allocator<char> > daxia::String_base<char, std::char_traits<char>, std::allocator<char>>::ToAnsi() const
	{
		if (utf8_)
		{
			daxia::String_base< char, std::char_traits<char>, std::allocator<char> > result = daxia::encode::Strconv::Utf82Ansi(reinterpret_cast<const char*>(v_.c_str()));
			result.Utf8() = false;
			return result;
		}
		else
		{
			return *this;
		}
	}

	template<>
	inline daxia::String_base< char, std::char_traits<char>, std::allocator<char> > daxia::String_base<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::ToUtf8() const
	{
		daxia::String_base< char, std::char_traits<char>, std::allocator<char> > result = daxia::encode::Strconv::Unicode2Utf8(reinterpret_cast<const wchar_t*>(v_.c_str()));
		result.Utf8() = true;
		return result;
	}

	template<>
	inline daxia::String_base< char, std::char_traits<char>, std::allocator<char> > daxia::String_base<char, std::char_traits<char>, std::allocator<char>>::ToUtf8() const
	{
		if (utf8_)
		{
			return *this;
		}
		else
		{
			daxia::String_base< char, std::char_traits<char>, std::allocator<char> > result = daxia::encode::Strconv::Ansi2Utf8(reinterpret_cast<const char*>(v_.c_str()));
			result.utf8_ = true;
			return result;
		}
	}

	template<>
	template<class T>
	inline String_base<char, std::char_traits<char>, std::allocator<char>> daxia::String_base<char, std::char_traits<char>, std::allocator<char>>::ToString(T v)
	{
		return std::to_string(v);
	}

	template<>
	template<class T>
	inline String_base<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>> daxia::String_base<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>::ToString(T v)
	{
		return std::to_wstring(v);
	}

	template<class Elem, class Traits, class Alloc>
	template<class T>
	T daxia::String_base<Elem, Traits, Alloc>::NumericCast() const
	{
		if (v_.empty())
		{
			return 0;
		}
		else
		{
			T v;
			cast(v);
			return v;
		}
	}

	template<class Elem, class Traits, class Alloc>
	size_t daxia::String_base<Elem, Traits, Alloc>::strlen(size_t maxCount /*= -1*/) const
	{
		size_t length = 0;

		const Elem* start = v_.c_str();
		while (maxCount < 0 || length <= maxCount)
		{
			if (!start[length++])
			{
				break;
			}
		}

		return length - 1;
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(char& v) const
	{
		v = static_cast<char>(std::stoi(v_));
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(unsigned char& v) const
	{
		v = static_cast<unsigned char>(std::stoi(v_));
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(int& v) const
	{
		v = std::stoi(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(unsigned int& v) const
	{
		v = static_cast<unsigned int>(std::stoi(v_));
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(long& v) const
	{
		v = std::stol(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(unsigned long& v) const
	{
		v = std::stoul(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(long long& v) const
	{
		v = std::stoll(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(unsigned long long& v) const
	{
		v = std::stoull(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(float& v) const
	{
		v = std::stof(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(double& v) const
	{
		v = std::stod(v_);
	}

	template<class Elem, class Traits, class Alloc>
	void daxia::String_base<Elem, Traits, Alloc>::cast(long double& v) const
	{
		v = std::stold(v_);
	}

	typedef String_base<char, std::char_traits<char>, std::allocator<char>> string;
	typedef String_base<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>> wstring;

	class buffer : public string
	{
	public:
		buffer(){}
		buffer(const char* str) : string(str){}
		buffer(const char* str, size_t count) : string(str, count) {}
		buffer(const std::string& str) : string(str){}
	};
#ifdef UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif // UNICODE

}

namespace std
{
	template <typename ValueType>
	class hash<daxia::String_base<ValueType, std::char_traits<ValueType>, std::allocator<ValueType>>>
	{
	public:
		size_t operator()(const daxia::String_base<ValueType, std::char_traits<ValueType>, std::allocator<ValueType>>& obj) const
		{
			return obj.Hash();
		}
	};
};

#endif // !__DAXIA_STRING_HPP

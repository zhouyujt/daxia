/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2018 漓江里的大虾.
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

std::string a;

namespace daxia
{
	template<class Elem,class Traits,class Alloc>
	class String_base
	{
	public:
		String_base();
		String_base(const Elem* str);
		String_base(const std::basic_string<Elem, Traits, Alloc>& str);

		// VC里CString的功能
	public:
		int GetLength() const;
		const Elem* GetString() const;
		Elem* GetBuffer(int length = 0);
		void ReleaseBuffer(int maxCount = -1);
		void Empty();
		bool IsEmpty() const;
		int Find(Elem ch, int start = 0) const;
		int Find(const Elem* str, int start = 0) const;
		void Format(const Elem* format, ...);
		void FormatV(const Elem* format, va_list valist);
		String_base Left(int count) const; 
		String_base Right(int count) const;
		String_base Mid(int start, int count) const;
		String_base Tokenize(const Elem* sub, int& start) const;
		int Replace(Elem oldch, Elem newch);
		int Replace(const Elem* oldstr, const Elem* newstr);
		// 其他方法
	public:
		// 分割字符串
		void Split(const Elem* sub, std::vector<String_base>& strings) const;
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

		operator const Elem*()
		{
			return v_.c_str();
		}
	private:
		int strlen(int maxCount = -1) const;
	private:
		std::basic_string<Elem, Traits, Alloc> v_;
	};

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base()
	{

	}

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base(const Elem* str)
	{
		v_ = std::basic_string<Elem, Traits, Alloc>(str);
	}

	template<class Elem, class Traits, class Alloc>
	daxia::String_base<Elem, Traits, Alloc>::String_base(const std::basic_string<Elem, Traits, Alloc>& str)
	{
		v_ = std::basic_string<Elem, Traits, Alloc>(str);
	}

	template<class Elem, class Traits, class Alloc>
	int daxia::String_base<Elem, Traits, Alloc>::GetLength() const
	{
		return v_.length();
	}

	template<class Elem, class Traits, class Alloc>
	const Elem* daxia::String_base<Elem, Traits, Alloc>::GetString() const
	{
		return v_.c_str();
	}

	template<class Elem, class Traits, class Alloc>
	Elem* daxia::String_base<Elem, Traits, Alloc>::GetBuffer(int length)
	{
		if (length == 0)
		{
			return const_cast<Elem*>GetString();
		}

		int newLength = v_.size();
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
	void daxia::String_base<Elem, Traits, Alloc>::ReleaseBuffer(int maxCount)
	{
		if (maxCount == -1 || maxCount > static_cast<int>(v_.size()))
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
	int daxia::String_base<Elem, Traits, Alloc>::Find(Elem ch, int start /*= 0*/) const
	{
		return v_.find(ch, start);
	}

	template<class Elem, class Traits, class Alloc>
	int daxia::String_base<Elem, Traits, Alloc>::Find(const Elem* str, int start /*= 0*/) const
	{
		return v_.find(str, start);
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
	void daxia::String_base<Elem, Traits, Alloc>::FormatV(const Elem* format, va_list valist)
	{
		Elem buffer[1024];

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4996)
#endif // _MSC_VER

#ifdef UNICODE
		vswprintf(buffer, sizeof(buffer) / sizeof(Elem), format, valist);
#else
		vsnprintf(buffer, sizeof(buffer) / sizeof(Elem), format, valist);
#endif

#ifdef _MSC_VER
#	pragma warning(pop)
#endif // _MSC_VER

		*this = buffer;
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Left(int count) const
	{
		return String_base<Elem, Traits, Alloc>(v_.substr(0, count));
	}

	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Right(int count) const
	{
		return String_base<Elem, Traits, Alloc>(v_.substr(v_.size() - count, count));
	}


	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Mid(int start, int count) const
	{
		if (start >= static_cast<int>(v_.size()))
		{
			return String_base<Elem, Traits, Alloc>();
		}

		return String_base<Elem, Traits, Alloc>(v_.substr(start, count));
	}


	template<class Elem, class Traits, class Alloc>
	String_base<Elem, Traits, Alloc> daxia::String_base<Elem, Traits, Alloc>::Tokenize(const Elem* sub, int& start /*= 0*/) const
	{
		if (start >= static_cast<int>(v_.size()))
		{
			start = -1;
			return String_base<Elem, Traits, Alloc>();
		}

		int pos = Find(sub, start);
		if (pos != -1)
		{
			int from = start;
			int count = pos - start;
			start = pos + 1;
			return Mid(from, count);
		}
		else
		{
			int from = start;
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
			while (count < length)
			{
				if (buffer[count] == oldch)
				{
					buffer[count] = newch;
					++count;
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
		int replaceLen = String_base<Elem, Traits, Alloc>(oldstr).GetLength();

		int pos;
		int count = 0;
		int start = 0;
		while ((pos = Find(oldstr,start)) != -1)
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
	void daxia::String_base<Elem, Traits, Alloc>::Split(const Elem* sub, std::vector<String_base>& strings) const
	{
		strings.clear();
		int pos = 0;
		while (pos >= 0)
		{
			String_base<Elem, Traits, Alloc> str = Tokenize(sub, pos);
			if (!str.IsEmpty())
			{
				strings.push_back(str);
			}
		}
	}

	template<class Elem, class Traits, class Alloc>
	int daxia::String_base<Elem, Traits, Alloc>::strlen(int maxCount /*= -1*/) const
	{
		int length = 0;

		const Elem* start = v_.c_str();
		while (maxCount < 0 || length < maxCount)
		{
			if (!start[length++])
			{
				break;
			}
		}

		return length - 1;
	}

	typedef String_base<char, std::char_traits<char>, std::allocator<char>> StringA;
	typedef String_base<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>> StringW;
#ifdef UNICODE
	typedef StringW String;
#else
	typedef StringA String;
#endif // UNICODE

}

#endif // !__DAXIA_STRING_HPP

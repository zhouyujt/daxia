#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <string>
#include <string.h>
#include <algorithm>
#include "strconv.h"

namespace daxia
{
	namespace encode
	{
		std::string Strconv::Unicode2Ansi(const wchar_t* str)
		{
			std::string r;

#ifdef _WIN32
			int size = ::WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
			--size;
			r.resize(size);
			::WideCharToMultiByte(CP_ACP, 0, str, -1, const_cast<char*>(r.c_str()), size, NULL, NULL);
#else
			if (str == nullptr || wcslen(str) == 0) return r;

			auto old = setlocale(LC_ALL, "zh_CN.gbk");
			if (old == nullptr) return r;
		
			size_t size = wcstombs(nullptr, str, 0);
			if (size == (size_t)-1) return r;

			r.resize(size);
			wcstombs(const_cast<char*>(r.c_str()), str, size);

			setlocale(LC_ALL, old);
#endif // _WIN32

			return r;
		}

		std::string Strconv::Unicode2Ansi(const std::wstring& str)
		{
			return Unicode2Ansi(str.c_str());
		}

		std::wstring Strconv::Ansi2Unicode(const char* str)
		{
			std::wstring r;

#ifdef _WIN32
			int size = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
			--size;
			r.resize(size);
			::MultiByteToWideChar(CP_ACP, 0, str, -1, const_cast<wchar_t*>(r.c_str()), size);
#else
			if (str == nullptr || strlen(str) == 0) return r;

			auto old = setlocale(LC_ALL, "zh_CN.gbk");
			if (old == nullptr) return r;

			size_t size = mbstowcs(nullptr, str, 0);
			if (size == (size_t)-1) return r;

			r.resize(size);
			mbstowcs(const_cast<wchar_t*>(r.c_str()), str, size);

			setlocale(LC_ALL, old);
#endif // _WIN32

			return r;
		}

		std::wstring Strconv::Ansi2Unicode(const std::string& str)
		{
			return Ansi2Unicode(str.c_str());
		}

		std::wstring Strconv::Utf82Unicode(const char* str)
		{
			std::wstring r;

#ifdef _WIN32
			int size = ::MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
			--size;
			r.resize(size);
			::MultiByteToWideChar(CP_UTF8, 0, str, -1, const_cast<wchar_t*>(r.c_str()), size);
#else
			if (str == nullptr || strlen(str) == 0) return r;

			auto old = setlocale(LC_ALL, "zh_CN.utf8");
			if (old == nullptr) return r;

			size_t size = mbstowcs(nullptr, str, 0);
			if (size == (size_t)-1) return r;

			r.resize(size);
			mbstowcs(const_cast<wchar_t*>(r.c_str()), str, size);

			setlocale(LC_ALL, old);
#endif // _WIN32

			return r;
		}

		std::wstring Strconv::Utf82Unicode(const std::string& str)
		{
			return Utf82Unicode(str.c_str());
		}

		std::string Strconv::Utf82Ansi(const char* str)
		{
			std::string r;

			std::wstring temp = Utf82Unicode(str);
			r = Unicode2Ansi(temp.c_str());

			return r;
		}

		std::string Strconv::Utf82Ansi(const std::string& str)
		{
			return Utf82Ansi(str.c_str());
		}

		std::string Strconv::Unicode2Utf8(const wchar_t* str)
		{
			std::string r;

#ifdef _WIN32
			int size = ::WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
			--size;
			r.resize(size);
			::WideCharToMultiByte(CP_UTF8, 0, str, -1, const_cast<char*>(r.c_str()), size, NULL, NULL);
#else
			if (str == nullptr || wcslen(str) == 0) return r;

			auto old = setlocale(LC_ALL, "zh_CN.utf8");
			if (old == nullptr) return r;

			size_t size = wcstombs(nullptr, str, 0);
			if (size == (size_t)-1) return r;

			r.resize(size);
			wcstombs(const_cast<char*>(r.c_str()), str, size);

			setlocale(LC_ALL, old);
#endif // _WIN32

			return r;
		}

		std::string Strconv::Unicode2Utf8(const std::wstring& str)
		{
			return Unicode2Utf8(str.c_str());
		}

		std::string Strconv::Ansi2Utf8(const char* str)
		{
			std::string r;

			std::wstring temp = Ansi2Unicode(str);
			r = Unicode2Utf8(temp.c_str());

			return r;
		}

		std::string Strconv::Ansi2Utf8(const std::string& str)
		{
			return Ansi2Utf8(str.c_str());
		}

		std::string Strconv::MakeLower(const char* str)
		{
			std::string s(str);
			std::transform(s.begin(), s.end(), s.begin(), tolower);

			return s;
		}

		std::string Strconv::MakeLower(const std::string& str)
		{
			return MakeLower(str.c_str());
		}

		std::wstring Strconv::MakeLower(const wchar_t* str)
		{
			std::wstring s(str);
			std::transform(s.begin(), s.end(), s.begin(), tolower);

			return s;
		}

		std::wstring Strconv::MakeLower(const std::wstring& str)
		{
			return MakeLower(str.c_str());
		}

		std::string Strconv::MakeUpper(const char* str)
		{
			std::string s(str);
			std::transform(s.begin(), s.end(), s.begin(), toupper);

			return s;
		}

		std::string Strconv::MakeUpper(const std::string& str)
		{
			return MakeUpper(str.c_str());
		}

		std::wstring Strconv::MakeUpper(const wchar_t* str)
		{
			std::wstring s(str);
			std::transform(s.begin(), s.end(), s.begin(), toupper);

			return s;
		}

		std::wstring Strconv::MakeUpper(const std::wstring& str)
		{
			return MakeUpper(str.c_str());
		}

	}// namespace encode
}// namespace daxia

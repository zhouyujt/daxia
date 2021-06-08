#include <string>

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "strconv.h"

namespace daxia
{
	namespace encode
	{
		std::string Strconv::Unicode2Ansi(const wchar_t* str)
		{
			std::string r;

#ifdef _MSC_VER
			int size = ::WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
			--size;
			r.resize(size);
			::WideCharToMultiByte(CP_ACP, 0, str, -1, const_cast<char*>(r.c_str()), size, NULL, NULL);
#else
#endif // _MSC_VER

			return r;
		}

		std::wstring Strconv::Ansi2Unicode(const char* str)
		{
			std::wstring r;

#ifdef _MSC_VER
			int size = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
			--size;
			r.resize(size);
			::MultiByteToWideChar(CP_ACP, 0, str, -1, const_cast<wchar_t*>(r.c_str()), size);
#else
#endif // _MSC_VER

			return r;
		}

		std::wstring Strconv::Utf82Unicode(const char* str)
		{
			std::wstring r;

#ifdef _MSC_VER
			int size = ::MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
			--size;
			r.resize(size);
			::MultiByteToWideChar(CP_UTF8, 0, str, -1, const_cast<wchar_t*>(r.c_str()), size);
#else
#endif // _MSC_VER

			return r;
		}

		std::string Strconv::Utf82Ansi(const char* str)
		{
			std::string r;

#ifdef _MSC_VER
			std::wstring temp = Utf82Unicode(str);
			r = Unicode2Ansi(temp.c_str());
#else
#endif // _MSC_VER

			return r;
		}

		std::string Strconv::Unicode2Utf8(const wchar_t* str)
		{
			std::string r;

#ifdef _MSC_VER
			int size = ::WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
			--size;
			r.resize(size);
			::WideCharToMultiByte(CP_UTF8, 0, str, -1, const_cast<char*>(r.c_str()), size, NULL, NULL);
#else
#endif // _MSC_VER

			return r;
		}

		std::string Strconv::Ansi2Utf8(const char* str)
		{
			std::string r;

#ifdef _MSC_VER
			std::wstring temp = Ansi2Unicode(str);
			r = Unicode2Utf8(temp.c_str());
#else
#endif // _MSC_VER

			return r;
		}

	}// namespace encode
}// namespace daxia

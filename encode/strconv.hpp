/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file strconv.hpp
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 * ascii、unicode、utf-8等字符串之间的编码相互转换
 *
 */
#ifndef __DAXIA_ENCODE_STRCONV_HPP
#define __DAXIA_ENCODE_STRCONV_HPP

#include <string>

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

namespace daxia
{
	namespace encode
	{
                inline std::string Unicode2Ansi(const wchar_t* pString)
		{
			std::string strRet;

#ifdef _MSC_VER
			int nSize = ::WideCharToMultiByte(CP_ACP, 0, pString, -1, NULL, 0, NULL, NULL);
			--nSize;
			strRet.resize(nSize);
			::WideCharToMultiByte(CP_ACP, 0, pString, -1, const_cast<char*>(strRet.c_str()), nSize, NULL, NULL);
#else
#endif // _MSC_VER

			return strRet;
		}

                inline std::wstring Ansi2Unicode(const char* pString)
		{
			std::wstring strRet;

#ifdef _MSC_VER
			int nSize = ::MultiByteToWideChar(CP_ACP, 0, pString, -1, NULL, 0);
			--nSize;
			strRet.resize(nSize);
			::MultiByteToWideChar(CP_ACP, 0, pString, -1, const_cast<wchar_t*>(strRet.c_str()), nSize);
#else
#endif // _MSC_VER

			return strRet;
		}

                inline std::wstring Utf82Unicode(const char* pString)
		{
			std::wstring strRet;

#ifdef _MSC_VER
			int nSize = ::MultiByteToWideChar(CP_UTF8, 0, pString, -1, NULL, 0);
			--nSize;
			strRet.resize(nSize);
			::MultiByteToWideChar(CP_UTF8, 0, pString, -1, const_cast<wchar_t*>(strRet.c_str()), nSize);
#else
#endif // _MSC_VER

			return strRet;
		}

                inline std::string Utf82Ansi(const char* pString)
		{
			std::string strRet;

#ifdef _MSC_VER
			std::wstring temp = Utf82Unicode(pString);
			strRet = Unicode2Ansi(temp.c_str());
#else
#endif // _MSC_VER

			return strRet;
		}

                inline std::string Unicode2Utf8(const wchar_t* pString)
		{
			std::string strRet;

#ifdef _MSC_VER
			int nSize = ::WideCharToMultiByte(CP_UTF8, 0, pString, -1, NULL, 0, NULL, NULL);
			--nSize;
			strRet.resize(nSize);
			::WideCharToMultiByte(CP_UTF8, 0, pString, -1, const_cast<char*>(strRet.c_str()), nSize, NULL, NULL);
#else
#endif // _MSC_VER

			return strRet;
		}

                inline std::string Ansi2Utf8(const char* pString)
		{
			std::string strRet;

#ifdef _MSC_VER
			std::wstring temp = Ansi2Unicode(pString);
			strRet = Unicode2Utf8(temp.c_str());
#else
#endif // _MSC_VER

			return strRet;
		}

	}// namespace encode
}// namespace daxia

#endif // !__DAXIA_ENCODE_STRCONV_HPP

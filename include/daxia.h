#ifndef __DAXIA_INCLUDE_DAXIA_H
#define __DAXIA_INCLUDE_DAXIA_H

// dxg
#include "../dxg/client.h"
#include "../dxg/server.h"

// encode
#include "../encode/json.h"
#include "../encode/strconv.h"
#include "../encode/base64.h"

// reflect
#include "../reflect/reflect.hpp"

// win32
#include "../win32/processes_manager.h"
#include "../win32/path.h"

#define dxSTRCAT_HELP(s1,s2) s1 ## s2
#define dxSTRCAT(s1,s2) dxSTRCAT_HELP(s1,s2)
#define dxSTRCAT3(s1,s2,s3) dxSTRCAT(dxSTRCAT(s1,s2),s3)
#define dxSTRCAT4(s1,s2,s3,s4) dxSTRCAT(dxSTRCAT3(s1,s2,s3),s4)
#define dxSTRCAT5(s1,s2,s3,s4,s5) dxSTRCAT(dxSTRCAT4(s1,s2,s3,s4),s5)
#define dxSTRCAT6(s1,s2,s3,s4,s5,s6) dxSTRCAT(dxSTRCAT5(s1,s2,s3,s4,s5),s6)
#define dxSTRCAT7(s1,s2,s3,s4,s5,s6,s7) dxSTRCAT(dxSTRCAT6(s1,s2,s3,s4,s5,s6),s7)
#define dxSTRCAT8(s1,s2,s3,s4,s5,s6,s7,s8) dxSTRCAT(dxSTRCAT7(s1,s2,s3,s4,s5,s6,s7),s8)
#define dxSTRCAT9(s1,s2,s3,s4,s5,s6,s7,s8,s9) dxSTRCAT(dxSTRCAT8(s1,s2,s3,s4,s5,s6,s7,s8),s9)
#define dxSTR_HELP(s) #s
#define dxSTR(s) dxSTR_HELP(s)

#ifdef _MSC_VER
//MSC    1.0   _MSC_VER == 100
//MSC    2.0   _MSC_VER == 200
//MSC    3.0   _MSC_VER == 300
//MSC    4.0   _MSC_VER == 400
//MSC    5.0   _MSC_VER == 500
//MSC    6.0   _MSC_VER == 600
//MSC    7.0   _MSC_VER == 700
//MSVC++ 1.0   _MSC_VER == 800
//MSVC++ 2.0   _MSC_VER == 900
//MSVC++ 4.0   _MSC_VER == 1000 (Developer Studio 4.0)
//MSVC++ 4.2   _MSC_VER == 1020 (Developer Studio 4.2)
//MSVC++ 5.0   _MSC_VER == 1100 (Visual Studio 97 version 5.0)
//MSVC++ 6.0   _MSC_VER == 1200 (Visual Studio 6.0 version 6.0)
//MSVC++ 7.0   _MSC_VER == 1300 (Visual Studio.NET 2002 version 7.0)
//MSVC++ 7.1   _MSC_VER == 1310 (Visual Studio.NET 2003 version 7.1)
//MSVC++ 8.0   _MSC_VER == 1400 (Visual Studio 2005 version 8.0)
//MSVC++ 9.0   _MSC_VER == 1500 (Visual Studio 2008 version 9.0)
//MSVC++ 10.0  _MSC_VER == 1600 (Visual Studio 2010 version 10.0)
//MSVC++ 11.0  _MSC_VER == 1700 (Visual Studio 2012 version 11.0)
//MSVC++ 12.0  _MSC_VER == 1800 (Visual Studio 2013 version 12.0)
//MSVC++ 14.0  _MSC_VER == 1900 (Visual Studio 2015 version 14.0)
//MSVC++ 14.1  _MSC_VER == 1910 (Visual Studio 2017 version 15.0)
//MSVC++ 14.11 _MSC_VER == 1911 (Visual Studio 2017 version 15.3)
//MSVC++ 14.12 _MSC_VER == 1912 (Visual Studio 2017 version 15.5)
//MSVC++ 14.13 _MSC_VER == 1913 (Visual Studio 2017 version 15.6)
//MSVC++ 14.14 _MSC_VER == 1914 (Visual Studio 2017 version 15.7)
//MSVC++ 14.15 _MSC_VER == 1915 (Visual Studio 2017 version 15.8)
//MSVC++ 14.16 _MSC_VER == 1916 (Visual Studio 2017 version 15.9)
//MSVC++ 14.2  _MSC_VER == 1920 (Visual Studio 2019 Version 16.0)
//MSVC++ 14.21 _MSC_VER == 1921 (Visual Studio 2019 Version 16.1)
//MSVC++ 14.22 _MSC_VER == 1922 (Visual Studio 2019 Version 16.2)
#if _MSC_VER == 1600
#define VER v100
#elif _MSC_VER == 1700
#define VER v110
#elif _MSC_VER == 1800
#define VER v120
#elif _MSC_VER == 1900
#define VER v140
#elif _MSC_VER == 1910
#define VER v150
#elif _MSC_VER == 1911
#define VER v153
#elif _MSC_VER == 1912
#define VER v155
#elif _MSC_VER == 1913
#define VER v156
#elif _MSC_VER == 1914
#define VER v157
#elif _MSC_VER == 1915
#define VER v158
#elif _MSC_VER == 1916
#define VER v159
#elif _MSC_VER == 1920
#define VER v160
#elif _MSC_VER == 1921
#define VER v161
#elif _MSC_VER == 1922
#define VER v162
#else
#	pragma message("daxia库当前版本暂不支持当前的编译器！")
#	pragma message("请前往 https://github.com/zhouyujt/daxia 下载最新版本")
#	pragma message("如有任何疑问，请联系作者")
#	pragma message("QQ:50347187")
#endif

#ifdef _DEBUG
#	ifdef _DLL
#		define MDMT mdd
#	else
#		define MDMT mtd
#	endif
#else
#	ifdef _DLL
#		define MDMT md
#	else
#		define MDMT mt
#	endif
#endif // _DEBUG

#ifdef _WIN64
#	define PRENAME daxia_x64_x64_
#else
#	define PRENAME daxia_x86_Win32_
#endif // _WIN64

#pragma comment(lib,dxSTR(dxSTRCAT5(PRENAME,VER,_,MDMT,.lib)))

#undef VER
#undef MDMT
#undef PRENAME

#endif // !_MSC_VER
#endif // !__DAXIA_INCLUDE_DAXIA_H

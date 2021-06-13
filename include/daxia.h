#ifndef __DAXIA_INCLUDE_DAXIA_H
#define __DAXIA_INCLUDE_DAXIA_H

// dxg
#include "../dxg/client.h"
#include "../dxg/server.h"

// encode
#include "../encode/json.h"
#include "../encode/strconv.h"

// reflect
#include "../reflect/reflect.hpp"

// win32
#include "../win32/processes_manager.h"
#include "../win32/path.h"

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
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v100_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v100_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v100_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v100_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1700
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v110_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v110_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v110_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v110_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1800
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v120_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v120_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v120_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v120_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1900
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v140_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v140_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v140_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v140_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1910
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v150_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v150_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v150_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v150_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1911
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v153_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v153_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v153_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v153_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1912
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v155_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v155_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v155_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v155_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1913
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v156_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v156_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v156_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v156_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1914
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v157_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v157_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v157_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v157_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1915
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v158_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v158_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v158_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v158_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1916
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v159_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v159_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v159_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v159_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1920
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v160_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v160_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v160_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v160_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1921
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v161_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v161_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v161_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v161_mt.lib")
#		endif
#	endif
#elif _MSC_VER == 1922
#	ifdef _DEBUG
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v162_mdd.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v162_mtd.lib")
#		endif
#	else
#		ifdef _DLL
#			pragma comment(lib,"daxia_x86_Win32_v162_md.lib")
#		else
#			pragma comment(lib,"daxia_x86_Win32_v162_mt.lib")
#		endif
#	endif
#else
#	pragma message("daxia库当前版本暂不支持当前的编译器！")
#	pragma message("请前往 https://github.com/zhouyujt/daxia 下载最新版本")
#	pragma message("如有任何疑问，请联系作者")
#	pragma message("QQ:50347187")
#endif
#endif // !_MSC_VER
#endif // !__DAXIA_INCLUDE_DAXIA_H

#ifndef __DAXIA_INCLUDE_DAXIA_H
#define __DAXIA_INCLUDE_DAXIA_H

// net
#include "../net/client.h"
#include "../net/http_client.h"
#include "../net/server.h"
#include "../net/common/byte_order.hpp"

// encode
#include "../encode/json.h"
#include "../encode/strconv.h"
#include "../encode/base64.h"
#include "../encode/md5.h"
#include "../encode/uuid.h"
#include "../encode/hex.h"
#include "../encode/url.h"
#include "../encode/zlib_wrap.h"

// reflect
#include "../reflect/reflect.hpp"

// system
#include "../system/datetime.h"
#include "../system/threadpool/thread_pool.h"
#include "../system/file.h"
#include "../system/find_file.h"
#ifdef _WIN32
#include "../system/windows/find_process.h"
#include "../system/windows/find_window.h"
#include "../system/windows/path.h"
#endif

// orm
#include "../database/orm.h"

// coroutine
#include "../system/coroutine.h"

#include "../singleton.hpp"
#include "../string.hpp"

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
#ifdef _WIN64
#	define dxPATH x64/
#else
#	define dxPATH
#endif // !_WIN64

#ifdef _DEBUG
#	ifdef MT
#		define dxPATH2 debugMT/
#	else
#		define dxPATH2 debug/
#	endif
#else
#	ifdef MT
#		define dxPATH2 releaseMT/
#	else
#		define dxPATH2 release/
#	endif
#endif // !_DEBUG

#ifndef DAXIA_DONOT_LINK_LIBRARIES
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,net.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,encode.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,reflect.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,system.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,database.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,sqlite3.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,zlib.lib)))
#endif

#undef dxPATH
#undef dxPATH2
#endif // _MSC_VER

namespace daxia
{
	inline static void Init()
	{
		daxia::database::Orm::Init();
		daxia::encode::Json::Init();
	}
}

#endif // !__DAXIA_INCLUDE_DAXIA_H

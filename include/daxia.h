#ifndef __DAXIA_INCLUDE_DAXIA_H
#define __DAXIA_INCLUDE_DAXIA_H

// dxg
#include "../dxg/client.h"
#include "../dxg/server.h"

// encode
#include "../encode/json.h"
#include "../encode/strconv.h"
#include "../encode/base64.h"
#include "../encode/md5.h"

// reflect
#include "../reflect/reflect.hpp"

// win32
#include "../win32/processes_manager.h"
#include "../win32/path.h"

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
#	define dxPATH2 debug/
#else
#	define dxPATH2 release/Release/
#endif // !_DEBUG

#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,dxg.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,encode.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,reflect.lib)))
#pragma comment(lib,dxSTR(dxSTRCAT3(dxPATH,dxPATH2,win32.lib)))

#undef dxPATH
#undef dxPATH2
#endif // _MSC_VER


#endif // !__DAXIA_INCLUDE_DAXIA_H

# 查找OpenSSL开发环境
# 查找成功后会定义如下变量：
# OpenSSL_INCLUDE_DIRS      - 头文件路径
# OpenSSL_LIBRARY_DIRS      - 链接库路径
# OpenSSL_LIBRARIES         - 需要链接的库
# 
# author 漓江里的大虾
# date 十一月 2021
#

if( WIN32 )
	find_path( OpenSSL_INCLUDE_DIR
		NAMES "ssl.h"
		PATHS "$ENV{PROGRAMFILES}/OpenSSL/include/openssl"
			  "$ENV{PROGRAMFILES\(x86\)}/OpenSSL/include/openssl"
			  "$ENV{SYSTEMDRIVE}/OpenSSL/include/openssl")
	
	find_library( OpenSSL_LIBRARY_DIR
		NAMES "libssl" "libcrypto"
		PATHS "$ENV{PROGRAMFILES}/OpenSSL/lib"
			  "$ENV{PROGRAMFILES\(x86\)}/OpenSSL/lib"
			  "$ENV{SYSTEMDRIVE}/OpenSSL/lib" )
else()
	find_path( OpenSSL_INCLUDE_DIR
		NAMES "ssl.h"
		PATHS "/usr/local/include/openssl" )
	
	find_library( OpenSSL_LIBRARY_DIR
		NAMES "ssl" "crypto"
		PATHS "/usr/local/lib"
			  "/usr/local/lib64" )
endif()

string(REPLACE "/openssl" "" OpenSSL_INCLUDE_DIR "${OpenSSL_INCLUDE_DIR}")

if( OpenSSL_INCLUDE_DIR AND EXISTS "${OpenSSL_INCLUDE_DIR}/openssl/opensslv.h" )
	file( STRINGS "${OpenSSL_INCLUDE_DIR}/openssl/opensslv.h"
		OpenSSL_VERSION_H REGEX "^# define[ \t]+OPENSSL_VERSION_TEXT[ \t]+\"[^\"]+\".*$" )
	string( REGEX REPLACE
		"^.*OPENSSL_VERSION_TEXT[ \t]+\"([^\"]+)\".*$" "\\1" OpenSSL_VERSION_STRING
		"${OpenSSL_VERSION_H}" )
endif()


MARK_AS_ADVANCED(
    OpenSSL_INCLUDE_DIR
  )
  
# handle the QUIETLY and REQUIRED arguments and set MYSQL_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( OpenSSL REQUIRED_VARS OpenSSL_INCLUDE_DIR OpenSSL_LIBRARY_DIR
	VERSION_VAR	OpenSSL_VERSION_STRING )

if(OpenSSL_FOUND)
    set( OpenSSL_INCLUDE_DIRS ${OpenSSL_INCLUDE_DIR} )
    set( OpenSSL_LIBRARY_DIRS ${OpenSSL_LIBRARY_DIR} )
  
    include_directories(${OpenSSL_INCLUDE_DIRS})
    link_directories(${OpenSSL_LIBRARY_DIR})
    
    if( WIN32 )
        set(OpenSSL_LIBRARIES libssl libcrypto)
    else()
        set(OpenSSL_LIBRARIES ssl crypto)
    endif()
  
    MESSAGE(STATUS "Found OpenSSL INCLUDE: ${OpenSSL_INCLUDE_DIRS} (found version \"${OpenSSL_VERSION_STRING}\")")
else()
    MESSAGE(FATAL_ERROR "无法找到OpenSSL开发环境，请先安装OpenSSL开发环境！")
endif()
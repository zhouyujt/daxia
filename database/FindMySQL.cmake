# - Try to find MySQL.
# Once done this will define:
# MYSQL_FOUND			- If false, do not try to use MySQL.
# MYSQL_INCLUDE_DIRS	- Where to find mysql.h, etc.
# MYSQL_LIBRARIES		- The libraries to link against.
# MYSQL_VERSION_STRING	- Version in a string of MySQL.
#
# Created by RenatoUtsch based on eAthena implementation.
#
# Please note that this module only supports Windows and Linux officially, but
# should work on all UNIX-like operational systems too.
#

#=============================================================================
# Copyright 2012 RenatoUtsch
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if( WIN32 )
	find_path( MYSQL_INCLUDE_DIR
		NAMES "mysql.h"
		PATHS "$ENV{PROGRAMFILES}/MySQL/*/include"
			  "$ENV{PROGRAMFILES\(x86\)}/MySQL/*/include"
			  "$ENV{SYSTEMDRIVE}/MySQL/*/include" 
			  "D:/vm_sharefile/mysql-5.7.25-winx64/include" )
	
	find_library( MYSQL_LIBRARY
		NAMES "mysqlclient" "mysqlclient_r"
		PATHS "$ENV{PROGRAMFILES}/MySQL/*/lib"
			  "$ENV{PROGRAMFILES\(x86\)}/MySQL/*/lib"
			  "$ENV{SYSTEMDRIVE}/MySQL/*/lib" )
else()
	find_path( MYSQL_INCLUDE_DIR
		NAMES "mysql.h"
		PATHS "/usr/include/mysql"
			  "/usr/app/mysql/include" )
	
	find_library( MYSQL_LIBRARY
		NAMES "mysqlclient" "mysqlclient_r"
		PATHS "/usr/lib/mysql"
			  "/usr/app/mysql/lib" )
endif()


if( MYSQL_INCLUDE_DIR AND EXISTS "${MYSQL_INCLUDE_DIR}/mysql_version.h" )
	file( STRINGS "${MYSQL_INCLUDE_DIR}/mysql_version.h"
		MYSQL_VERSION_H REGEX "^#define[ \t]+MYSQL_SERVER_VERSION[ \t]+\"[^\"]+\".*$" )
	string( REGEX REPLACE
		"^.*MYSQL_SERVER_VERSION[ \t]+\"([^\"]+)\".*$" "\\1" MYSQL_VERSION_STRING
		"${MYSQL_VERSION_H}" )
endif()


MARK_AS_ADVANCED(
  MYSQL_INCLUDE_DIR
  )
  
# handle the QUIETLY and REQUIRED arguments and set MYSQL_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MYSQL REQUIRED_VARS MYSQL_LIBRARY MYSQL_INCLUDE_DIR
	VERSION_VAR	MYSQL_VERSION_STRING )

if(MYSQL_FOUND)
  set( MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR} )
  set( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
  
  include_directories(${MYSQL_INCLUDE_DIRS})
  link_directories(${MYSQL_LIBRARY})

  MESSAGE(STATUS "Found MYSQL INCLUDE: ${MYSQL_INCLUDE_DIRS} (found version \"${MYSQL_VERSION_STRING}\")")
else()
  MESSAGE(FATAL_ERROR "无法找到MySQL开发环境，请先安装MySQL开发环境！")
endif()
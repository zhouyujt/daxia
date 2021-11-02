set_property(GLOBAL PROPERTY USE_FOLDERS ON)
cmake_minimum_required(VERSION 3.21)
project(daxia VERSION 1.0.0)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(IS64 false)	
if(MSVC)
	if(CMAKE_CL_64)
		set(IS64 true)	
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P)
		set(IS64 true)
	endif()
endif()

if(IS64)
	if(MSVC AND MT STREQUAL 1)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debugMT)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/releaseMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debugMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/releaseMT)
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debug)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/release)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/debug)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/x64/release)
	endif()
else()
	if(MSVC AND MT STREQUAL 1)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debugMT)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/releaseMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debugMT)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/releaseMT)
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debug)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/release)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${CMAKE_CURRENT_SOURCE_DIR}/libs/debug)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE	${CMAKE_CURRENT_SOURCE_DIR}/libs/release)
	endif()
endif()

if(MSVC AND MT STREQUAL 1)
    add_compile_options(
        $<$<CONFIG:>:/MT> #----------|
        $<$<CONFIG:Debug>:/MTd>#--|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})
#find_package(MySQL)
#include_directories(${MySQL_INCLUDE_DIRS})

add_subdirectory(net)
add_subdirectory(encode)
add_subdirectory(reflect)
add_subdirectory(database)
add_subdirectory(system)

# 安装发行库文件
install(
	TARGETS net encode reflect database system zlib sqlite3
	EXPORT daxia
	CONFIGURATIONS Release
    LIBRARY DESTINATION lib/daxia/release  # 动态库安装路径
    ARCHIVE DESTINATION lib/daxia/release  # 静态库安装路径
    RUNTIME DESTINATION bin/daxia/release  # 可执行文件安装路径
)

# 安装调试库文件
install(
	TARGETS net encode reflect database system zlib sqlite3
	CONFIGURATIONS Debug
    LIBRARY DESTINATION lib/daxia/debug  # 动态库安装路径
    ARCHIVE DESTINATION lib/daxia/debug  # 静态库安装路径
    RUNTIME DESTINATION bin/daxia/debug  # 可执行文件安装路径
)

# 安装源文件
install(
	DIRECTORY database encode include net reflect system 
	DESTINATION include/github.com/zhouyujt/daxia
	PATTERN "CMakeLists.txt" EXCLUDE
)

# 安装源文件
install(
	FILES defer.hpp singleton.hpp string.hpp
	DESTINATION include/github.com/zhouyujt/daxia
)

# 安装导出库
install(
	EXPORT daxia
    FILE daxiaTargets.cmake
	NAMESPACE daxia::
    DESTINATION lib/cmake/daxia-${PROJECT_VERSION}
)

# 生成version文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	daxiaConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion  # 表示该函数库向下兼容
)

# 生成配置文件
configure_file(daxiaConfig.cmake.in daxiaConfig.cmake @ONLY)

# 安装version文件及配置文件
install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/daxiaConfigVersion.cmake" "${CMAKE_CURRENT_BINARY_DIR}/daxiaConfig.cmake" 
    DESTINATION lib/cmake/daxia-${PROJECT_VERSION}
)



								
								
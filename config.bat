@echo off
echo /*
echo  *            daxia 库 
echo  *           版本: 1.0.0
echo  *
echo  * 依赖库:（请自行前往官网下载安装）
echo  * boost 1.76	https://www.boost.org/
echo  * openssl 1.1.1 https://www.openssl.org/
echo  *
echo  */

echo.

rem ///////////////////////////////////////////////////////////
rem // 设置VS版本
rem ///////////////////////////////////////////////////////////
:INPUT1
set /p input=-- (1) 指定Visual Studio版本(2013/2015/2017/2019/2022):
if %input% equ 2013 set vs=Visual Studio 12 2013
if %input% equ 2015 set vs=Visual Studio 14 2015
if %input% equ 2017 set vs=Visual Studio 15 2017
if %input% equ 2019 set vs=Visual Studio 16 2019
if %input% equ 2022 set vs=Visual Studio 17 2022
if %input% neq 2013 if %input% neq 2015 if %input% neq 2017 if %input% neq 2019 if %input% neq 2022 goto ERROR1

rem ///////////////////////////////////////////////////////////
rem // 设置运行时库
rem ///////////////////////////////////////////////////////////
:INPUT2
set /p input=-- (2) 选择运行库(MT/MD)：
if /i "%input%" == "mt" set runtimelib=-DMT=1
if /i "%input%" == "md" set runtimelib=-DMT=0
if "%input%" neq "mt" if "%input%" neq "md" goto ERROR2

rem ///////////////////////////////////////////////////////////
rem // 设置是否支持MySQL
rem ///////////////////////////////////////////////////////////
:INPUT3
set /p input=-- (3) orm库是否需要支持MySQL(Y/N):
if /i "%input%" == "y" (set mysql=-DORM_MYSQL=1) else set mysql=-DORM_MYSQL=0

rem ///////////////////////////////////////////////////////////
rem // 设置是否支持sqlite3
rem ///////////////////////////////////////////////////////////
:INPUT4
set /p input=-- (4) orm库是否需要支持sqlite3(Y/N):
if /i "%input%" == "y" (set sqlite3=-DORM_SQLITE=1)  else set sqlite3=-DORM_SQLITE=0

rem ///////////////////////////////////////////////////////////
rem // 设置是否支持HTTPS
rem ///////////////////////////////////////////////////////////
:INPUT5
set /p ssl=-- (5) net库是否需要支持HTTPS(Y/N):
goto EXIT

:ERROR1
echo --
echo -- 输入错误,无法识别的Visual Studio版本,请重新输入!
echo --
goto INPUT1

:ERROR2
echo --
echo -- 输入错误,无法识别的运行时库,请重新输入!
echo --
goto INPUT2

:EXIT 
rem // 创建文件夹
set floder="%vs%" 
mkdir %floder% >nul 2>nul
echo.
echo -- %floder% 在此文件夹中生成

rem // 生成
set cmakecmd=cmake -G"%vs%" %runtimelib% %mysql% %sqlite3% ../
echo -- %cmakecmd%
echo.
cd %floder%
cmd /c %cmakecmd% ../
cd ..

rem // 创建文件夹
set floder64="%vs% Win64"
mkdir %floder64% >nul 2>nul
echo.
echo -- %floder64% 在此文件夹中生成

rem // 生成x64
set cmakecmd=cmake -G"%vs% Win64" %runtimelib% %mysql% %sqlite3% ../
echo -- %cmakecmd%
echo.
cd %floder64%
cmd /c %cmakecmd% ../
cd ..

pause
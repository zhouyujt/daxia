@echo off
rd /S /Q build_mt >nul 2>nul
mkdir build_mt
cd build_mt

cmake -G "Visual Studio 12 2013" -DORM_MYSQL=0 -DORM_SQLITE=1 -DMT=1 ../

mkdir x64
cd x64
cmake -G "Visual Studio 12 2013 Win64" -DORM_MYSQL=0 -DORM_SQLITE=1 -DMT=1 ../../

pause
@echo off
cd build

cmake -G "Visual Studio 12 2013" -DORM_MYSQL=1 -DORM_SQLITE=0 ../

cd x64
cmake -G "Visual Studio 12 2013 Win64" -DORM_MYSQL=1 -DORM_SQLITE=0 ../../

pause
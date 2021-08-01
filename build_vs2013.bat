@echo off
cd build

cmake -G "Visual Studio 12 2013" ../

cd x64
cmake -G "Visual Studio 12 2013 Win64" ../../

pause
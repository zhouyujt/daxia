cd build
mkdir "Visual Studio 9 2008\x64"
mkdir "Visual Studio 10 2010\x64"
mkdir "Visual Studio 11 2012\x64"
mkdir "Visual Studio 12 2013\x64"
mkdir "Visual Studio 14 2015\x64"
mkdir "Visual Studio 15 2017\x64"
mkdir "Visual Studio 16 2019\x64"
mkdir "Visual Studio 17 2022\x64"
cd "Visual Studio 9 2008"
cmake -G "Visual Studio 9 2008" ../../
cmake  ../../

cd x64
cmake -G "Visual Studio 9 2008 Win64" ../../../
cmake  ../../../

cd "../../Visual Studio 10 2010"
cmake -G "Visual Studio 10 2010" ../../
cmake  ../../

cd x64
cmake -G "Visual Studio 10 2010 Win64" ../../../
cmake  ../../../

cd "../../Visual Studio 11 2012"
cmake -G "Visual Studio 11 2012" ../../
cmake  ../../

cd x64
cmake -G "Visual Studio 11 2012 Win64" ../../../
cmake  ../../../

cd "../../Visual Studio 12 2013"
cmake -G "Visual Studio 12 2013" ../../
cmake  ../../

cd x64
cmake -G "Visual Studio 12 2013 Win64" ../../../
cmake  ../../../

pause
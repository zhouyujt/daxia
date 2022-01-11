#!/bin/bash
echo /\*
echo " "\* daxia 库
echo " "\* 版本: 1.1.0
echo " "\*
echo " "\* 依赖库:（请自行前往官网下载安装）
echo " "\* boost 1.76 https://www.boost.org
echo " "\* openssl 1.1.1 https://www.openssl.org
echo " "\*
echo " "\*/

echo

############################################################
# 选择生成调试版还是发布版
############################################################
while true; do
    read -p "-- (1) 选择生成调试版还是发布版(DEBUG/RELEASE):" input
    temp=$(echo $input | tr [A-Z] [a-z])
    if [ $temp == "debug" ] || [ $temp == "d" ]; then
        buildtype="Debug"
        break
    elif [ $temp == "release" ] || [ $temp == "r" ]; then
        buildtype="Release"
        break
    else
        echo "输入错误,请重新输入!"
    fi
done

############################################################
# 设置是否支持MySQL
############################################################
read -p "-- (2) orm库是否需要支持MySQL(Y/N):" input
temp=$(echo $input | tr [A-Z] [a-z])
if [ $temp == "y" ]; then
    mysql=-DORM_MYSQL=1
else
    mysql=-DORM_MYSQL=0
fi

############################################################
# 设置是否支持sqlite3
############################################################
read -p "-- (3) orm库是否需要支持sqlite3(Y/N):" input
temp=$(echo $input | tr [A-Z] [a-z])
if [ $temp == "y" ]; then
    sqlite3=-DORM_SQLITE=1
else
    sqlite3=-DORM_SQLITE=0
fi

############################################################
# 设置是否支持HTTPS
############################################################
read -p "-- (4) net库是否需要支持HTTPS(Y/N):" input
temp=$(echo $input | tr [A-Z] [a-z])
if [ $temp == "y" ]; then
    https=-DNET_HTTPS=1
else
    https=-DNET_HTTPS=0
fi

# 创建文件夹
if [ $buildtype == "Debug" ]; then
    floder="build_debug"
else
    floder="build_release"
fi
mkdir $floder 2> /dev/null
echo
echo  "-- $floder 在此文件夹中生成"

# 生成
cmakecmd="cmake -G\"Unix Makefiles\" -DCMAKE_BUILD_TYPE=$buildtype $mysql $sqlite3 $https ../"
echo "-- $cmakecmd"
echo
cd $floder
cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=$buildtype $mysql $sqlite3 $https ../

# getch()
# {
#     savedstty=`stty -g`
#     stty -echo
#     stty raw
#     dd if=/dev/tty bs=1 count=1 2> /dev/null
#     stty -raw
#     stty echo
#     stty $savedstty
# }

# echo "请按任意键继续..."
# getch
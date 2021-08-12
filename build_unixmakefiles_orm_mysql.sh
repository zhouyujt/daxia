#!/bin/bash
cd build
cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DORM_MYSQL=1 -DORM_SQLITE=0 ../
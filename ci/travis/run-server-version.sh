#!/usr/bin/env sh

cmake CMakeLists.txt -DBUILD_OPTIMIZER=1 -DBUILD_FORECASTER=1 -DUSE_GUI=0 -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j$(nproc)

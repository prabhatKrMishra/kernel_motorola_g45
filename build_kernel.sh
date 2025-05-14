#!/bin/bash

export SKIP_MRPROPER=1
export SKIP_DEFCONFIG=0
export TARGET_BUILD_VARIANT=user

BUILD_CONFIG=kernel/msm-5.4/build.config.msm.holi VARIANT=qgki LTO=thin TARGET_PRODUCT=fogos BUILD_KERNEL=1 build/build.sh -j$(($(nproc) - 1))
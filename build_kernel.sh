#!/bin/bash

ROOT_PATH=$(pwd)
OUTPUT_DIRECTORY_PATH=$ROOT_PATH/out

#============== Build specific variables ==============#
BUILD_CONFIG="kernel/msm-5.4/build.config.msm.holi"
BUILD_VARIANT="qgki"
BUILD_TARGET_PRODUCT="fogos"
CLANG_LTO_TYPE="thin"

handle_arguments() {
    case "$1" in
        --new)
            SKIP_MRPROPER=0
            SKIP_DEFCONFIG=0
            rm -rf $OUTPUT_DIRECTORY_PATH
            mkdir -p $OUTPUT_DIRECTORY_PATH
            ;;
        --rebuild)
            SKIP_MRPROPER=1
            SKIP_DEFCONFIG=0
            ;;
        *)
            echo "Invalid option: $1"
            echo "Usage: $0 --new | --rebuild"
            exit 1
            ;;
    esac
}

handle_arguments "$1"

if [ "$SKIP_MRPROPER" -eq 1 ]; then
    export SKIP_MRPROPER=1
fi

if [ "$SKIP_DEFCONFIG" -eq 1 ]; then
    export SKIP_DEFCONFIG=1
fi

export TARGET_BUILD_VARIANT=user
export KERNEL_BUILD_MODE=user
export ARCH=arm64

BUILD_CONFIG=$BUILD_CONFIG VARIANT=$BUILD_VARIANT LTO=$CLANG_LTO_TYPE TARGET_PRODUCT=$BUILD_TARGET_PRODUCT BUILD_KERNEL=1 build/build.sh -j$(($(nproc) - 1))

copy_binaries() {
	echo " Copying kernel binaries"
	KERNEL_BINARY_DIR=$OUTPUT_DIRECTORY_PATH/msm-5.4-holi-qgki/kernel/msm-5.4/arch/arm64/boot
	KERNEL_DTB_DIR=$OUTPUT_DIRECTORY_PATH/msm-5.4-holi-qgki/kernel/msm-5.4/arch/arm64/boot/dts/vendor/qcom

	cp "$KERNEL_BINARY_DIR/Image" "$OUTPUT_DIRECTORY_PATH"
	cp "$KERNEL_BINARY_DIR/Image.gz" "$OUTPUT_DIRECTORY_PATH"
	echo " KERNEL binary file is ready in out"
	cp "$KERNEL_DTB_DIR/blair-moto-fogos-base.dtb" "$OUTPUT_DIRECTORY_PATH/blair-moto-fogos-base.dtb"
	echo " DTB binary file is ready in out"
	cp "$KERNEL_DTB_DIR/blair-fogos-dvt1-overlay.dtbo" "$OUTPUT_DIRECTORY_PATH/blair-fogos-dvt1-overlay.dtbo"
	cp "$KERNEL_DTB_DIR/blair-fogos-evt-overlay.dtbo" "$OUTPUT_DIRECTORY_PATH/blair-fogos-evt-overlay.dtbo"
	echo " DTBO binary files are ready in out"
}

# Check if 'arch/arm64/boot/Image.gz' exists
if [ -f $OUTPUT_DIRECTORY_PATH/msm-5.4-holi-qgki/kernel/msm-5.4/arch/arm64/boot/Image.gz ]; then
    echo ''
    echo " Kernel build successful! "
    echo ''
    copy_binaries
    echo ''
else
    echo ''
    echo " Kernel build failed !"
    echo ''
fi
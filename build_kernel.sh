#!/bin/bash

ROOT_PATH=$(pwd)
OUTPUT_DIRECTORY_PATH=$ROOT_PATH/out

#============== Build specific variables ==============#
BUILD_CONFIG="kernel/msm-5.4/build.config.msm.holi"
BUILD_VARIANT="qgki"
BUILD_TARGET_PRODUCT="fogos"
CLANG_LTO_TYPE="thin"

#============== Argument Handler ==============#
CLEAR_CCACHE=0

handle_arguments() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --new)
                SKIP_MRPROPER=0
                SKIP_DEFCONFIG=0
                rm -rf "$OUTPUT_DIRECTORY_PATH"
                mkdir -p "$OUTPUT_DIRECTORY_PATH"
                ;;
            --rebuild)
                SKIP_MRPROPER=1
                SKIP_DEFCONFIG=0
                ;;
            -c|--clear-cache)
                CLEAR_CCACHE=1
                ;;
            *)
                echo "Invalid option: $1"
                echo "Usage: $0 [--new | --rebuild] [-c | --clear-cache]"
                exit 1
                ;;
        esac
        shift
    done
}

handle_arguments "$@"

if [ "$SKIP_MRPROPER" -eq 1 ]; then
    export SKIP_MRPROPER=1
fi

if [ "$SKIP_DEFCONFIG" -eq 1 ]; then
    export SKIP_DEFCONFIG=1
fi

export TARGET_BUILD_VARIANT=user
export KERNEL_BUILD_MODE=user
export ARCH=arm64
export SUBARCH=arm64
export HEADER_ARCH=arm64

#============== LLVM TOOLCHAIN ==============#
# AOSP clang v20.0.0
export LLVM_DIR=$ROOT_PATH/prebuilts-master/clang/host/linux-x86/clang-r547379/bin

# Qualcomm® Snapdragon™ LLVM 19.0.0
# export LLVM_DIR=$ROOT_PATH/sdclang-19.0.0/bin

#============== CCACHE CONFIGURATION ==============#
export PATH="/usr/lib/ccache:$PATH"
export SOURCE_DATE_EPOCH=1717300000

export CCACHE_DIR="$HOME/.ccache"
export CCACHE_BASEDIR="$ROOT_PATH"
export CCACHE_CPP2=yes
export CCACHE_COMPILERCHECK=content

# Configure caching behavior
ccache --set-config sloppiness=time_macros,include_file_mtime,include_file_ctime

# Optional clear if requested
if [ "$CLEAR_CCACHE" -eq 1 ]; then
    echo ">>> Clearing ccache..."
    ccache -C
    echo ">>> ccache cleared successfully."
fi

# Clean stale cache entries
ccache -c

# Show current settings
ccache -p | grep -E "base|cpp2|compilercheck|sloppiness"

#============== WRAP CLANG WITH CCACHE ==============#
cat > "$ROOT_PATH/ccache-clang" <<EOF
#!/bin/bash
exec ccache "$LLVM_DIR/clang" "\$@"
EOF
chmod +x "$ROOT_PATH/ccache-clang"

#============== BUILD ==============#
time BUILD_CONFIG=$BUILD_CONFIG VARIANT=$BUILD_VARIANT LTO=$CLANG_LTO_TYPE TARGET_PRODUCT=$BUILD_TARGET_PRODUCT BUILD_KERNEL=1 build/build.sh \
  CC=$ROOT_PATH/ccache-clang \
  LD=${LLVM_DIR}/ld.lld \
  AR=${LLVM_DIR}/llvm-ar \
  NM=${LLVM_DIR}/llvm-nm \
  AS=${LLVM_DIR}/llvm-as \
  OBJCOPY=${LLVM_DIR}/llvm-objcopy \
  OBJDUMP=${LLVM_DIR}/llvm-objdump \
  READELF=${LLVM_DIR}/llvm-readelf \
  OBJSIZE=${LLVM_DIR}/llvm-size \
  STRIP=${LLVM_DIR}/llvm-strip \
  LLVM_AR=${LLVM_DIR}/llvm-ar \
  LLVM_NM=${LLVM_DIR}/llvm-nm \
  LLVM=1 \
  LLVM_IAS=1 \
  -j$(($(nproc) - 1))

# Cleanup wrapper
rm -f "$ROOT_PATH/ccache-clang"

#============== COPY BINARIES ==============#
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

#============== BUILD RESULT ==============#
if [ -f "$OUTPUT_DIRECTORY_PATH/msm-5.4-holi-qgki/kernel/msm-5.4/arch/arm64/boot/Image.gz" ]; then
    echo ''
    echo " Kernel build successful! "
    echo ''
    copy_binaries
    echo ''
else
    echo ''
    echo " Kernel build failed!"
    echo ''
fi


PLATFORM_BOOT=$(pwd)

SRC_DIR=${PLATFORM_BOOT}
DST_DIR=${PROGRAM_SDK_PATH}/qemu/qemu-11.0.0

cp -rv ${SRC_DIR}/hw/* ${DST_DIR}/hw/

cp -rv ${SRC_DIR}/include/* ${DST_DIR}/include/

cp -rv ${SRC_DIR}/monitor/* ${DST_DIR}/monitor/

cp -rv *.hx ${DST_DIR}/
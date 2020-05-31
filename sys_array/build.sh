#!/bin/bash

set -e
# echo '###GCC version info###'
# gcc --version
# echo '###GCC version info end###'
ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=src/custom.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

# if [ -z "${CROSS_COMPILE+xxx}" ]; then
# case $(uname -p) in
# 	arm*)
# 		# Use native arm compiler (no cross prefix)
# 		CROSS_COMPILE=
# 		;;
# 	   *)
# 		# Use cross compiler
# 		CROSS_COMPILE="arm-linux-gnueabihf-"
# 		;;
# esac
# fi

# Build all 
gcc -O2 -std=c99 src/sys_array_host.c -o bin/main ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread -lm
e-gcc -Os -std=c99 -falign-loops=8 -falign-functions=8 -Wall  -T ${ELDF} src/static_buffers.c src/e_inner.c -o bin/e_inner.elf -le-lib -lm -g
e-gcc -Os -std=c99 -falign-loops=8 -falign-functions=8 -Wall  -T ${ELDF} src/static_buffers.c src/e_boundary.c -o bin/e_boundary.elf -le-lib -lm -g
e-gcc -Os -std=c99 -falign-loops=8 -falign-functions=8 -Wall  -T ${ELDF} src/static_buffers.c src/e_dummy.c -o bin/e_dummy.elf -le-lib -lm -g
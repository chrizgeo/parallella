#!/bin/bash

set -e
# echo '###GCC version info###'
# gcc --version
# echo '###GCC version info end###'
ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf

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
gcc -O2 -std=c99 src/main.c src/seq_main.c src/matrix.c src/givens_qr.c -o bin/main ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread -lm
#e-gcc -Os -std=c99 -falign-loops=8 -falign-functions=8 -Wall  -T ${ELDF} src/e_seq.c -o bin/e_seq.elf -le-lib -lm

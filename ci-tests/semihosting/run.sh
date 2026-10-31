#!/usr/bin/env bash
# Build semihosting_test.c for RV32 and RV64 and run both on spike.
# Usage: run.sh [path/to/spike]
set -eu

ROOT=`git rev-parse --show-toplevel`
HERE=`pwd`
CI="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
INSTALL=$HERE/install
BUILD=$HERE/build
RUN=$HERE/run

SPIKE="${1:-${SPIKE:-$INSTALL/bin/spike}}"
CC="${CC:-riscv%d-unknown-elf-gcc}"

# build tests
rm -rf $RUN
mkdir -p $RUN
cd $RUN

run() {
  local xlen=$1 arch=$2 abi=$3
  local cc=$(printf "$CC" $xlen)
  local elf="semihosting_test$xlen.elf"
  "$cc" -Oz -march="$arch" -mabi="$abi" --specs=semihost.specs \
    -o "$elf" "$CI/semihosting_test.c"
  echo "== RV$xlen =="
  "$SPIKE" --isa="rv${xlen}imafdc" --disable-dtb --no-dm \
    -m0:0x100000000,0xffffffffffff0000:0x10000 "$elf"
}

run 32 rv32gc_zicsr ilp32d
run 64 rv64gc_zicsr lp64d
echo "ALL PASSED"

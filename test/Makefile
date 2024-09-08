TARGET := hello
TARGET := rot13

# RISCV := /opt/riscv
# riscv-collab requires Ubuntu:2204 libboost_regex.so.1.7
# RISCV := /opt/riscv-collab/riscv
# RISCV := /opt/riscv32-embecosm-macos-gcc13.2.0
# RISCV := /opt/riscv32-embecosm-ubuntu2204-gcc13.2.0
RISCV := /opt/riscv64-embecosm-linux-gcc-ubuntu2204-20240407

export PATH := $(RISCV)/bin:$(PATH)

ifeq ($(RISCV),/opt/riscv)
prefix := riscv64-unknown-elf
#prefix := riscv64-unknown-linux-gnu
else ifeq ($(RISCV),/opt/riscv-collab/riscv)
prefix := riscv64-unknown-elf
rv64 := 1
else ifeq ($(RISCV),/opt/riscv32-embecosm-macos-gcc13.2.0)
prefix := riscv32-unknown-elf
else ifeq ($(RISCV),/opt/riscv32-embecosm-ubuntu2204-gcc13.2.0)
prefix := riscv32-unknown-elf
else ifeq ($(RISCV),/opt/riscv64-embecosm-linux-gcc-ubuntu2204-20240407)
prefix := riscv64-unknown-linux-gnu
rv64 := 1
else
$(error "Please set RISCV to the base of your RISC-V installation")
endif
$(info $(RISCV):$(prefix))

# prefix := riscv64-elf
# prefix := riscv64-unknown-elf
# prefix := riscv64-embecosm-linux

ifdef prefix
CC = ${prefix}-gcc
objdump = ${prefix}-objdump
gdb = ${prefix}-gdb
endif

CFLAGS = -g -Og

ifndef rv64
CFLAGS += -march=rv32imc_zicsr -mabi=ilp32
#CFLAGS += -march=rv32imc_zicsr -mabi=ilp32
#LDFLAGS += -march=rv32ia -mabi=ilp32
SPIKE_FLAGS  = --isa=rv32imc_zicsr_zifencei
#SPIKE_FLAGS  = --isa=rv32imc_zicsr_zifencei_zba_zbb_zbc_zbs_zcb_zcmp_zcmt_zicond
else
LDFLAGS = -static
# default
# CFLAGS += -march=rv64imafdc -mabi=lp64d
# CFLAGS += -march=rv64imafd -mabi=lp64d
# CFLAGS += -march=rv64ic -mabi=lp64
# SPIKE_FLAGS = --isa=rv64imafdc
# SPIKE_FLAGS = --isa=rv64ima
#PK = pk
endif
PK = $(RISCV)/${prefix}/bin/pk #-p -s
SPIKE_FLAGS += --rbb-port=9824

all: $(TARGET)

spike: $(TARGET)
	spike $(SPIKE_FLAGS) $(PK) $<
#	spike --rbb-port=9824 -m0x10100000:0x20000 $<

openocd:
	/usr/bin/openocd -f spike.cfg

gdb_flags = -x run.gdb
gdb: $(TARGET)
	${gdb} $(gdb_flags) $<

%.dmp : %
	${objdump} -D $< > $@

# original in riscv-isa-sim
rot13-64: rot13.c
	$(CC) $(CFLAGS) -o rot13-64.o -c rot13.c
	$(CC) $(CFLAGS) -T spike.lds -nostartfiles -o rot13-64 rot13-64.o

dts_dump: $(TARGET)
	spike $(SPIKE_FLAGS) --dump-dts $(PK) $<

clean:
	rm -f *.o $(TARGET) hello rot13 rot13-64 *.dmp

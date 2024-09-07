TARGET = hello
PATH := /opt/riscv/bin:$(PATH)

prefix := riscv64-unknown-elf-
prefix := riscv64-elf-
CC = ${prefix}gcc
objdump = ${prefix}objdump
gdb = ${prefix}gdb

CFLAGS = -g -Og

#rv32 := 1
ifdef rv32
CFLAGS += -march=rv32imc_zicsr -mabi=ilp32
#LDFLAGS += -march=rv32ia -mabi=ilp32
SPIKE_FLAGS  = --isa=rv32imc_zicsr_zifencei
#SPIKE_FLAGS  = --isa=rv32imc_zicsr_zifencei_zba_zbb_zbc_zbs_zcb_zcmp_zcmt_zicond
PK = /opt/riscv/riscv32-unknown-elf/bin/pk #-p -s
else
# default
# CFLAGS += -march=rv64imafdc -mabi=lp64d
# CFLAGS += -march=rv64imafd -mabi=lp64d
# CFLAGS += -march=rv64ic -mabi=lp64
# SPIKE_FLAGS = --isa=rv64imafdc
# SPIKE_FLAGS = --isa=rv64ima
PK = pk
endif
SPIKE_FLAGS += --rbb-port=9824

# original in riscv-isa-sim
rot13-64: rot13.c
	$(CC) $(CFLAGS) -o rot13-64.o -c rot13.c
	$(CC) $(CFLAGS) -T spike.lds -nostartfiles -o rot13-64 rot13-64.o

spike: $(TARGET)
	spike $(SPIKE_FLAGS) $(PK) $<
#	spike --rbb-port=9824 -m0x10100000:0x20000 $<

dts_dump: $(TARGET)
	spike $(SPIKE_FLAGS) --dump-dts $(PK) $<

openocd:
	openocd -f spike.cfg

gdb_flags = -x run.gdb
gdb: $(TARGET)
	${gdb} $(gdb_flags) $<

# rot13: rot13.o
# 	$(CC) $(CFLAGS) -T spike.lds -nostartfiles -o $@ $<

%.dmp : %
	${objdump} -D $< > $@
clean:
	rm -f *.o $(TARGET) hello rot13 rot13-64 *.dmp

# rv32 spike debug

spike fails with hello world compiled with -march=rv32imac

spike works with hello world compiled with `-march=rv32i`, but spike fails with hello-world compiled with `-march=rv32imac`.
What am I doing wrong?

## gcc

gcc 12.2.0, latest homebrew version on macOS

```sh
$ riscv64-unknown-elf-gcc -v
Using built-in specs.
COLLECT_GCC=riscv64-unknown-elf-gcc
COLLECT_LTO_WRAPPER=/opt/homebrew/Cellar/riscv-gnu-toolchain/main/libexec/gcc/riscv64-unknown-elf/12.2.0/lto-wrapper
Target: riscv64-unknown-elf
Configured with: /private/tmp/riscv-gnu-toolchain-20230223-34216-18jz0sg/gcc/configure --target=riscv64-unknown-elf --prefix=/opt/homebrew/Cellar/riscv-gnu-toolchain/main --disable-shared --disable-threads --enable-languages=c,c++ --with-pkgversion=g2ee5e430018-dirty --with-system-zlib --enable-tls --with-newlib --with-sysroot=/opt/homebrew/Cellar/riscv-gnu-toolchain/main/riscv64-unknown-elf --with-native-system-header-dir=/include --disable-libmudflap --disable-libssp --disable-libquadmath --disable-libgomp --disable-nls --disable-tm-clone-registry --src=/private/tmp/riscv-gnu-toolchain-20230223-34216-18jz0sg/gcc --enable-multilib --with-abi=lp64d --with-arch=rv64imafdc --with-tune=rocket --with-isa-spec=2.2 'CFLAGS_FOR_TARGET=-Os   -mcmodel=medany' 'CXXFLAGS_FOR_TARGET=-Os   -mcmodel=medany'
Thread model: single
Supported LTO compression algorithms: zlib
```

## spike and pk

I built the latest spike and pk from GitHub.

- https://github.com/riscv-software-src/riscv-isa-sim
- https://github.com/riscv-software-src/riscv-pk


### `spike` build

```sh
$ mkdir build
$ cd build
$ ../configure --prefix=/opt/riscv
$ make install
```

### `pk` build

```sh
$ mkdir build
$ cd build
$ ../configure --prefix=/opt/riscv --host=riscv64-unknown-elf --with-arch=rv32i
$ make install
```

## hello world with `-march=rv32i`

```sh
$ riscv64-unknown-elf-gcc -g -O -march=rv32i -mabi=ilp32    hello.c   -o hello
$ spike --isa=rv32imac /opt/riscv/riscv32-unknown-elf/bin/pk hello
bbl loader
hello world!
```

```sh
$ riscv64-unknown-elf-gcc -g -O -march=rv32im -mabi=ilp32    hello.c   -o hello
$ spike --isa=rv32imac /opt/riscv/riscv32-unknown-elf/bin/pk hello
bbl loader
hello world!
```

Works fine.

## hello world with `-march=rv32imac`

```sh
$ riscv64-unknown-elf-gcc -g -O -march=rv32imac -mabi=ilp32    hello.c   -o hello
$ spike --isa=rv32imac /opt/riscv/riscv32-unknown-elf/bin/pk hello
bbl loader
z  00000000 ra 8000452c sp 80216cd0 gp 00000000
tp 00000000 t0 8000d184 t1 80216e14 t2 00000000
s0 80216cd1 s1 8001405c a0 0000002e a1 00000000
a2 0000003f a3 10000000 a4 10000005 a5 00000000
a6 00000000 a7 00000000 s2 80014064 s3 80014068
s4 80012000 s5 8000f2f4 s6 00000001 s7 00000000
s8 00000000 s9 8001229e sA 80012260 sB 00000001
t3 80216e4c t4 00000000 t5 00000003 t6 00000000
pc 80005624 va/inst 10000005 sr 80006100
Kernel load segfault @ 0x10000005
```

Segfault on load from 0x10000005 at 0x80005624.

```sh
$ riscv64-unknown-elf-objdump -D /opt/riscv/riscv32-unknown-elf/bin/pk
...
80005620 <uart16550_getchar>:
80005620:	0000f697          	auipc	a3,0xf
80005624:	a406a683          	lw	a3,-1472(a3) # 80014060 <uart16550_reg_shift>  <- fails here!!!
80005628:	00500793          	li	a5,5
...
```

### `uart16550.c`

```c
#define UART_REG_LSR       5    // line status register
...
int uart16550_getchar()
{
  if (uart16550[UART_REG_LSR << uart16550_reg_shift] & UART_REG_STATUS_RX)
    return uart16550[UART_REG_QUEUE << uart16550_reg_shift];
  return -1;
}
```

load segfault causes on access to `UART_REG_LSR`.

```sh
$ spike --isa=rv32imac --dump-dts /opt/riscv/riscv32-unknown-elf/bin/pk hello
    SERIAL0: ns16550@10000000 {
      compatible = "ns16550a";
      clock-frequency = <10000000>;
      interrupt-parent = <&PLIC>;
      interrupts = <1>;
      reg = <0x0 0x10000000 0x0 0x100>;
      reg-shift = <0x0>;
      reg-io-width = <0x1>;
    };
...
```

```sh
riscv64-unknown-elf-gcc -g -O -march=rv32imc -mabi=ilp32    hello.c   -o hello
spike --isa=rv32imac /opt/riscv/riscv32-unknown-elf/bin/pk hello
bbl loader
z  00000000 ra 80004514 sp 80216cd0 gp 00000000
tp 00000000 t0 8000d16c t1 80216e14 t2 00000000
s0 80216cd1 s1 8001405c a0 0000002e a1 00000000
a2 0000003f a3 10000000 a4 10000005 a5 00000000
a6 00000000 a7 00000000 s2 80014064 s3 80014068
s4 80012000 s5 8000f2dc s6 00000001 s7 00000000
s8 00000000 s9 8001229e sA 80012260 sB 00000001
t3 80216e4c t4 00000000 t5 00000003 t6 00000000
pc 8000560c va/inst 10000005 sr 80006100
Kernel load segfault @ 0x10000005
```

### `uart.c`

```c
volatile uint32_t* uart;

void uart_putchar(uint8_t ch)
{
#ifdef __riscv_atomic
    int32_t r;
    do {
      __asm__ __volatile__ (
        "amoor.w %0, %2, %1\n"
        : "=r" (r), "+A" (uart[UART_REG_TXFIFO])
        : "r" (ch));
    } while (r < 0);
#else
    volatile uint32_t *tx = uart + UART_REG_TXFIFO;
    while ((int32_t)(*tx) < 0);
    *tx = ch;
#endif
}
```

rv32i

```sh
RISCV=/opt/riscv
../configure --prefix=/opt/riscv --host=riscv64-unknown-elf --with-arch=rv32i
```

```c
8000527c <uart_putchar>:
8000527c:	0000f717          	auipc	a4,0xf
80005280:	de072703          	lw	a4,-544(a4) # 8001405c <uart>
80005284:	00072783          	lw	a5,0(a4)
80005288:	fe07cee3          	bltz	a5,80005284 <uart_putchar+0x8>
8000528c:	00a72023          	sw	a0,0(a4)
80005290:	00008067          	ret
```

rv32imac

```sh
../configure --prefix=/opt/riscv --host=riscv64-unknown-elf --with-arch=rv32imac
```

```c
80003c9e <uart_putchar>:
80003c9e:	00013697          	auipc	a3,0x13
80003ca2:	3de68693          	add	a3,a3,990 # 8001707c <uart>
80003ca6:	429c                	lw	a5,0(a3)
80003ca8:	40a7a72f          	amoor.w	a4,a0,(a5)
80003cac:	fe074de3          	bltz	a4,80003ca6 <uart_putchar+0x8>
80003cb0:	8082                	ret
```

## `--march=rv32ima`, `--march=rv32ia`

```
/opt/homebrew/Cellar/riscv-gnu-toolchain/main/lib/gcc/riscv64-unknown-elf/12.2.0/../../../../riscv64-unknown-elf/bin/ld: /opt/homebrew/Cellar/riscv-gnu-toolchain/main/lib/gcc/riscv64-unknown-elf/12.2.0/../../../../riscv64-unknown-elf/lib/crt0.o: ABI is incompatible with that of the selected emulation:
  target emulation `elf64-littleriscv' does not match `elf32-littleriscv'
...
```

```sh
$ riscv64-unknown-elf-gcc --print-multi-lib
.;
rv32i/ilp32;@march=rv32i@mabi=ilp32
rv32im/ilp32;@march=rv32im@mabi=ilp32
rv32iac/ilp32;@march=rv32iac@mabi=ilp32
rv32imac/ilp32;@march=rv32imac@mabi=ilp32
rv32imafc/ilp32f;@march=rv32imafc@mabi=ilp32f
rv64imac/lp64;@march=rv64imac@mabi=lp64
```
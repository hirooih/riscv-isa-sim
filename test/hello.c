#include <stdio.h>
#include <string.h>
#include <unistd.h>

uint64_t
rdcycle ()
{
  uint32_t lo, hi1, hi2;
  // cf. RISC-V Unprivileged ISA, 10.1 Base Counters and Timers
  __asm__ __volatile__ ("1:\n\t"                     \
                        "csrr %1, cycle\n\t"        \
                        "csrr %0, cycleh\n\t"       \
                        "csrr %2, cycleh\n\t"       \
                        "bne  %0, %2, 1b\n\t"        \
                        : "=r" (hi1), "=r" (lo), "=r" (hi2));
  return (uint64_t)hi1 << 32 | lo;
}

// Don't use the stack, because sp isn't set up.
volatile int wait = 1;

int main(int argc, char const *argv[])
{
#if 1
    while (wait)
        ;
#endif
#if 1
    // rdcycle();
    printf("hello world!\n");
    // printf("hello world! %d\n", 129);
#else
    char *str = "hello world!\n";
    write(0, str, strlen(str));
#endif
done:
#if 1
    while (!wait)
        ;
#endif
    return 0;
}

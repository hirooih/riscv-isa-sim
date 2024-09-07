#include <string.h>
#include <stdio.h>

char str[] = "Vafgehpgvba frgf jnag gb or serr!";

int main()
{
    // newlib/libgloss/riscv/crt0.S does not copy .rodata to .data.
    // so store to str[] causes access fail.
    char text[sizeof(str)];
    strncpy(text, str, sizeof(str));
    printf("%s\n", text);

    // wait for gdb being connected
    volatile int wait = 1;
    while (wait)
        ;

    int i = 0;
    while (text[i]) {
        char lower = text[i] | 32;
        if (lower >= 'a' && lower <= 'm')
            text[i] += 13;
        else if (lower > 'm' && lower <= 'z')
            text[i] -= 13;
        i++;
    }

    printf("%s\n", text);
done:
    while (!wait)
        ;
}

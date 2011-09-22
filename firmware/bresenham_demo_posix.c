#include <stdint.h>
#include <stdio.h>

#include "bresenham.c"

int main()
{
    uint16_t value;
    bresenham_state fading;

    printf("Initializing fading\n");

    value = 0;

    bresenham_init(&fading, value, 0xfff, 0x20);

    do {
        //printf("%d,%d\n", fading.dx_remaining, value);
        printf("current state: dx %d, _dy %d, err %d, remaining %d; ", fading.dx, fading._dy, fading.err, fading.dx_remaining);
        bresenham_step(&fading, &value);
        printf("after stepping, value is %d\n", value);
    } while (!bresenham_finished(&fading));

    printf("final value %d\n", value);

    return 0;
}

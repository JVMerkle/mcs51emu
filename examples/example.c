#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mcs51.h"

int main()
{
    mcs51_t proc = {};

    FILE* fp = fopen("../../examples/example.bin", "rb");
    if (fp == 0)
        exit(1);

    size_t n = fread(&proc.C, 1, sizeof(proc.C), fp);
    if (n == 0)
        exit(2);

    fclose(fp);

    mcs51_init(&proc);

    do
    {
        // 12MHz / 12 = 1MHz
        // 1 / 1MHz = 1 us
        for (int i = 0; i < 12; i++)
            msc51_do_osc_period(&proc);

        usleep(1);
    } while (proc._instruction_register.opcode.code != 0x00);

    printf("\n\nFinished with %ld oscillator periods.\n", proc._osc_periods);
    printf("That's %.2fms.\n", msc51_execution_time_ms(&proc));

    return 0;
}

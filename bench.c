
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "u512.h"
#include "fp.h"
#include "mont.h"
#include "csidh.h"
#include "mcl.h"

#include <inttypes.h>

static __inline__ uint64_t rdtsc(void)
{
    uint32_t hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return lo | (uint64_t) hi << 32;
}

unsigned long its = 100;

int main()
{
{
    const char *pStr = "5326738796327623094747867617954605554069371494832722337612446642054009560026576537626892113026381253624626941643949444792662881241621373288942880288065659";
	mcl_init(pStr);
}
    clock_t t0, t1, time = 0;
    uint64_t c0, c1, cycles = 0;

    private_key priv;
    public_key pub = base;

    for (unsigned long i = 0; i < its; ++i) {

        csidh_private(&priv);

        t0 = clock();
        c0 = rdtsc();

        /**************************************/
        assert(validate(&pub));
        action(&pub, &pub, &priv);
        /**************************************/

        c1 = rdtsc();
        t1 = clock();
        cycles += c1 - c0;
        time += t1 - t0;
    }

    printf("iterations: %lu\n", its);
    printf("clock cycles: %" PRIu64 "\n", (uint64_t) cycles / its);
    printf("wall-clock time: %.3lf ms\n", 1000. * time / CLOCKS_PER_SEC / its);
}


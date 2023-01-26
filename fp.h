#ifndef FP_H
#define FP_H

#include "u512.h"
#include "mcl.h"

/* fp is in the Montgomery domain, so interpreting that
   as an integer should never make sense.
   enable compiler warnings when mixing up u512 and fp. */
typedef struct fp {
    u512 x;
} fp;

extern const fp fp_0;
extern const fp fp_1;

void fp_set(fp *x, uint64_t y);
void fp_cswap(fp *x, fp *y, bool c);

void fp_enc(fp *x, u512 const *y); /* encode to Montgomery representation */
void fp_dec(u512 *x, fp const *y); /* decode from Montgomery representation */

#if 0
void fp_add2(fp *x, fp const *y);
void fp_sub2(fp *x, fp const *y);
void fp_mul2(fp *x, fp const *y);
#else
#define fp_add2(_x, _y) mcl_add((_x)->x.c, (_x)->x.c, (_y)->x.c)
#define fp_sub2(_x, _y) mcl_sub((_x)->x.c, (_x)->x.c, (_y)->x.c)
#define fp_mul2(_x, _y) mcl_mont((_x)->x.c, (_x)->x.c, (_y)->x.c)
#endif

#if 0
void fp_add3(fp *x, fp const *y, fp const *z);
void fp_sub3(fp *x, fp const *y, fp const *z);
void fp_mul3(fp *x, fp const *y, fp const *z);
#else
#define fp_add3(_x, _y, _z) mcl_add((_x)->x.c, (_y)->x.c, (_z)->x.c)
#define fp_sub3(_x, _y, _z) mcl_sub((_x)->x.c, (_y)->x.c, (_z)->x.c)
#define fp_mul3(_x, _y, _z) mcl_mont((_x)->x.c, (_y)->x.c, (_z)->x.c)
#endif

#if 0
void fp_sq1(fp *x);
void fp_sq2(fp *x, fp const *y);
#else
#define fp_sq1(_x) mcl_mont((_x)->x.c, (_x)->x.c, (_x)->x.c);
#define fp_sq2(_x, _y) mcl_mont((_x)->x.c, (_y)->x.c, (_y)->x.c);
#endif
void fp_inv(fp *x);
bool fp_issquare(fp const *x);

void fp_random(fp *x);

#endif

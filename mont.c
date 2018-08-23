
#include <assert.h>

#include "mont.h"
#include "u512.h"

void xDBLADD(proj *R, proj *S, proj const *P, proj const *Q, proj const *PQ, proj const *A24)
{
    fp tmp0, tmp1, tmp2;        //requires precomputation of A24=(A+2C:4C)

    fp_add3(&tmp0, &P->x, &P->z);
    fp_sub3(&tmp1, &P->x, &P->z);
    fp_sq2(&R->x, &tmp0);
    fp_sub3(&tmp2, &Q->x, &Q->z);
    fp_add3(&S->x, &Q->x, &Q->z);
    fp_mul2(&tmp0, &tmp2);
    fp_sq2(&R->z, &tmp1);
    fp_mul2(&tmp1, &S->x);
    fp_sub3(&tmp2, &R->x, &R->z);
    fp_mul2(&R->z, &A24->z);
    fp_mul2(&R->x, &R->z);
    fp_mul3(&S->x, &A24->x, &tmp2);
    fp_sub3(&S->z, &tmp0, &tmp1);
    fp_add2(&R->z, &S->x);
    fp_add3(&S->x, &tmp0, &tmp1);
    fp_mul2(&R->z, &tmp2);
    fp_sq1(&S->z);
    fp_sq1(&S->x);
    fp_mul2(&S->z, &PQ->x);
    fp_mul2(&S->x, &PQ->z);
}

void xDBL(proj *Q, proj const *A, proj const *P)
{
    fp a, b, c;
    fp_add3(&a, &P->x, &P->z);
    fp_sq1(&a);
    fp_sub3(&b, &P->x, &P->z);
    fp_sq1(&b);
    fp_sub3(&c, &a, &b);
    fp_add2(&b, &b); fp_add2(&b, &b); /* multiplication by 4 */
    fp_mul2(&b, &A->z);
    fp_mul3(&Q->x, &a, &b);
    fp_add3(&a, &A->z, &A->z); /* multiplication by 2 */
    fp_add2(&a, &A->x);
    fp_mul2(&a, &c);
    fp_add2(&a, &b);
    fp_mul3(&Q->z, &a, &c);
}

void xADD(proj *S, proj const *P, proj const *Q, proj const *PQ)
{
    fp a, b, c, d;
    fp_add3(&a, &P->x, &P->z);
    fp_sub3(&b, &P->x, &P->z);
    fp_add3(&c, &Q->x, &Q->z);
    fp_sub3(&d, &Q->x, &Q->z);
    fp_mul2(&a, &d);
    fp_mul2(&b, &c);
    fp_add3(&c, &a, &b);
    fp_sub3(&d, &a, &b);
    fp_sq1(&c);
    fp_sq1(&d);
    fp_mul3(&S->x, &PQ->z, &c);
    fp_mul3(&S->z, &PQ->x, &d);
}

/* Montgomery ladder. */
/* P must not be the unique point of order 2. */
/* not constant-time! */
void xMUL(proj *Q, proj const *A, proj const *P, u512 const *k)
{
    proj R = *P;
    proj A24;
    const proj Pcopy = *P; /* in case Q = P */

    Q->x = fp_1;
    Q->z = fp_0;

    fp_add3(&A24.x, &A->z, &A->z);    //precomputation of A24=(A+2C:4C)
    fp_add3(&A24.z, &A24.x, &A24.x);
    fp_add2(&A24.x, &A->x);

    unsigned long i = 512;
    while (--i && !u512_bit(k, i));

    do {

        bool bit = u512_bit(k, i);

        if (bit) { proj T = *Q; *Q = R; R = T; } /* not constant-time */
        //fp_cswap(&Q->x, &R.x, bit);
        //fp_cswap(&Q->z, &R.z, bit);

        xDBLADD(Q, &R, Q, &R, &Pcopy, &A24);

        if (bit) { proj T = *Q; *Q = R; R = T; } /* not constant-time */
        //fp_cswap(&Q->x, &R.x, bit);
        //fp_cswap(&Q->z, &R.z, bit);

    } while (i--);
}

//simultaneous square-and-multiply, computes x^exp and y^exp 
void exp_by_squaring_(fp* x, fp* y, uint64_t exp)
{
	fp result1, result2;
	fp_set(&result1, 1);
	fp_set(&result2, 1);

    while (exp)
    {
        if (exp & 1){
          fp_mul2(&result1, x);
          fp_mul2(&result2, y);
	}
	
        fp_sq1(x);
	fp_sq1(y);
        exp >>= 1;
    }

    fp_cswap(&result1, x, 1);
    fp_cswap(&result2, y, 1);

}


/* computes the isogeny with kernel point K of order k */
/* returns the new curve coefficient A and the image of P */
/* (obviously) not constant time in k */
void xISOG(proj *A, proj *P, proj const *K, uint64_t k)
{
    assert (k >= 3);
    assert (k % 2 == 1);

    fp tmp0, tmp1, tmp2, Psum, Pdif;
    proj Q, Aed, prod;

    fp_add3(&Aed.z, &A->z, &A->z);  //compute twisted Edwards curve coefficients
    fp_add3(&Aed.x, &A->x, &Aed.z);
    fp_sub3(&Aed.z, &A->x, &Aed.z);
   
    fp_add3(&Psum, &P->x, &P->z);   //precomputations
    fp_sub3(&Pdif, &P->x, &P->z);

    fp_sub3(&prod.x, &K->x, &K->z);
    fp_add3(&prod.z, &K->x, &K->z);
    
    fp_mul3(&tmp1, &prod.x, &Psum);
    fp_mul3(&tmp0, &prod.z, &Pdif);
    fp_add3(&Q.x, &tmp0, &tmp1);
    fp_sub3(&Q.z, &tmp0, &tmp1);

    proj M[3] = {*K};
    xDBL(&M[1], A, K);

    for (uint64_t i = 1; i < k / 2; ++i) {

        if (i >= 2)
            xADD(&M[i % 3], &M[(i - 1) % 3], K, &M[(i - 2) % 3]);

	fp_sub3(&tmp1, &M[i % 3].x, &M[i % 3].z);
    	fp_add3(&tmp0, &M[i % 3].x, &M[i % 3].z);
	fp_mul2(&prod.x, &tmp1);
        fp_mul2(&prod.z, &tmp0);
    	fp_mul2(&tmp1, &Psum);
    	fp_mul2(&tmp0, &Pdif);
    	fp_add3(&tmp2, &tmp0, &tmp1);
	fp_mul2(&Q.x, &tmp2);
    	fp_sub3(&tmp2, &tmp0, &tmp1);
	fp_mul2(&Q.z, &tmp2);

    }


    // point evaluation
    fp_sq1(&Q.x);
    fp_sq1(&Q.z);
    fp_mul2(&P->x, &Q.x);
    fp_mul2(&P->z, &Q.z);

    //compute Aed.x^k, Aed.z^k
    exp_by_squaring_(&Aed.x, &Aed.z, k);

    //compute prod.x^8, prod.z^8
    fp_sq1(&prod.x);
    fp_sq1(&prod.x);
    fp_sq1(&prod.x);
    fp_sq1(&prod.z);
    fp_sq1(&prod.z);
    fp_sq1(&prod.z);

    //compute image curve parameters
    fp_mul2(&Aed.z, &prod.x);
    fp_mul2(&Aed.x, &prod.z);

    //compute Montgomery params
    fp_add3(&A->x, &Aed.x, &Aed.z);
    fp_sub3(&A->z, &Aed.x, &Aed.z);
    fp_add2(&A->x, &A->x);
}


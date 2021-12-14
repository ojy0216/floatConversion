#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "f2b.h"

uint64_t shiftRound(uint64_t n, int shift){
    if(shift < 0){
        printf("SHIFT WITH NEGATIVE NUMBER!\n");
        return EXIT_FAILURE;
    }

    uint64_t result = n >> shift;

    // Using only 1 extra bit 
    if(shift == 0)
        return result;

    uint64_t mask = 1lu << (shift - 1);
    if((n & mask) >> (shift - 1))
        result += 1;

    // Using GRS bits, round to nearest even
    /*
    uint64_t guard_mask, round_mask, sticky_mask;
    uint64_t grs_mask, lsb_mask;
    uint64_t grs, lsb;

    if(shift >= 1)
        guard_mask = 1lu << (shift - 1);
    else
        guard_mask = 0u;

    if(shift >= 2)
        round_mask = 1lu << (shift - 2);
    else
        round_mask = 0lu;

    if(shift >= 3)
        sticky_mask = round_mask - 1;
    else
        sticky_mask = 0lu;

    grs_mask = guard_mask + round_mask + sticky_mask;

    if(shift >= 3)
        grs = (n & grs_mask) >> (shift - 3);
    else
        grs = (n & grs_mask) << (3 - shift);

    lsb_mask = 1lu << shift;
    lsb = (n & lsb_mask) >> shift;

    if(grs > 0b100)
        result += 1;
    else if(grs == 0b100){
        // Tie -> Round to Nearest Even
        if(lsb)
            result += 1;
    }
    */

    return result;
}

void bf2single(BF16* f){
    uint32_t bf;
    float fp;

	uint8_t sign = f->sign;
	uint8_t exp = f->exp;
	uint32_t mant = (uint32_t)f->mant << (S_MANT_BIT - B_MANT_BIT);

    bf = ((uint32_t)sign << (S_EXP_BIT + S_MANT_BIT)) | ((uint32_t)exp << S_MANT_BIT) | mant;

    memcpy(&fp, &bf, sizeof(fp));

    printf("%0.95e\n\n", fp);
}

void half2single(FP16* f){
    uint32_t bf;
    float fp;

    uint8_t sign = f->sign;
    uint8_t exp = f->exp - H_BIAS + S_BIAS;
    uint32_t mant = (uint32_t)f->mant << (S_MANT_BIT - H_MANT_BIT);

    if(f->exp == 0u){
        if(f->mant == 0u){
            // Underflow
            exp = 0u;
            mant = 0u;
        }
        else{
            // Subnormal number dealing
            while(!(mant & S_MANT_NAN)){
                mant <<= 1;
                exp--;
            }
            mant <<= 1;
            mant = mant & H2S_MASK;
        }
    }
    else if(f->exp == (H_EXP_MAX + H_BIAS)){
        if(f->mant == H_MANT_NAN){
            // NaN
            exp = S_EXP_MAX + S_BIAS;
            mant = S_MANT_NAN;
        }
        else{
            // Inf
            exp = S_EXP_MAX + S_BIAS;
            mant = 0u;
        }
    }

    bf = ((uint32_t)sign << (S_EXP_BIT + S_MANT_BIT)) | ((uint32_t)exp << S_MANT_BIT) | mant;

    memcpy(&fp, &bf, sizeof(fp));

    printf("%0.19e\n\n", fp);
}

void printBF16(BF16* f){
    uint16_t bf = (f->sign << (B_EXP_BIT + B_MANT_BIT)) | (f->exp << B_MANT_BIT) | f->mant;

	printf("[bfloat16]\n");

    printf("Sign : %u\n", f->sign);

    printf("Exponent : ");
    for(int i = 0; i < B_EXP_BIT; i++)
        printf("%u", (f->exp & ((uint8_t)1 << (B_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", f->exp);

    printf("Mantissa : ");
    for(int i = 0; i < B_MANT_BIT; i++)
        printf("%u", (f->mant & ((uint8_t)1 << (B_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", f->mant);

    printf("0x%04X\n", bf);
    bf2single(f);
}

void printHalfPrecision(FP16* f){
    uint16_t hp = (f->sign << (H_EXP_BIT + H_MANT_BIT)) | (f->exp << H_MANT_BIT) | f->mant;

    printf("[Half Precision]\n");

    printf("Sign : %u\n", f->sign);

    printf("Exponent : ");
    for(int i = 0; i < H_EXP_BIT; i++)
        printf("%u", (f->exp & ((uint8_t)1 << (H_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", f->exp);

    printf("Mantissa : ");
    for(int i = 0; i < H_MANT_BIT; i++)
        printf("%u", (f->mant & ((uint16_t)1 << (H_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", f->mant);

    printf("0x%04X\n", hp);
    half2single(f);
}

void printSinglePrecision(FP32* f){
    float fp;
    uint32_t sp = (f->sign << (S_EXP_BIT + S_MANT_BIT)) | (f->exp << S_MANT_BIT) | f->mant;

    printf("[Single Precision]\n");

    printf("Sign : %u\n", f->sign);

    printf("Exponent : ");
    for(int i = 0; i < S_EXP_BIT; i++)
        printf("%u", (f->exp & ((uint8_t)1 << (S_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", f->exp);

    printf("Mantissa : ");
    for(int i = 0; i < S_MANT_BIT; i++)
        printf("%u", (f->mant & ((uint32_t)1 << (S_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", f->mant);

    printf("0x%08X\n", sp);

    memcpy(&fp, &sp, sizeof(fp));

    printf("%0.111e\n\n", fp);
}

void printDoublePrecision(FP64* f){
    uint64_t dp = ((uint64_t)f->sign << (D_EXP_BIT + D_MANT_BIT)) | ((uint64_t)f->exp << D_MANT_BIT) | f->mant;

    printf("[Double Precision]\n");

    printf("Sign : %u\n", f->sign);

    printf("Exponent : ");
    for(int i = 0; i < D_EXP_BIT; i++)
        printf("%u", (f->exp & ((uint16_t)1 << (D_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", f->exp);

    printf("Mantissa : ");
    for(int i = 0; i < D_MANT_BIT; i++)
        printf("%u", (f->mant & ((uint64_t)1 << (D_MANT_BIT - 1 - i))) != 0);
    printf(" (%lu)\n", f->mant);

    printf("0x%016lX\n\n", dp);
}

void convert(double n){
    uint64_t dn;

    /* Double Precision */
    FP64* dp = malloc(sizeof(FP64));

    /* Single Precision */
    FP32* sp = malloc(sizeof(FP32));

    /* Half Precision */
    FP16* hp = malloc(sizeof(FP16));

    /* bfloat16 */
    BF16* bf = malloc(sizeof(BF16));

    memcpy(&dn, &n, sizeof(n));

    /* Double Precision Calculation */
    dp->sign = (dn & D_SIGN_MASK) >> (D_EXP_BIT + D_MANT_BIT);
    dp->exp = (dn & D_EXP_MASK) >> D_MANT_BIT;
    dp->mant = dn & D_MANT_MASK;

    /* Single Precision Calculation */
    sp->sign = dp->sign;
    sp->mant = (uint32_t)shiftRound(dp->mant, D_MANT_BIT - S_MANT_BIT);
    if(dp->exp == 0){
        // 0, subnormal number -> 0
        sp->exp = 0u;
        sp->mant = 0u;
    }
    else if(dp->exp == (D_EXP_MAX + D_BIAS)){
        if(dp->mant == D_MANT_NAN){
            // NaN
            sp->exp = S_EXP_MAX + S_BIAS;
            sp->mant = S_MANT_NAN;
        }
        else{
            // Inf
            sp->exp = S_EXP_MAX + S_BIAS;
            sp->mant = 0u;
        }
    }
    else if(dp->exp >= D_BIAS + S_EXP_MAX){
        // Overflow -> Inf
        sp->exp = S_EXP_MAX + S_BIAS;
        sp->mant = 0u;
    }
    else if(dp->exp <= D_BIAS + S_EXP_MIN){
        sp->exp = 0u;
        if(dp->mant == 0lu || (D_BIAS + S_EXP_MIN - dp->exp >= (S_MANT_BIT + 1))){
            // Underflow -> 0
            sp->mant = 0u;
        }
        else{
            // Subnormal number
            sp->mant = (sp->mant >> 1) | S_MANT_NAN;
            if(D_BIAS + S_EXP_MIN - dp->exp > 0)
                sp->mant = (uint32_t)shiftRound((uint64_t)sp->mant, D_BIAS + S_EXP_MIN - dp->exp);
        }
    }
    else{
        sp->exp = (uint8_t)(dp->exp - D_BIAS + S_BIAS);
    }

    /* Half Precision Calculation */
    hp->sign = dp->sign;
    hp->mant = (uint16_t)shiftRound(dp->mant, D_MANT_BIT - H_MANT_BIT);
    if(dp->exp == 0){
        // 0, subnormal number -> 0
        hp->exp = 0u;
        hp->mant = 0u;
    }
    else if(dp->exp == (D_EXP_MAX + D_BIAS)){
        if(dp->mant == D_MANT_NAN){
            // NaN
            hp->exp = H_EXP_MAX + H_BIAS;
            hp->mant = H_MANT_NAN;
        }
        else{
            // Inf
            hp->exp = H_EXP_MAX + H_BIAS;
            hp->mant = 0u;
        }
    }
    else if(dp->exp >= D_BIAS + H_EXP_MAX){
        // Overflow -> Inf
        hp->exp = H_EXP_MAX + H_BIAS;
        hp->mant = 0u;
    }
    else if(dp->exp <= D_BIAS + H_EXP_MIN){
        hp->exp = 0u;
        if(dp->mant == 0lu || (D_BIAS + H_EXP_MIN - dp->exp >= (H_MANT_BIT + 1))){
            // Underflow -> 0
            hp->mant = 0u;
        }
        else{
            // Subnormal number
            hp->mant = (hp->mant >> 1) | H_MANT_NAN;
            if(D_BIAS + H_EXP_MIN - dp->exp > 0)
                hp->mant = (uint16_t)shiftRound((uint64_t)hp->mant, D_BIAS + H_EXP_MIN - dp->exp);
        }
    }
    else{
        hp->exp = (uint8_t)(dp->exp - D_BIAS + H_BIAS);
    }

    /* bfloat16 Calculation */
    bf->sign = dp->sign;
    bf->mant = (uint8_t)shiftRound(dp->mant, D_MANT_BIT - B_MANT_BIT);
    if(dp->exp == 0){
        // 0, subnormal number -> 0
        bf->exp = 0u;
        bf->mant = 0u;
    }
    else if(dp->exp == (D_EXP_MAX + D_BIAS)){
        if(dp->mant == D_MANT_NAN){
            // NaN
            bf->exp = B_EXP_MAX + B_BIAS;
            bf->mant = B_MANT_NAN;
        }
        else{
            // Inf
            bf->exp = B_EXP_MAX + B_BIAS;
            bf->mant = 0u;
        }
    }
    else if(dp->exp >= D_BIAS + B_EXP_MAX){
        // Overflow -> Inf
        bf->exp = B_EXP_MAX + B_BIAS;
        bf->mant = 0u;
    }
    else if(dp->exp <= D_BIAS + B_EXP_MIN){
        bf->exp = 0u;
        if(dp->mant == 0lu || (D_BIAS + B_EXP_MIN - dp->exp >= (B_MANT_BIT + 1))){
            // Underflow -> 0
            bf->mant = 0u;
        }
        else{
            // Subnormal number
            bf->mant = (bf->mant >> 1) | B_MANT_NAN;
            if(D_BIAS + B_EXP_MIN - dp->exp > 0)
                bf->mant = (uint8_t)shiftRound((uint64_t)bf->mant, D_BIAS + B_EXP_MIN - dp->exp);
        }
    }
    else{
        bf->exp = (uint8_t)(dp->exp - D_BIAS + B_BIAS);
    }

    printHalfPrecision(hp);
    printSinglePrecision(sp);
    printDoublePrecision(dp);
    printBF16(bf);

    free(dp);
    free(sp);
    free(hp);
    free(bf);
}

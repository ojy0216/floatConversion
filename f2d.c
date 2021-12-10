#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "f2d.h"

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

void bf2single(uint8_t sign, uint8_t exp, uint8_t mant){
    uint32_t bf;
    float f;

	uint8_t f_sign = sign;
	uint8_t f_exp = exp;
	uint32_t f_mant = (uint32_t)mant << (S_MANT_BIT - B_MANT_BIT);

    bf = ((uint32_t)f_sign << (S_EXP_BIT + S_MANT_BIT)) | ((uint32_t)f_exp << S_MANT_BIT) | f_mant;

    memcpy(&f, &bf, sizeof(f));

    printf("%0.95e\n\n", f);
}

void half2single(uint8_t sign, uint8_t exp, uint16_t mant){
    uint32_t bf;
    float f;

    uint8_t f_sign = sign;
    uint8_t f_exp = exp - H_BIAS + S_BIAS;
    uint32_t f_mant = (uint32_t)mant << (S_MANT_BIT - H_MANT_BIT);

    if(exp == 0u){
        if(mant == 0u){
            // Underflow
            f_exp = 0u;
            f_mant = 0u;
        }
        else{
            // Subnormal number dealing
            while(!(f_mant & S_MANT_NAN)){
                f_mant <<= 1;
                f_exp--;
            }
            f_mant <<= 1;
            f_mant = f_mant & H2S_MASK;
        }
    }
    else if(exp == (H_EXP_MAX + H_BIAS)){
        if(mant == H_MANT_NAN){
            // NaN
            f_exp = S_EXP_MAX + S_BIAS;
            f_mant = S_MANT_NAN;
        }
        else{
            // Inf
            f_exp = S_EXP_MAX + S_BIAS;
            f_mant = 0u;
        }
    }

    bf = ((uint32_t)f_sign << (S_EXP_BIT + S_MANT_BIT)) | ((uint32_t)f_exp << S_MANT_BIT) | f_mant;

    memcpy(&f, &bf, sizeof(f));

    printf("%0.19e\n\n", f);
}

void printBF16(uint8_t sign, uint8_t exp, uint8_t mant){
    uint16_t bf = (sign << (B_EXP_BIT + B_MANT_BIT)) | (exp << B_MANT_BIT) | mant;

	printf("[bfloat16]\n");

	printf("Sign : %u\n", sign);

    printf("Exponent : ");
    for(int i = 0; i < B_EXP_BIT; i++)
        printf("%u", (exp & ((uint8_t)1 << (B_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", exp);

    printf("Mantissa : ");
    for(int i = 0; i < B_MANT_BIT; i++)
        printf("%u", (mant & ((uint8_t)1 << (B_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", mant);

    printf("0x%04X\n", bf);
    bf2single(sign, exp, mant);
}

void printHalfPrecision(uint8_t sign, uint8_t exp, uint16_t mant){
    uint16_t hp = (sign << (H_EXP_BIT + H_MANT_BIT)) | (exp << H_MANT_BIT) | mant;

    printf("[Half Precision]\n");

    printf("Sign : %u\n", sign);

    printf("Exponent : ");
    for(int i = 0; i < H_EXP_BIT; i++)
        printf("%u", (exp & ((uint8_t)1 << (H_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", exp);

    printf("Mantissa : ");
    for(int i = 0; i < H_MANT_BIT; i++)
        printf("%u", (mant & ((uint16_t)1 << (H_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", mant);

    printf("0x%04X\n", hp);
    half2single(sign, exp, mant);
}

void printSinglePrecision(uint8_t sign, uint8_t exp, uint32_t mant){
    float f;
    uint32_t sp = (sign << (S_EXP_BIT + S_MANT_BIT)) | (exp << S_MANT_BIT) | mant;

    printf("[Single Precision]\n");

    printf("Sign : %u\n", sign);

    printf("Exponent : ");
    for(int i = 0; i < S_EXP_BIT; i++)
        printf("%u", (exp & ((uint8_t)1 << (S_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", exp);

    printf("Mantissa : ");
    for(int i = 0; i < S_MANT_BIT; i++)
        printf("%u", (mant & ((uint32_t)1 << (S_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", mant);

    printf("0x%08X\n", sp);

    memcpy(&f, &sp, sizeof(f));

    printf("%0.111e\n\n", f);
}

void printDoublePrecision(uint8_t sign, uint16_t exp, uint64_t mant){
    uint64_t dp = ((uint64_t)sign << (D_EXP_BIT + D_MANT_BIT)) | ((uint64_t)exp << D_MANT_BIT) | mant;

    printf("[Double Precision]\n");

    printf("Sign : %u\n", sign);

    printf("Exponent : ");
    for(int i = 0; i < D_EXP_BIT; i++)
        printf("%u", (exp & ((uint16_t)1 << (D_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", exp);

    printf("Mantissa : ");
    for(int i = 0; i < D_MANT_BIT; i++)
        printf("%u", (mant & ((uint64_t)1 << (D_MANT_BIT - 1 - i))) != 0);
    printf(" (%lu)\n", mant);

    printf("0x%016lX\n\n", dp);
}

void convert(double n){
    uint64_t dn;

    /* Double Precision */
    uint8_t d_sign;
    uint16_t d_exp;
    uint64_t d_mant;

    /* Single Precision */
    uint8_t s_sign;
    uint8_t s_exp;
    uint32_t s_mant;

    /* Half Precision */
    uint8_t h_sign;
    uint8_t h_exp;
    uint16_t h_mant;

    /* bfloat16 */
    uint8_t b_sign;
    uint8_t b_exp;
    uint8_t b_mant;

    memcpy(&dn, &n, sizeof(n));

    /* Double Precision Calculation */
    d_sign = (dn & D_SIGN_MASK) >> (D_EXP_BIT + D_MANT_BIT);
    d_exp = (dn & D_EXP_MASK) >> D_MANT_BIT;
    d_mant = dn & D_MANT_MASK;

    /* Single Precision Calculation */
    s_sign = d_sign;
    s_mant = (uint32_t)shiftRound(d_mant, D_MANT_BIT - S_MANT_BIT);
    if(d_mant == D_MANT_NAN){
        // NaN
        s_exp = S_EXP_MAX + S_BIAS;
        s_mant = S_MANT_NAN;
    }
    else if(d_exp >= D_BIAS + S_EXP_MAX){
        // Overflow -> inf
        s_exp = S_EXP_MAX + S_BIAS;
        s_mant = 0u;
    }
    else if(d_exp <= D_BIAS + S_EXP_MIN){
        s_exp = 0u;
        if(d_mant == 0u || (D_BIAS + S_EXP_MIN - d_exp >= (S_MANT_BIT + 1))){
            // Underflow -> 0
            s_mant = 0u;
        }
        else{
            // Subnormal number
            s_mant = (s_mant >> 1) | S_MANT_NAN;
            if(D_BIAS + S_EXP_MIN - d_exp > 0)
                s_mant = (uint32_t)shiftRound((uint64_t)s_mant, D_BIAS + S_EXP_MIN - d_exp);
        }
    }
    else
        s_exp = (uint8_t)(d_exp - D_BIAS + S_BIAS);

    /* Half Precision Calculation */
    h_sign = d_sign;
    h_mant = (uint16_t)shiftRound(d_mant, D_MANT_BIT - H_MANT_BIT);
    if(d_mant == D_MANT_NAN){
        // NaN
        h_exp = H_EXP_MAX + H_BIAS;
        h_mant = H_MANT_NAN;
    }
    else if(d_exp >= D_BIAS + H_EXP_MAX){
        // Overflow -> inf
        h_exp = H_EXP_MAX + H_BIAS;
        h_mant = 0u;
    }
    else if(d_exp <= D_BIAS + H_EXP_MIN){
        h_exp = 0u;
        if(d_mant == 0u || (D_BIAS + H_EXP_MIN - d_exp >= (H_MANT_BIT + 1))){
            // Underflow -> 0
            h_mant = 0u;
        }
        else{
            // Subnormal number
            h_mant = (h_mant >> 1) | H_MANT_NAN;
            if(D_BIAS + H_EXP_MIN - d_exp > 0)
                h_mant = (uint16_t)shiftRound((uint64_t)h_mant, D_BIAS + H_EXP_MIN - d_exp);
        }
    }
    else
        h_exp = (uint8_t)(d_exp - D_BIAS + H_BIAS);

    /* bfloat16 Calculation */
    b_sign = d_sign;
    b_mant = (uint8_t)shiftRound(d_mant, D_MANT_BIT - B_MANT_BIT);
    if(d_mant == D_MANT_NAN){
        // NaN
        b_exp = B_EXP_MAX + B_BIAS;
        b_mant = B_MANT_NAN;
    }
    else if(d_exp >= D_BIAS + B_EXP_MAX){
        // Overflow -> inf
        b_exp = B_EXP_MAX + B_BIAS;
        b_mant = 0u;
    }
    else if(d_exp <= D_BIAS + B_EXP_MIN){
        b_exp = 0u;
        if(d_mant == 0u || (D_BIAS + B_EXP_MIN - d_exp >= (B_MANT_BIT + 1))){
            // Underflow -> 0
            b_mant = 0u;    
        }
        else{
            // Subnormal number
            b_mant = (b_mant >> 1) | B_MANT_NAN;
            if(D_BIAS + B_EXP_MIN - d_exp > 0)
                b_mant = (uint8_t)shiftRound((uint64_t)b_mant, D_BIAS + B_EXP_MIN - d_exp);
        }
    }
    else
        b_exp = (uint8_t)(d_exp - D_BIAS + B_BIAS);

    printHalfPrecision(h_sign, h_exp, h_mant);
    printSinglePrecision(s_sign, s_exp, s_mant);
    printDoublePrecision(d_sign, d_exp, d_mant);
    printBF16(b_sign, b_exp, b_mant);
}

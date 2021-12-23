#include "f2b.h"

uint64_t shiftRound(uint64_t n, int shift){
    if(shift < 0){
        printf("SHIFT WITH NEGATIVE NUMBER!\n");
        return EXIT_FAILURE;
    }

    uint64_t result = n >> shift;

    // Using only 1 extra bit 
    /*
    if(shift == 0)
        return result;

    uint64_t mask = 1lu << (shift - 1);
    if((n & mask) >> (shift - 1))
        result += 1;
    */

    // Using GRS bits, round to nearest even
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

    return result;
}

void printBF2Float(BF16* bf){
    FP32* sp = BF2SP(bf);

    float f = hexSP2Float(assembleSP(sp));

    free(sp);

    printf("%0.95e\n\n", f);
}

void printHP2Float(FP16* hp){
    FP32* sp = HP2SP(hp);

    float f = hexSP2Float(assembleSP(sp));

    free(sp);

    printf("%0.19e\n\n", f);
}

void printBF16(BF16* bf){
    uint16_t bn = (bf->sign << (B_EXP_BIT + B_MANT_BIT)) | (bf->exp << B_MANT_BIT) | bf->mant;

	printf("[bfloat16]\n");

    printf("Sign : %u\n", bf->sign);

    printf("Exponent : ");
    for(int i = 0; i < B_EXP_BIT; i++)
        printf("%u", (bf->exp & ((uint8_t)1 << (B_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", bf->exp);

    printf("Mantissa : ");
    for(int i = 0; i < B_MANT_BIT; i++)
        printf("%u", (bf->mant & ((uint8_t)1 << (B_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", bf->mant);

    printf("0x%04X\n", bn);
    printBF2Float(bf);
}

void printHalfPrecision(FP16* hp){
    uint16_t hn = (hp->sign << (H_EXP_BIT + H_MANT_BIT)) | (hp->exp << H_MANT_BIT) | hp->mant;

    printf("[Half Precision]\n");

    printf("Sign : %u\n", hp->sign);

    printf("Exponent : ");
    for(int i = 0; i < H_EXP_BIT; i++)
        printf("%u", (hp->exp & ((uint8_t)1 << (H_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", hp->exp);

    printf("Mantissa : ");
    for(int i = 0; i < H_MANT_BIT; i++)
        printf("%u", (hp->mant & ((uint16_t)1 << (H_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", hp->mant);

    printf("0x%04X\n", hn);
    printHP2Float(hp);
}

void printSinglePrecision(FP32* sp){
    float f;
    uint32_t sn = (sp->sign << (S_EXP_BIT + S_MANT_BIT)) | (sp->exp << S_MANT_BIT) | sp->mant;

    printf("[Single Precision]\n");

    printf("Sign : %u\n", sp->sign);

    printf("Exponent : ");
    for(int i = 0; i < S_EXP_BIT; i++)
        printf("%u", (sp->exp & ((uint8_t)1 << (S_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", sp->exp);

    printf("Mantissa : ");
    for(int i = 0; i < S_MANT_BIT; i++)
        printf("%u", (sp->mant & ((uint32_t)1 << (S_MANT_BIT - 1 - i))) != 0);
    printf(" (%u)\n", sp->mant);

    printf("0x%08X\n", sn);

    memcpy(&f, &sn, sizeof(f));

    printf("%0.111e\n\n", f);
}

void printDoublePrecision(FP64* dp){
    uint64_t dn = ((uint64_t)dp->sign << (D_EXP_BIT + D_MANT_BIT)) | ((uint64_t)dp->exp << D_MANT_BIT) | dp->mant;

    printf("[Double Precision]\n");

    printf("Sign : %u\n", dp->sign);

    printf("Exponent : ");
    for(int i = 0; i < D_EXP_BIT; i++)
        printf("%u", (dp->exp & ((uint16_t)1 << (D_EXP_BIT - 1 - i))) != 0);
    printf(" (%u)\n", dp->exp);

    printf("Mantissa : ");
    for(int i = 0; i < D_MANT_BIT; i++)
        printf("%u", (dp->mant & ((uint64_t)1 << (D_MANT_BIT - 1 - i))) != 0);
    printf(" (%lu)\n", dp->mant);

    printf("0x%016lX\n", dn);

    if(dp->exp == D_EXP_MAX + D_BIAS){
        if(dp->sign){
            printf("-");
        }
        if(dp->mant == 0lu){
            printf("inf\n");
        }
        else if(dp->mant == D_MANT_NAN){
            printf("nan\n");
        }
    }
    else if(dp->exp == 0u && dp->mant == 0lu){
        printf("0\n");
    }
    printf("\n");
}

BF16* disassembleHexBF(uint16_t n){
    BF16* bf = malloc(sizeof(BF16));

    bf->sign = (uint8_t)((n & B_SIGN_MASK) >> (B_EXP_BIT + B_MANT_BIT));
    bf->exp = (uint8_t)((n & B_EXP_MASK) >> B_MANT_BIT);
    bf->mant = (uint8_t)(n & B_MANT_MASK);

    return bf;
}

FP16* disassembleHexHP(uint16_t n){
    FP16* hp = malloc(sizeof(FP16));

    hp->sign = (uint8_t)((n & H_SIGN_MASK) >> (H_EXP_BIT + H_MANT_BIT));
    hp->exp = (uint8_t)((n & H_EXP_MASK) >> H_MANT_BIT);
    hp->mant = n & H_MANT_MASK;

    return hp;
}

FP32* disassembleHexSP(uint32_t n){
    FP32* sp = malloc(sizeof(FP32));

    sp->sign = (uint8_t)((n & S_SIGN_MASK) >> (S_EXP_BIT + S_MANT_BIT));
    sp->exp = (uint16_t)((n & S_EXP_MASK) >> S_MANT_BIT);
    sp->mant = n & S_MANT_MASK;

    return sp;
}

FP64* disassembleHexDP(uint64_t n){
    FP64* dp = malloc(sizeof(FP64));

    dp->sign = (uint8_t)((n & D_SIGN_MASK) >> (D_EXP_BIT + D_MANT_BIT));
    dp->exp = (uint16_t)((n & D_EXP_MASK) >> D_MANT_BIT);
    dp->mant = n & D_MANT_MASK;

    return dp;
}

FP64* disassembleDP(uint64_t n){
    FP64* dp = malloc(sizeof(FP64));

    dp->sign = (uint8_t)((n & D_SIGN_MASK) >> (D_EXP_BIT + D_MANT_BIT));
    dp->exp = (uint16_t)((n & D_EXP_MASK) >> D_MANT_BIT);
    dp->mant = n & D_MANT_MASK;

    return dp;
}

float hexSP2Float(uint32_t n){
    float f;

    memcpy(&f, &n, sizeof(f));

    return f;
}

double hexBF2Double(uint16_t n){
    BF16* bf = disassembleHexBF(n);

    if(bf->exp == (B_EXP_MAX + B_BIAS) && bf->mant != B_MANT_NAN && bf->mant != 0u){
        WARNING
        printf("Given input is not a IEEE-754 standard, but it will be interpreted as nan.\n");
        free(bf);
        return NAN;
    }

    FP64* dp = malloc(sizeof(FP64));
    uint64_t d_bin;
    double d;

    dp->sign = bf->sign;
    dp->exp = (uint16_t)bf->exp - B_BIAS + D_BIAS;
    dp->mant = (uint64_t)bf->mant << (D_MANT_BIT - B_MANT_BIT);

    if(bf->exp == 0u){
        if(bf->mant == 0u){
            // 0
            dp->exp = 0u;
            dp->mant = 0lu;
        }
        else{
            // Subnormal number
            while(!(dp->mant & D_MANT_NAN)){
                dp->mant <<= 1;
                dp->exp--;
            }
            dp->mant <<= 1;
            dp->mant = dp->mant & B2D_MASK;
        }
    }
    else if(bf->exp == (B_EXP_MAX + B_BIAS)){
        dp->exp = D_EXP_MAX + D_BIAS;
        if(bf->mant == B_MANT_NAN){
            // NaN
            dp->mant = D_MANT_NAN;
        }
        else{
            // Inf
            dp->mant = 0lu;
        }
    }

    free(bf);

    d_bin = assembleDP(dp);

    free(dp);

    memcpy(&d, &d_bin, sizeof(d));

    return d;
}

double hexHP2Double(uint16_t n){
    FP16* hp = disassembleHexHP(n);

    if(hp->exp == (H_EXP_MAX + H_BIAS) && hp->mant != H_MANT_NAN && hp->mant != 0u){
        WARNING
        printf("Given input is not a IEEE-754 standard, but it will be interpreted as nan.\n");
        free(hp);
        return NAN;
    }

    FP64* dp = malloc(sizeof(FP64));
    uint64_t d_bin;
    double d;

    dp->sign = hp->sign;
    dp->exp = (uint16_t)hp->exp - H_BIAS + D_BIAS;
    dp->mant = (uint64_t)hp->mant << (D_MANT_BIT - H_MANT_BIT);

    if(hp->exp == 0u){
        if(hp->mant == 0u){
            // 0
            dp->exp = 0u;
            dp->mant = 0lu;
        }
        else{
            // Subnormal number
            while(!(dp->mant & D_MANT_NAN)){
                dp->mant <<= 1;
                dp->exp--;
            }
            dp->mant <<= 1;
            dp->mant = dp->mant & H2D_MASK;
        }
    }
    else if(hp->exp == (H_EXP_MAX + H_BIAS)){
        dp->exp = D_EXP_MAX + D_BIAS;
        if(hp->mant == H_MANT_NAN){
            // NaN
            dp->mant = D_MANT_NAN;
        }
        else{
            // Inf
            dp->mant = 0lu;
        }
    }

    free(hp);

    d_bin = assembleDP(dp);

    free(dp);

    memcpy(&d, &d_bin, sizeof(d));

    return d;
}

double hexSP2Double(uint32_t n){
    FP32* sp = disassembleHexSP(n);

    if(sp->exp == (S_EXP_MAX + S_BIAS) && sp->mant != S_MANT_NAN && sp->mant != 0u){
        WARNING
        printf("Given input is not a IEEE-754 standard, but it will be interpreted as nan.\n");
        free(sp);
        return NAN;
    }

    FP64* dp = malloc(sizeof(FP64));
    uint64_t d_bin;
    double d;

    dp->sign = sp->sign;
    dp->exp = (uint16_t)sp->exp - S_BIAS + D_BIAS;
    dp->mant = (uint64_t)sp->mant << (D_MANT_BIT - S_MANT_BIT);

    if(sp->exp == 0u){
        if(sp->mant == 0u){
            // 0
            dp->exp = 0u;
            dp->mant = 0lu;
        }
        else{
            // Subnormal number
            while(!(dp->mant & D_MANT_NAN)){
                dp->mant <<= 1;
                dp->exp--;
            }
            dp->mant <<= 1;
            dp->mant = dp->mant & S2D_MASK;
        }
    }
    else if(sp->exp == (S_EXP_MAX + S_BIAS)){
        dp->exp = D_EXP_MAX + D_BIAS;
        if(sp->mant == S_MANT_NAN){
            // NaN
            dp->mant = D_MANT_NAN;
        }
        else{
            // Inf
            dp->mant = 0lu;
        }
    }

    free(sp);

    d_bin = assembleDP(dp);

    free(dp);

    memcpy(&d, &d_bin, sizeof(d));

    return d;
}

double hexDP2Double(uint64_t n){
    FP64* dp = disassembleHexDP(n);

    if(dp->exp == (D_EXP_MAX + D_BIAS) && dp->mant != D_MANT_NAN && dp->mant != 0lu){
        WARNING
        printf("Given input is not a IEEE-754 standard, but it will be interpreted as nan.\n");
        free(dp);
        return NAN;
    }

    double d;

    memcpy(&d, &n, sizeof(d));

    return d;
}

uint32_t assembleSP(FP32* sp){
    uint32_t sn = ((uint32_t)sp->sign << (S_EXP_BIT + S_MANT_BIT)) | ((uint32_t)sp->exp << S_MANT_BIT) | sp->mant;

    return sn;
}

uint64_t assembleDP(FP64* dp){
    uint64_t dn = ((uint64_t)dp->sign << (D_EXP_BIT + D_MANT_BIT)) | ((uint64_t)dp->exp << D_MANT_BIT) | dp->mant;

    return dn;
}

FP32* DP2SP(FP64* dp){
    FP32* sp = malloc(sizeof(FP32));

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
        if(D_BIAS + S_EXP_MIN - dp->exp >= (S_MANT_BIT + 1)){
            // Underflow -> 0
            sp->mant = 0u;
        }
        else{
            // Subnormal number
            sp->mant = (sp->mant >> 1) | S_MANT_NAN;
            sp->mant >>= ((-S_BIAS + 1) - (dp->exp - D_BIAS + 1));
        }
    }
    else{
        sp->exp = (uint8_t)(dp->exp - D_BIAS + S_BIAS);
    }

    return sp;
}

FP16* DP2HP(FP64* dp){
    FP16* hp = malloc(sizeof(FP16));

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
        if(D_BIAS + H_EXP_MIN - dp->exp >= (H_MANT_BIT + 1)){
            // Underflow -> 0
            hp->mant = 0u;
        }
        else{
            // Subnormal number
            hp->mant = (hp->mant >> 1) | H_MANT_NAN;
            hp->mant >>= ((-H_BIAS + 1) - (dp->exp - D_BIAS + 1));
        }
    }
    else{
        hp->exp = (uint8_t)(dp->exp - D_BIAS + H_BIAS);
    }

    return hp;
}

BF16* DP2BF(FP64* dp){
    BF16* bf = malloc(sizeof(BF16));

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
        if(D_BIAS + B_EXP_MIN - dp->exp >= (B_MANT_BIT + 1)){
            // Underflow -> 0
            bf->mant = 0u;
        }
        else{
            // Subnormal number
            bf->mant = (bf->mant >> 1) | B_MANT_NAN;
            bf->mant >>= ((-B_BIAS + 1) - (dp->exp - D_BIAS + 1));
        }
    }
    else{
        bf->exp = (uint8_t)(dp->exp - D_BIAS + B_BIAS);
    }

    return bf;
}

FP32* BF2SP(BF16* bf){
    FP32* sp = malloc(sizeof(FP32));

    sp->sign = bf->sign;
    sp->exp = bf->exp;
    sp->mant = (uint32_t)bf->mant << (S_MANT_BIT - B_MANT_BIT);

    return sp;
}

FP32* HP2SP(FP16* hp){
	// TODO
    FP32* sp = malloc(sizeof(FP32));

    sp->sign = hp->sign;
    sp->exp = hp->exp - H_BIAS + S_BIAS;
    sp->mant = (uint32_t)hp->mant << (S_MANT_BIT - H_MANT_BIT);

    if(hp->exp == 0u){
        if(hp->mant == 0u){
            // Underflow
            sp->exp = 0u;
            sp->mant = 0u;
        }
        else{
            // Subnormal number
            while(!(sp->mant & S_MANT_NAN)){
                sp->mant <<= 1;
                sp->exp--;
            }
            sp->mant <<= 1;
            sp->mant = sp->mant & H2S_MASK;
        }
    }
    else if(hp->exp == (H_EXP_MAX + H_BIAS)){
        sp->exp = S_EXP_MAX + S_BIAS;
        if(hp->mant == H_MANT_NAN){
            // NaN
            sp->mant = S_MANT_NAN;
        }
        else{
            // Inf
            sp->mant = 0u;
        }
    }

    return sp;
}

void convert(double n){
    uint64_t dn;

    memcpy(&dn, &n, sizeof(n));

    FP64* dp = disassembleHexDP(dn);
    FP32* sp = DP2SP(dp);
    FP16* hp = DP2HP(dp);
    BF16* bf = DP2BF(dp);

    printHalfPrecision(hp);
    printSinglePrecision(sp);
    printDoublePrecision(dp);
    printBF16(bf);

    free(dp);
    free(sp);
    free(hp);
    free(bf);
}

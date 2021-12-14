#ifndef __F2B_H__
#define __F2B_H__

#include <stdint.h>

/* bfloat16 */
#define B_SIGN_BIT  1
#define B_EXP_BIT   8
#define B_MANT_BIT  7

#define B_BIAS      ((1 << (B_EXP_BIT - 1)) - 1)

#define B_MANT_NAN  0x0000000000000040

#define B_EXP_MAX   B_BIAS + 1
#define B_EXP_MIN   -B_BIAS

/* Half Precision */
#define H_SIGN_BIT  1
#define H_EXP_BIT   5
#define H_MANT_BIT  10

#define H_BIAS      ((1 << (H_EXP_BIT - 1)) - 1)

#define H_MANT_NAN  0x0000000000000200

#define H_EXP_MAX   H_BIAS + 1
#define H_EXP_MIN   -H_BIAS

/* Single Precision */
#define S_SIGN_BIT  1
#define S_EXP_BIT   8
#define S_MANT_BIT  23

#define S_BIAS      ((1 << (S_EXP_BIT - 1)) - 1)

#define S_EXP_MAX   S_BIAS + 1
#define S_EXP_MIN   -S_BIAS

#define H2S_MASK    0x00000000007FE000
#define B2S_MASK    0x00000000007F0000

#define S_MANT_MASK 0x000FFFFFE0000000

#define S_MANT_NAN  0x0000000000400000

/* Double Precision */
#define D_SIGN_BIT  1
#define D_EXP_BIT   11
#define D_MANT_BIT  52

#define D_BIAS      ((1 << (D_EXP_BIT - 1)) - 1)

#define D_EXP_MAX   D_BIAS + 1
#define D_EXP_MIN   -D_BIAS

#define D_SIGN_MASK 0x8000000000000000
#define D_EXP_MASK  0x7FF0000000000000
#define D_MANT_MASK 0x000FFFFFFFFFFFFF

#define D_MANT_NAN  0x0008000000000000

typedef struct _BF16{
    uint8_t sign;
    uint8_t exp;
    uint8_t mant;
} BF16;

typedef struct _FP16{
    uint8_t sign;
    uint8_t exp;
    uint16_t mant;
} FP16;

typedef struct _FP32{
    uint8_t sign;
    uint16_t exp;
    uint32_t mant;
} FP32;

typedef struct _FP64{
    uint8_t sign;
    uint16_t exp;
    uint64_t mant;
} FP64;

/* Function Declaration */
uint64_t shiftRound(uint64_t n, int shift);

void bf2single(BF16* f);
void half2single(FP16* f);

void printBF16(BF16* f);
void printHalfPrecision(FP16* f);
void printSinglePrecision(FP32* f);
void printDoublePrecision(FP64* f);

void convert(double n);

#endif
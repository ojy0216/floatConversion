#ifndef __F_CONVERT_H__
#define __F_CONVERT_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "warning.h"

/* bfloat16 */
#define B_SIGN_BIT  1
#define B_EXP_BIT   8
#define B_MANT_BIT  7

#define B_BIAS      ((1 << (B_EXP_BIT - 1)) - 1)

#define B_MANT_NAN  0x0000000000000040

#define B_EXP_MAX   B_BIAS + 1
#define B_EXP_MIN   -B_BIAS

#define B_SIGN_MASK 0x8000
#define B_EXP_MASK  0x7F80
#define B_MANT_MASK 0x007F

/* Half Precision */
#define H_SIGN_BIT  1
#define H_EXP_BIT   5
#define H_MANT_BIT  10

#define H_BIAS      ((1 << (H_EXP_BIT - 1)) - 1)

#define H_MANT_NAN  0x0000000000000200

#define H_EXP_MAX   H_BIAS + 1
#define H_EXP_MIN   -H_BIAS

#define H_SIGN_MASK 0x8000
#define H_EXP_MASK  0x7C00
#define H_MANT_MASK 0x03FF

/* Single Precision */
#define S_SIGN_BIT  1
#define S_EXP_BIT   8
#define S_MANT_BIT  23

#define S_BIAS      ((1 << (S_EXP_BIT - 1)) - 1)

#define S_EXP_MAX   S_BIAS + 1
#define S_EXP_MIN   -S_BIAS

#define S_SIGN_MASK 0x80000000
#define S_EXP_MASK  0x7F800000
#define S_MANT_MASK  0x007FFFFF

#define H2S_MASK    0x00000000007FE000
#define B2S_MASK    0x00000000007F0000

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

#define B2D_MASK    0x000FE00000000000
#define H2D_MASK    0x000FFC0000000000
#define S2D_MASK    0x000FFFFFE0000000

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

void printBF2Float(BF16* bf);
void printHP2Float(FP16* hp);

void printBF16(BF16* bf);
void printHalfPrecision(FP16* hp);
void printSinglePrecision(FP32* sp);
void printDoublePrecision(FP64* dp);

BF16* disassembleHexBF(uint16_t n);
FP16* disassembleHexHP(uint16_t n);
FP32* disassembleHexSP(uint32_t n);
FP64* disassembleHexDP(uint64_t n);

uint32_t assembleSP(FP32* sp);
uint64_t assembleDP(FP64* dp);

float hexSP2Float(uint32_t n);

double hexBF2Double(uint16_t n);
double hexHP2Double(uint16_t n);
double hexSP2Double(uint32_t n);
double hexDP2Double(uint64_t n);

FP32* DP2SP(FP64* dp);
FP16* DP2HP(FP64* dp);
BF16* DP2BF(FP64* dp);

FP32* BF2SP(BF16* bf);
FP32* HP2SP(FP16* hp);

void convert(double n);

#endif
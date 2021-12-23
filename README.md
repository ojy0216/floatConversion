# Floating point number Converter

C implementation of converting floating point number into various formats(FP16, FP32, FP64, bfloat16) and displays into binary, hexadecimal format.

## Features
* Floating points conversion to FP16, FP32, FP64, BF16
* Displays sign, exponent, mantissa bits
* Displays hexadecimal representation of floating points
* Displays converted exact floating point value
* Input can be floating point number or hexadecimal representation of floating point number()
## Usage
Include "fConvert.h" header and compile with "fConvert.c" file
```
#include "fConvert.h"
```

Function convert() gets double(FP64) inputs and displays conversion results.
```
convert(3.141592654);
```

## Example(main.c)
1. Floating point input
```
./main 3.141592654
```
![fig1](https://user-images.githubusercontent.com/47859342/147200911-19a7eb0f-0ac1-4d46-af5d-7c09fe50b8fd.png)

2. Hexadecimal input(w/ -h, -s, -d, -b option)
```
./main 402df84d -s 
```
![fig2](https://user-images.githubusercontent.com/47859342/147200912-1ad0dd1d-9883-4934-88e1-f9bb1dc55245.png)
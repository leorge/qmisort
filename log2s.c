/*
 ============================================================================
 Name        : log2.c
 Author      : Leorge Takeuchi
 Version     : 1.0.0
 Copyright   : free
 Description : int log2N(size_t n) in C, Ansi-style
 Options     : -O3 -std=c99  -lm  [-DDEBUG]
 ============================================================================
 */

#include <math.h>
#include "log2s.h"

int log2A(TYPE n)	// use inline Assembly
{	// BSR (Bit Search Reverse) : Get highest "1" bit position. LSB --> 0
	int	rtn = -1;
	if (n)	// Destination operand of BSR is undefined if n == 0.
		__asm__(
			"bsrq %%rdi,%%rax\n\r"	// search in 64 bit DI register --> 64 bit A register
			"movl %%eax, %0\n\r"	// lower 32 bit of A register --> rtn
			: "=r" (rtn)
			:
			: "rax");
	return	rtn;
}

int log2B(TYPE n)
{	// Binray search
	int	rtn = -1;
	if (n) {
		int width = 8 * sizeof(TYPE);
		rtn = width - 1;
		TYPE pattern = ~(TYPE)0;
		for (pattern <<= (width >> 1) ; width > 1 ; )
		{
			TYPE    found = pattern & n;
			pattern &= pattern << ((width >>= 1) >> 1);
			if (found) continue;
			rtn -= width;
			pattern >>= width;
		}
	}
	return  rtn;
}

int log2G(TYPE n)
{	// GCC , GNU
	return  n ? (int)log2((double)n) : -1;
}

int log2R(TYPE n)
{	// Right shift count while n > 0.
	int rtn = -1;
	for (; n ; n >>= 1) rtn++;
	return  rtn;
}

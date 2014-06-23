#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef CRCINIT
#define CRCINIT 0xFFFFFFFF
#endif

#ifdef __clang__
#define HAVE_INTTYPES_H 1
#define HAVE_X86INTRIN_H 1
#endif

#if HAVE_INTTYPES_H
#include <inttypes.h>
#elif HAVE_STDINT_H
#include <stdint.h>
#else
#error crc32.c requires a definition of uint32_t, uint64_t
#endif

#if HAVE_X86INTRIN_H
#include <x86intrin.h>
#else
# error FIXME: Make a non-hardware version of crc32
#endif

uint64_t crc32_64(void* restrict msg)
{
    uint32_t* restrict buf = (uint32_t* restrict) msg;
	uint64_t crc = CRCINIT;

    #if HAVE_X86INTRIN_H
    crc = _mm_crc32_u32(crc, buf[0]);
    crc = _mm_crc32_u32(crc, buf[1]);
    #endif

	return ~crc;
}

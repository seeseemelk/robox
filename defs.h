#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdint.h>

#define MASK(bit) (1 << bit)

#define TEST_BIT_SET(var, bit) ((var & MASK(bit)) != 0)
#define SET_BIT(var, bit) var |= MASK(bit)
#define CLEAR_BIT(var, bit) var &= ~MASK(bit)

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

#endif

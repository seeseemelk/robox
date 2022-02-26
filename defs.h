#ifndef _DEFS_H_
#define _DEFS_H_

#define MASK(bit) (1 << bit)

#define TEST_BIT_SET(var, bit) ((var & MASK(bit)) != 0)
#define SET_BIT(var, bit) var |= MASK(bit)
#define CLEAR_BIT(var, bit) var &= ~MASK(bit)

#endif

#ifndef __UTILS_MACROS_H__
#define __UTILS_MACROS_H__

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof((x)[0]))
#define PIN_SELECT(pin) (1ULL << (pin))

#endif
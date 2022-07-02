#pragma once
#include <assert.h>

#ifdef DEBUG

#define OLA_ASSERT(expression, statement) (void)(                                             \
            (!!(expression)) ||                                                              \
            (_wassert(_CRT_WIDE(statement), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0)  \
        )

#else

#define OLA_ASSERT(expression, statement)

#endif
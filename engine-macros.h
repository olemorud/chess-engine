#pragma once

#define STR(x) #x

#if !defined(NDEBUG) && defined(USE_PRINTF)
#define assuming(expr) \
            ((expr) ? 0 : (fprintf(stderr, "assumption <" #expr "> failed on line %d\n", __LINE__), __builtin_trap(), 0))
#elif !defined(NDEBUG)
#define assuming(expr) \
            ((expr) ? 0 : (__builtin_trap(), 0))
#else
#define assuming(expr) ((expr) ? 0 : (__builtin_unreachable(), 0))
#endif

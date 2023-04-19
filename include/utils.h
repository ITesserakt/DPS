#pragma once

#include <stdio.h>
#include <stdlib.h>

#if DEBUG
#define DEBUG_PRINT(file, format, ...) fprintf(file, format, __VA_ARGS__)
#else
#define DEBUG_PRINT(...) 0
#endif

/// Checks if pointer is not null, otherwise returns [default] value and prints debug message to stderr (in debug mode
/// only)
#define RETURN_DEFAULT_IF_NULL(ptr, default)                                                                           \
    if ((ptr) == NULL) {                                                                                               \
        DEBUG_PRINT(stderr, "DEBUG: %s was NULL at %s:%d\n", #ptr, __FILE__, __LINE__);                                \
        return (default);                                                                                              \
    }                                                                                                                  \
    0

/// Checks if expr is non-negative integer, otherwise returns [on_error] code and prints debug message to stderr (in
/// debug mode only)
#define TRY_OR_ELSE(expr, on_error)                                                                                    \
    {                                                                                                                  \
        int code = expr;                                                                                               \
        if ((code) < 0) {                                                                                              \
            DEBUG_PRINT(stderr, "DEBUG: %s was lead to error %d at %s:%d\n", #expr, code, __FILE__, __LINE__);           \
            return on_error;                                                                                           \
        }                                                                                                              \
    }                                                                                                                  \
    0

/// Checks if expr is non-negative integer, otherwise forwards its error return code and prints debug message to stderr
/// (in debug mode only)
#define TRY_FORWARD(expr)                                                                                              \
    {                                                                                                                  \
        int code = expr;                                                                                               \
        if ((code) < 0) {                                                                                              \
            DEBUG_PRINT(stderr, "DEBUG: %s was lead to error %d at %s:%d\n", #expr, code, __FILE__, __LINE__);           \
            return code;                                                                                               \
        }                                                                                                              \
    }                                                                                                                  \
    0

/// Used in macro overloading
#define __TRY_OVERLOAD(_1, _2, name, ...) name

/// Checks if expr is non-negative integer, otherwise does action based on overload
/// SEE ALSO: [TRY_OR_ELSE], [TRY_FORWARD]
#define TRY(...) __TRY_OVERLOAD(__VA_ARGS__, TRY_OR_ELSE, TRY_FORWARD)(__VA_ARGS__)

/// Shortcut for write(..., <string literal>, sizeof(<same string literal>))
#define WRITE(fmt, message) write((fmt), (message), sizeof(message))

/// Shortcut for sizeof(<array type>) / sizeof(<type of item of array type>)
#define ARRAY_LEN(array) (sizeof(array) / sizeof(*(array)))
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <quadmath.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef size_t usz;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#ifdef __SIZEOF_INT128__
typedef __int128_t i128;
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#ifdef __SIZEOF_INT128__
typedef __uint128_t u128;
#endif

typedef _Float16 f16;
typedef float f32;
typedef double f64;

#ifdef __SIZEOF_FLOAT128__
typedef __float128 f128;
#endif

typedef struct SHUOwner
{
    void *data;
    size_t size;
} SHUOwner;

typedef struct SHUView
{
    const void *const data;
    const usz size;
} SHUView;

#define SHU_NON_HANDLED_ERROR(type, target, code, function)                                                  \
    do                                                                                                       \
    {                                                                                                        \
        fprintf(stderr, "ERROR : Non-handled error detected. '%s', '%s', '%s'.", #type, #target, #function); \
        exit(code);                                                                                          \
    } while (0)

#define SHUResult(type, ...) \
    struct SHUResult_##type  \
    {                        \
        bool ok;             \
        union                \
        {                    \
            type _data;      \
            enum SHUError_   \
            {                \
                __VA_ARGS__  \
            } _error;        \
        } option;            \
    }
// todo error enum name too

#define SHURetOK(type, data) \
    (struct SHUResult_##type) { .ok = true, .option._data = data }

#define SHURetErr(type, error) \
    (struct SHUResult_##type) { .ok = false, .option._error = error }

#define SHUGet(type, target, code, function, ...)                                \
    do                                                                           \
    {                                                                            \
        struct SHUResult_##type result = function;                               \
        if (result.ok)                                                           \
        {                                                                        \
            target = result.option._data;                                        \
        }                                                                        \
        else if (0 __VA_OPT__(+1))                                               \
        {                                                                        \
            __VA_ARGS__                                                          \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            SHU_NON_HANDLED_ERROR(type, target, result.option._error, function); \
        }                                                                        \
    } while (0)

SHUResult(SHUOwner, ALLOCATION) SHU_Allocate(usz size)
{
    SHUOwner owner = {0};
    owner.size = size;
    owner.data = malloc(owner.size);

    if (owner.data == NULL)
    {
        return SHURetErr(SHUOwner, ALLOCATION);
    }

    return SHURetOK(SHUOwner, owner);
}

int main(int argc, char **argv)
{
    SHUOwner test;
    int errCode;
    SHUGet(SHUOwner, test, errCode, SHU_Allocate(-1));

    /*
    char *const data = malloc(1);
    valid
    owner variable, can mutate, cant change
    handled by compiler

    *data = 0; will not warn
    data = NULL; will warn
    */

    /*
    char *copy = data;
    !invalid
    a second owner variable
    allowed by compiler

    *copy = 0; will not warn
    copy = NULL; will not warn
    */

    /*
    const char *const view1 = data;
    valid
    viewer variable, cant mutate, cant change
    handled by compiler

    *view1 = 0; will warn
    view1 = NULL; will warn
    */

    /*
    const char *view2 = data;
    !invalid
    viewer variable, cant mutate, can change
    allowed by compiler

    *view2 = 0; will warn
    view2 = NULL; will not warn
    */

    return 0;
}
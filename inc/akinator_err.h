#ifndef AKINATOR_ERR_H
#define AKINATOR_ERR_H

#include <string.h>

enum akinator_err_t {
    AR_ERR_OK                       = 0ull,
    AR_ERR_FILE_OPEN                = 1ull << 0,
    AR_ERR_ALLOC                    = 1ull << 1,
    AR_ERR_SYSTEM                   = 1ull << 2,
    AR_ERR_STACK                    = 1ull << 3,
    AR_ERR_CYCLED                   = 1ull << 4,
    AR_ERR_REALLOC                  = 1ull << 5,
    AR_ERR_NULLPTR                  = 1ull << 6,
    AR_ERR_NOT_ENOUGH_FEATURES      = 1ull << 7,
    AR_ERR_GET_DESCR                = 1ull << 8,
};

const size_t ERR_DESCR_MAX_SIZE = 128;

void akinator_err_add(enum akinator_err_t *dest, enum akinator_err_t add);
void akinator_err_get_descr(const enum akinator_err_t err_code, char err_descr_str[]);

#ifdef _DEBUG
    #define DEBUG_AR_LIST_ERROR(err_code, str_, ...) {                                                      \
        char akinator_err_desr_str[ERR_DESCR_MAX_SIZE];                                                    \
        akinator_err_get_descr(err_code, akinator_err_desr_str); \
        printf("DESCR: '%s'\n", akinator_err_desr_str);                                                      \
        fprintf_red(stderr, "{%s} [%s: %d]: err_descr: {%s}, message: {" str_ "}\n",              \
             __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, akinator_err_desr_str, ##__VA_ARGS__);                    \
        fprintf(stderr, WHT); \
    }
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define DEBUG_AR_LIST_ERROR(err_code, str_, ...) ;
#endif // _DEBUG


#endif // AKINATOR_ERR_H
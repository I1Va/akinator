#include <stdio.h>

#include "akinator_err.h"

void akinator_err_add(enum akinator_err_t *dest, enum akinator_err_t add) {
    *dest = (akinator_err_t)((unsigned long long)(*dest) | (unsigned long long) add);
}

void akinator_err_get_descr(const enum akinator_err_t err_code, char err_descr_str[]) {
    bool error = false;
    #define DESCR_(err_code, err)                 \
        {                                         \
            if (err_code & err) {                 \
                sprintf(err_descr_str, #err", "); \
                error = true;                     \
            }                                     \
        }                                         \

    DESCR_(err_code, AR_ERR_FILE_OPEN);
    DESCR_(err_code, AR_ERR_ALLOC);
    DESCR_(err_code, AR_ERR_SYSTEM);
    DESCR_(err_code, AR_ERR_STACK);
    DESCR_(err_code, AR_ERR_CYCLED);
    DESCR_(err_code, AR_ERR_REALLOC);
    DESCR_(err_code, AR_ERR_NULLPTR);
    DESCR_(err_code, AR_ERR_NOT_ENOUGH_FEATURES);
    DESCR_(err_code, AR_ERR_GET_DESCR);

    if (!error) {
        sprintf(err_descr_str, "ALL IS OK:)");
    }
    #undef DESCR_
}
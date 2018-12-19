#ifndef PROP_H_OLIVECC
#define PROP_H_OLIVECC

#include <stdbool.h>

//should be possible to increase this up to ((SIZE_MAX + 1) / 2); untested
enum { MAX_FORMULA_LENGTH = 50 };

bool is_formula_legal(const char* fm_str, size_t fm_len);

bool is_formula_satisfiable(const char* fm_str, size_t fm_len);

#endif //PROP_H_OLIVECC

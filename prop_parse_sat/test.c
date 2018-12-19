#include <stdbool.h>
#include <stdio.h>      //printf
#include <string.h>     //strlen

#include "prop.h"

int main(int argc, char** argv)
{
    for(size_t i = 1; i < argc; ++i)
    {
        const char* str = argv[i];
        const size_t len = strlen(str);
        const char* msg = (is_formula_legal(str, len)
           ? (is_formula_satisfiable(str, len) 
               ? "satisfiable" 
               : "not satisfiable")
           : "not a legal formula");
        printf("\"%s\" is %s\n", str, msg); 
    }
}

#include <stdio.h>

#include "prop_parse.h"

int main(int argc, char** argv)
{
    const char* str_type[] = 
    {
        "not a valid propositional formula", 
        "a proposition", 
        "a negation", 
        "a binary formula"
    };

    for(size_t i = 1; i < argc; ++i)
    {
        char* str = argv[i];
        printf("\"%s\" is %s\n", str, str_type[parse(str)]); 
    }
}

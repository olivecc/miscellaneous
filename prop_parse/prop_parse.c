#include <string.h>     //size_t, strlen

#include "prop_parse.h"

typedef enum {false, true} bool;

typedef enum fm_type_e
{
    FM_INVALID,
    FM_PROPOSITION,
    FM_NEGATION,
    FM_BINARY
} fm_type;

size_t find_binary_connective(char* g, size_t start, size_t end)
{
    int parenthesis_count = 0;

    for(size_t i = start + 1; i < end; i++)
    {
        switch(g[i])
        {
            case('('):
                parenthesis_count++;
                break;

            case(')'):
                parenthesis_count--;
                break;

            case('v'):
            case('^'):
            case('>'):
                if(parenthesis_count == 0)
                    return i;
                break;
        }
    }

    //if no binary connective found
    return start;
}

//forward declaration for use in is_binary_formula
fm_type parse_impl(char*, size_t, size_t);

bool is_binary_formula(char* g, size_t start, size_t end)
{
    if(g[start] != '(' || g[end] != ')')
        return false;
    //has at least two characters ('(', ')')

    size_t middle = find_binary_connective(g, start, end);
    if(middle <= start + 1 || end <= middle + 1)
        return false;
    //has at least five characters ('(', ?, 'b', ?, ')')

    return (parse_impl(g, start + 1,  middle - 1) != FM_INVALID &&
            parse_impl(g, middle + 1, end - 1)    != FM_INVALID);
}

//precondition: start <= end
fm_type parse_impl(char* g, size_t start, size_t end)
{
    switch(g[start])
    {
        case('p'):
        case('q'):
        case('r'):
            return (end == start) 
                ? FM_PROPOSITION 
                : FM_INVALID;
            break;

        case('-'):
            return (parse_impl(g, start + 1, end) != FM_INVALID) 
                ? FM_NEGATION
                : FM_INVALID;
            break;
    
        case('('):
            return (is_binary_formula(g, start, end))
                ? FM_BINARY
                : FM_INVALID;
            break;

        default:
            return FM_INVALID;
    }
}

int parse(char* g)
{
    size_t len = strlen(g);
    if(len == 0)
        return FM_INVALID;

    return parse_impl(g, 0, len - 1);
}

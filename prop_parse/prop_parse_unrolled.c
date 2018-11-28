#include <math.h>       //ceil, log2
#include <stdio.h>      //fprintf, stderr
#include <stdlib.h>     //free, malloc
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

typedef struct pair_s
{
    size_t fst;
    size_t snd;
} pair;

typedef struct two_pair_s
{
    pair fst;
    pair snd;
} two_pair;


fm_type formula_type(char* g, size_t start)
{
    switch(g[start])
    {
        case('p'):
        case('q'):
        case('r'):
            return FM_PROPOSITION;
            break;

        case('-'):
            return FM_NEGATION;
            break;

        case('('):
            return FM_BINARY;
            break;

        default:
            return FM_INVALID;
            break;
    }
}


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


two_pair binary_formula_layout(char* g, size_t start, size_t end)
{
    const two_pair bad_bf = {{0,0},{0,0}};
    
    if(g[start] != '(' || g[end] != ')')
        return bad_bf;
    //has at least two characters ('(', ')')

    size_t middle = find_binary_connective(g, start, end);
    if(middle <= start + 1 || end <= middle + 1)
        return bad_bf;
    //has at least five characters ('(', ?, 'b', ?, ')')

    return (two_pair){{start + 1, middle - 1}, {middle + 1, end - 1}};
}

bool is_valid_formula(char* g, size_t start, size_t end)
{
    if(start > end)
        return FM_INVALID;
    size_t len = end + 1 - start;
    
    //worst-case length of parse queue, where the formula consists 
    //entirely of binary connectives, is (or _should_ be) n*log(n)
    const size_t log_two_len = (size_t)ceil(log2((double)len));
    const size_t parse_queue_sz = (len + 1) * (log_two_len + 1);

    pair* parse_queue = malloc(parse_queue_sz * sizeof(pair));
    if(!parse_queue)
    {
        fprintf(stderr, "Error: could not allocate parse queue\n");
        return false;
    }

    parse_queue[0] = (pair){start, end};
    
    size_t head_index = 0;
    size_t tail_index = 1;

    while(head_index < tail_index && tail_index < parse_queue_sz)
    {
        start = parse_queue[head_index].fst;
        end   = parse_queue[head_index].snd;

        two_pair bf_layout = {};
        switch(formula_type(g, start))
        {
            case(FM_PROPOSITION):
                if(end != start)            //exactly 1 char
                    return false;
                break;

            case(FM_NEGATION):
                if(start + 1 > end)         //minimum 2 chars
                    return false;
                parse_queue[tail_index++] = (pair){start + 1, end};
                break;
        
            case(FM_BINARY):
                if(start + 4 > end)         //minimum 5 chars
                    return false;
                bf_layout = binary_formula_layout(g, start, end);
                if(bf_layout.fst.fst == bf_layout.snd.snd)      //bad layout
                    return false;
                parse_queue[tail_index++] = bf_layout.fst;
                if(tail_index < parse_queue_sz)
                    parse_queue[tail_index++] = bf_layout.snd;
                break;

            default:
                return false;
        }
        head_index++;
    }

    free(parse_queue);

    if(tail_index < parse_queue_sz)
        return true;
    else
    {
        fprintf(stderr, "Error: max length of parse queue exceeded: %d > %d\n", 
                tail_index, parse_queue_sz - 1);
        return false;
    }
}

int parse(char* g)
{
    return (is_valid_formula(g, 0, strlen(g) - 1)) 
        ? formula_type(g,0)
        : FM_INVALID;
}

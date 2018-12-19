#include <stdlib.h>     //size_t
#include <stdbool.h>
#include "prop.h"

enum { TABLEAU_SZ = 2 * MAX_FORMULA_LENGTH };

typedef enum prop_fm_type_e
{
    FM_ILLEGAL,
    FM_PROPOSITION,
    FM_NEGATION,
    FM_BINARY
} prop_fm_type;

prop_fm_type formula_type(char first_char)
{
    switch(first_char)
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
            return FM_ILLEGAL;
            break;
    }
}

size_t find_binary_connective(const char* fm_str, size_t fm_len)
{
    size_t opened_count = 0;
    size_t closed_count = 0;

    for(size_t i = 1; i < fm_len - 1; ++i)
    {
        switch(fm_str[i])
        {
            case('('):
                ++opened_count;
                break;

            case(')'):
                ++closed_count;
                break;

            case('v'):
            case('^'):
            case('>'):
                if(opened_count == closed_count)
                    return i;
                break;
        }
    }

    //if no binary connective found
    return 0;
}

typedef struct fm_s
{
    const char* str;
    size_t len;
} fm;

typedef struct fm_pair_s
{
    fm fst;
    fm snd;
} fm_pair;


fm_pair binary_formula_layout(const char* fm_str, size_t fm_len)
{
    const fm_pair bad_bf = { { NULL, 0 }, { NULL, 0 } };
    
    if(fm_str[0] != '(' || fm_str[fm_len - 1] != ')')
        return bad_bf;
    //has at least two characters ('(', ')')

    size_t middle = find_binary_connective(fm_str, fm_len);
    if(middle == 0)
        return bad_bf;
    //has at least five characters ('(', ?, 'b', ?, ')')

    return (fm_pair)
        { 
            { fm_str + 1,             middle - 1                }, 
            { fm_str + (middle + 1),  fm_len - (middle + 1) - 1 }
        };
}

bool is_formula_legal(const char* fm_str, size_t fm_len)
{
    if(fm_len == 0)
        return false;

    fm parse_queue[TABLEAU_SZ] = { fm_str, fm_len };
    
    size_t head_index = 0;
    size_t tail_index = 1;

    const char* cur_fm_str = NULL;
    size_t      cur_fm_len = 0;

    while(head_index < tail_index)
    {
        cur_fm_str = parse_queue[head_index].str;
        cur_fm_len = parse_queue[head_index].len;

        switch(formula_type(cur_fm_str[0]))
        {
            case(FM_PROPOSITION):
            {
                if(cur_fm_len != 1)
                    return false;
                break;
            }

            case(FM_NEGATION):
            {
                if(cur_fm_len < 2)
                    return false;
                parse_queue[tail_index++] = 
                    (fm){ cur_fm_str + 1, cur_fm_len - 1 };
                break;
            }
        
            case(FM_BINARY):
            {
                if(cur_fm_len < 5)
                    return false;
                fm_pair bf = binary_formula_layout(cur_fm_str, cur_fm_len);
                if(bf.fst.str == NULL)     //bad binary formula
                    return false;
                parse_queue[tail_index++] = bf.fst;
                parse_queue[tail_index++] = bf.snd;
                break;
            }

            default:
                return false;
        }
        ++head_index;
    }
    return true;
}

typedef struct tableau_node_s
{
    const char* fm_str;
    size_t      fm_len;
    size_t      branch_ID;
    bool        negated;
} tableau_node;

typedef enum branch_status_e
{
    BR_INACTIVE,
    BR_OPEN,
    BR_NONLEAF,
    BR_CLOSED
} branch_status;

typedef enum prop_status_e
{
    PROP_UNKNOWN,
    PROP_TRUE,
    PROP_FALSE
} prop_status;

typedef struct branch_info_s
{
    branch_status status;
    prop_status p;
    prop_status q;
    prop_status r;
} branch_info;


size_t left_subbranch_index(size_t b)
{
    return (2 * b) + 1;
}

size_t right_subbranch_index(size_t b)
{
    return 2 * (b + 1);
}

size_t fill_osb(size_t* osb_queue, size_t top_br, branch_info* br_info)
{
    size_t sb_queue[TABLEAU_SZ] = { top_br };
    size_t sb_head = 0;
    size_t sb_tail = 1;
    size_t osb_tail = 0;

    while(sb_head < sb_tail)
    {
        size_t cur_br_index = sb_queue[sb_head];
        branch_info cur_br = br_info[cur_br_index];
        
        if(cur_br.status == BR_OPEN)
        {
            osb_queue[osb_tail++] = cur_br_index;
        } 
        else if (cur_br.status == BR_NONLEAF)
        {
            size_t left_sb_index = left_subbranch_index(cur_br_index);
            size_t right_sb_index = right_subbranch_index(cur_br_index);

            for(unsigned int i = 0; i < 2; ++i)
            {
                size_t index = (size_t[]){left_sb_index, right_sb_index}[i];

                if((index < TABLEAU_SZ) && 
                   ((br_info[index].status == BR_OPEN) ||
                    (br_info[index].status == BR_NONLEAF)))
                {
                    sb_queue[sb_tail++] = index;
                }
            }
        }
        ++sb_head;
    }
    return osb_tail;
}

size_t negate(const char** fm_str, size_t* fm_len, bool* negated)
{
    while(formula_type((*fm_str)[0]) == FM_NEGATION)
    {
        ++(*fm_str);
        --(*fm_len);
        *negated = !(*negated);
    }
}

//precondition: the first fm_len characters of fm_str form a legal 
//propositional formula
bool is_formula_satisfiable(const char* fm_str, size_t fm_len)
{
    bool initial_negation = false;
    negate(&fm_str, &fm_len, &initial_negation);

    if(formula_type(fm_str[0]) == FM_PROPOSITION)
        return 1;

    //now, formula cannot be a negation/proposition, hence is binary

    tableau_node queue[TABLEAU_SZ] = { fm_str, fm_len, 0, initial_negation };
    
    branch_info br_info[TABLEAU_SZ] = 
    { { BR_OPEN, PROP_UNKNOWN, PROP_UNKNOWN, PROP_UNKNOWN } };   

    size_t head = 0;
    size_t tail = 1;
    size_t open_branch_quantity = 1;

    const char* cur_fm_str = NULL;
    size_t      cur_fm_len = 0;

    //invariant: all tableau elements are either binary formulae or propositions
    while((head < tail) && (open_branch_quantity > 0))
    {
        cur_fm_str = queue[head].fm_str;
        cur_fm_len = queue[head].fm_len;

        if(formula_type(cur_fm_str[0]) == FM_BINARY)
        {
            size_t bc_index = find_binary_connective(cur_fm_str, cur_fm_len);
            
            char bc = cur_fm_str[bc_index];
            
            bool left_fm_neg = (bc == '>');
            bool right_fm_neg = false;
            bool is_expansion_alpha = (bc == '^');

            if(queue[head].negated)     //propositional duality
            {
                left_fm_neg  = !left_fm_neg;
                right_fm_neg = !right_fm_neg;
                is_expansion_alpha  = !is_expansion_alpha;
            }

            fm_pair lr_fm = binary_formula_layout(cur_fm_str, cur_fm_len);

            negate(&(lr_fm.fst.str), &(lr_fm.fst.len),  &left_fm_neg);
            negate(&(lr_fm.snd.str), &(lr_fm.snd.len), &right_fm_neg);
            
            size_t open_subbranches[TABLEAU_SZ];
            size_t osb_quantity = fill_osb(
                    open_subbranches,
                    queue[head].branch_ID, 
                    br_info);

            for(size_t osb_head = 0; osb_head < osb_quantity; ++osb_head)
            {
                size_t branch = open_subbranches[osb_head];

                size_t  left_sfm_b = 0;
                size_t right_sfm_b = 0;

                if(is_expansion_alpha)  //alpha expansion
                {
                    left_sfm_b  = branch;
                    right_sfm_b = branch;
                }
                else                    //beta expansion
                {
                    left_sfm_b  = left_subbranch_index(branch);
                    right_sfm_b = right_subbranch_index(branch);
                    
                    branch_info old_br_info = br_info[branch];
                    
                    br_info[left_sfm_b]  = old_br_info;
                    br_info[right_sfm_b] = old_br_info;
                    
                    br_info[branch].status = BR_NONLEAF;
                    ++open_branch_quantity;
                }

                for(unsigned int i = 0; i < 2; ++i)
                {
                    fm new_fm;
                    size_t new_br;
                    bool new_neg;

                    new_fm     = (fm[2])    { lr_fm.fst,   lr_fm.snd    }[i];
                    new_br     = (size_t[2]){ left_sfm_b,  right_sfm_b  }[i];
                    new_neg    = (bool[2])  { left_fm_neg, right_fm_neg }[i];

                    queue[tail++] = (tableau_node) 
                    {
                        new_fm.str, 
                        new_fm.len, 
                        new_br, 
                        new_neg
                    };

                    if(formula_type(new_fm.str[0]) == FM_PROPOSITION)
                    {
                        prop_status* prop = NULL;
                        branch_info* new_br_info = &br_info[new_br];
                        switch(new_fm.str[0])
                        {
                            case('p'):
                                prop = &(new_br_info->p);
                                break;

                            case('q'):
                                prop = &(new_br_info->q);
                                break;

                            case('r'):
                                prop = &(new_br_info->r);
                                break;
                        }
                        if(*prop == PROP_UNKNOWN)
                        {
                            *prop = (new_neg ? PROP_FALSE : PROP_TRUE);
                        }
                        else if(((*prop == PROP_TRUE)  &&  new_neg) ||
                                ((*prop == PROP_FALSE) && !new_neg))
                        {
                            --open_branch_quantity;
                            new_br_info->status = BR_CLOSED;
                        }
                    }
                }
            }
        }
        ++head;
    }
    return open_branch_quantity;
}

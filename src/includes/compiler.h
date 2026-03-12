#ifndef COMPILER_H
#define COMPILER_H


#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "global_helper.h"
#include "opcodes.h"

// forward declaration
typedef struct Expr Expr;

typedef enum {
    EXPR_INT,
    EXPR_CHAR,
    EXPR_BOOL,
    EXPR_SYMBOL,
    EXPR_LIST
} ExprType;

// A list of expressions (for nested lists)
typedef struct {
    Expr **items; // array of pointers to expression
    size_t count;
    size_t capacity;
} ExprList;

// The expression itself
struct Expr {
    ExprType type;
    int plex;
    union {
        int64_t int_val;        // EXPR_INT
        int64_t char_val;       // EXPR_CHAR
        int64_t bool_val;       // EXPR_BOOL
        int64_t mt_list;
        char *symbol;        // EXPR_SYMBOL
        ExprList list;          // EXPR_LIST
    } as;
};

// Struct for the environment variable
struct Val {
    char *symbol;
    Expr *closure;
    int64_t stack_location;
};

typedef struct Env {
    struct Val *val;
    size_t count;
    size_t capacity;
    int type;
    struct Env *parent; 
} Env;

struct Ret {
    int type;
    int64_t stack_pos;
    int depth;
};

typedef struct Label{
    Int64_Array *from;
    Int64_Array *to;
    int64_t offset;
    int64_t index;
} Label;

typedef struct UnresolvedJmp {
    Label *l;
    size_t size;
    size_t capacity;
} UnresolvedJmp;

Int64_Array *flow_segment;
Int64_Array *label_segment;
Int64_Array *constant_segment;
Int64_Array *init_segment;
UnresolveJmp *j;
int64_t global_stackPos;
int64_t stack_pointer;
static size_t lambdacount = 0;
static size_t ccount = 0;

/*
 *  Function Declaration
 */
void Compiler(Expr *parsed, Env *env, Env *labelEnv, Int64_Array *segment);
// Unary Primitives
void compile_list(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_add1(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_sub1(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_int2char(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_char2int(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_nullp(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_zerop(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_not(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_intp(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_boolp(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
// Binary Primitives
void compile_add(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_mul(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_sub(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_le(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_eq(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_eqs(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment); 
// Local variables
void compile_let(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
// conditionals
void compile_if(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
// Pairs: cons, car, cdr
void compile_cons(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_car(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_cdr(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
// string
void compile_string(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_stringRef(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_stringSet(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_stringAppend(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
// vector
void compile_vector(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_vectorRef(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_vectorSet(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_vectorAppend(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
// Begin
void compile_begin(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
// Lambda stuff
void compile_label(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_code(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_closure(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void funcall(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void compile_var(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment); 
Expr* create_closure(Expr *list, Env *env, Env *labelEnv, int64_t entry);
void tailcall(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
void ccconstant(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment);
Expr * create_complex_ref(int64_t loc);
Expr *complex_init(Expr *list, int64_t loc);
void compile_c_ref(Expr *list, Int64_Array *segment);
void compile_c_init(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment, int64_t loc);
// Env
Env *initializeEnv();
void init_binding(Env *env, char *symbol);
void add_binding(Env *env, char *symbol, int64_t stack_location);
int64_t lookup(Env *env, char *symbol);
int64_t  recurse_env(Env *env, size_t index);
// Helper
void add_to_list(ExprList *list, Expr *item);
void display_parsed_list(Expr *parsed);
void free_expr(Expr *parsed);
void free_env(Env *env);
struct Ret getEnv(Env *env, char *symbol);
char* gen_lambdaName();
void inc_loc(Env *env);
void dec_loc(Env *env);
void collect_free_vars(Expr *expr, Env *boundedEnv, Env *freeEnv, Env *lexicalEnv); 
bool isbuiltin(const char *symbol);
void print_env(Env *env, const char *name);

Env* initializeEnv() {
    Env *env = malloc(sizeof(Env));
    env->count = 0;
    env->capacity = 8;
    env->val = calloc(env->capacity, sizeof(struct Val));
    env->type = 0;
    env->parent = NULL;
    return env;
}

// typedef struct Label{
//     Int64_Array *from;
//     Int64_Array *to;
//     int64_t offset;
//     int64_t index;
// };
//
// typedef struct UnresolvedJmp {
//     Label *l;
//     size_t size;
//     size_t capacity;
// };

UnresolvedJmp *initializeJmp() {
    UnresolvedJmp *j = malloc(sizeof(UnresolvedJmp));
    j->size = 0;
    j->capacity = 1;
    j->l = calloc(j->capacity, sizeof(int64_t));
    return j;
}

void inc_loc(Env *env) {

    if (env == NULL) return;

    if (env->type == 0) {
        for (size_t i = 0; i < env->count; i++) {
            env->val[i].stack_location += 1;
        }
    }

    if (env->parent != NULL) {
        inc_loc(env->parent);
    }
}

void dec_loc(Env *env) {
    if (env == NULL) return;

    if (env->type == 0) {
        for (size_t i = 0; i < env->count; i++) {
            if (env->val[i].stack_location > 1) {
                env->val[i].stack_location -= 1;
            }
        }
    }

    if (env->parent != NULL) {
        dec_loc(env->parent);
    }
}

int64_t count_loc(Env *env) {

    if (env == NULL) return 0;
    if (env->type == 0){
        return env->count;
    } else if (env->type == 2) {
        return env->count + 2;
    }
    return 0;
}

struct Ret getEnv(Env *env, char *symbol) {

    const char *s1 = symbol;
    for (size_t i = 0; i < env->count; i++) {
        const char *s2 = env->val[i].symbol;
        if (strcmp(s1, s2) == 0) {
            struct Ret r = { .type = env->type, .stack_pos = (env->val[i].stack_location)};
            return r;
        }
    }

    if (env->parent != NULL) {
        return getEnv(env->parent, symbol);
    }

    struct Ret r = { .type = -1, .stack_pos = -1};
    return r;

}


void init_binding(Env *env, char *symbol) {
    if (env->count >= env->capacity) {
        env->capacity *= 2;
        env->val = realloc(env->val, env->capacity * sizeof(struct Val));
        if (env->val == NULL) {
            printf("Error: allocation failure\n");
            exit(-1);
        }
    }

    const char *s1 = symbol;
    int64_t stack_pos = lookup(env, symbol);
    if (stack_pos == -1 ) {
        env->val[env->count].symbol = calloc(strlen(symbol)+1, sizeof(char));
        if (env->val[env->count].symbol == NULL) {
            printf("Error: allocation failure\n");
            exit(-1);
        }
        strcpy(env->val[env->count].symbol, s1);
        env->val[env->count].stack_location = 1;
        env->count++;
    }
}

void addJ(UnresolvedJmp *j, Label l) {
    if (j->size >= j->capacity) {
        j->capacity = j->capacity * 2;

        j->
}


void add_binding(Env *env, char *symbol, int64_t location) {
    if (env->count >= env->capacity) {
        env->capacity *= 2;
        env->val = realloc(env->val, env->capacity * sizeof(struct Val));
        if (env->val == NULL) {
            printf("Error: allocation failure\n");
            exit(-1);
        }
    }

    const char *s1 = symbol;
    int64_t stack_pos = lookup(env, symbol);


    if (stack_pos == -1 ) {
        env->val[env->count].symbol = calloc(strlen(symbol)+1, sizeof(char));
        if (env->val[env->count].symbol == NULL) {
            printf("Error: allocation failure\n");
            exit(-1);
        }
        strcpy(env->val[env->count].symbol, s1);
        env->val[env->count].stack_location = location;
        env->count++;
    }
}


int64_t lookup(Env *env, char *symbol) {
    const char *s1 = symbol;
    for (size_t i = 0; i < env->count; i++) {
        const char *s2 = env->val[i].symbol;
        if (strcmp(s1, s2) == 0) {
            return i;
        }

    }
    if(env->parent != NULL) {
        return lookup(env->parent, symbol);
    }
    return -1;

}

/*
 * HELPERS 
 */
bool isbuiltin(const char *symbol) {
    static const char *builtins[] = {
        "+", "-", "*", "<", "=",
        "add1", "sub1", "cons", "car", "cdr",
        "null?", "zero?", "integer?", "boolean?", "not",
        "if", "funcall", "begin", "closure",
        "string", "string-ref", "string-set!", "string-append",
        "vector", "vector-ref", "vector-set!", "vector-append",
        "integer->char", "char->integer",
        NULL
    };
    
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(symbol, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

// prints the env
void print_env(Env *env, const char *name) {
    printf("=== %s ===\n", name);
    Env *current = env;
    int level = 0;
    while (current != NULL) {
        printf("  level %d (type=%d):\n", level, current->type);
        for (size_t i = 0; i < current->count; i++) {
            printf("    %s -> %ld\n", current->val[i].symbol, current->val[i].stack_location);
        }
        current = current->parent;
        level++;
    }
}

// gen unique lambda labels
char* gen_lambdaName() {
    char *str = malloc(64 * sizeof(char));
    sprintf(str, "l%ld", lambdacount);
    lambdacount++;
    return str;
}

char* gen_ccName() {
    char *str = malloc(64 * sizeof(char));
    sprintf(str, "t%ld", ccount);
    ccount++;
    return str;

}

// add to list
void add_to_list(ExprList *list, Expr *item) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, list->capacity * sizeof(Expr*));
        if (list->items == NULL) {
            printf("Error: allocation failure\n");
            exit(-1);
        }
    }
    list->items[list->count++] = item;
}

// Display parsed list
void display_parsed_list(Expr *parsed) {
    if (parsed == NULL) {
        printf("NULL");
        return;
    }

    switch (parsed->type) {
        case EXPR_INT:
            printf("%lld", (long long)parsed->as.int_val);
            break;

        case EXPR_CHAR:
            // Print character in Scheme format
            if (parsed->as.char_val == '\n') {
                printf("\"#\\newline\"");
            } else if (parsed->as.char_val == ' ') {
                printf("\"#\\space\"");
            } else {
                printf("\"#\\%c\"", (char)parsed->as.char_val);
            }
            break;

        case EXPR_BOOL:
            if (parsed->as.bool_val == 0) {
                printf("\"#f\"");
            } else {
                printf("\"#t\"");
            }
            break;

        case EXPR_SYMBOL:
            printf("\"%s\"", parsed->as.symbol);
            break;

        case EXPR_LIST:
            printf("[");
            for (size_t i = 0; i < parsed->as.list.count; i++) {
                if (i > 0) {
                    printf(", ");
                }
                display_parsed_list(parsed->as.list.items[i]);
            }
            printf("]");
            break;

        default:
            printf("UNKNOWN");
            break;
    }
}

/*
 * Helper functions to free structs
 */

void free_expr(Expr *parsed) {
    if (parsed == NULL){
        free(parsed);
        return;
    }

    switch (parsed->type) {
        case EXPR_INT:
        case EXPR_CHAR:
        case EXPR_BOOL:
            free(parsed);
            break;
        case EXPR_SYMBOL:
            free(parsed->as.symbol);
            free(parsed);
            break;
        case EXPR_LIST:
            for(size_t i = 0; i < parsed->as.list.count; i++) {
                free_expr(parsed->as.list.items[i]);

            }
            free(parsed->as.list.items);
            free(parsed);
            break;
        default:
            free(parsed);
            break;
    }
}

void free_env(Env *env) {
    if (env == NULL) return;
    for (size_t i = 0; i < env->count; i++) {
        free(env->val[i].symbol); 
    }

    free(env->val);
    free(env);
}


#endif

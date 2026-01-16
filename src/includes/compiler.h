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
    union {
        int64_t int_val;        // EXPR_INT
        int64_t char_val;       // EXPR_CHAR
        int64_t bool_val;       // EXPR_BOOL
        int64_t mt_list;
        char symbol[64];        // EXPR_SYMBOL
        ExprList list;          // EXPR_LIST
    } as;
};

Int64_Array code_array;

/*
 *  Function Declaration
 */
void Compiler(Expr *parsed);
void compile_list(Expr *list);
void compile_add1(Expr *list);
void compile_sub1(Expr *list);
void compile_int2char(Expr *list);
void compile_char2int(Expr *list);
void compile_nullp(Expr *list);
void compile_zerop(Expr *list);
void compile_not(Expr *list);
void compile_intp(Expr *list);
void compile_boolp(Expr *list);
void add_to_list(ExprList *list, Expr *item);
void display_parsed_list(Expr *parsed);




void compile_add1(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: add1 expects 1 argument\n");
        return;
    }
    puts("in the add1 comiler");
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, AEG1);

}

/*
 * HELPER functions for the Expr array
 */
void add_to_list(ExprList *list, Expr *item) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, list->capacity * sizeof(Expr*));
        if (list->items == NULL) {
            printf("Error: allocation failure\n");
            exit(1);
        }
    }

    list->items[list->count++] = item;

}

/*
 * HELPER function to display the parsed expr
 * in the list
 */
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
                printf("#\\newline");
            } else if (parsed->as.char_val == ' ') {
                printf("#\\space");
            } else {
                printf("#\\%c", (char)parsed->as.char_val);
            }
            break;

        case EXPR_BOOL:
            if (parsed->as.bool_val == 0) {
                printf("#t");
            } else {
                printf("#f");
            }
            break;

        case EXPR_SYMBOL:
            printf("%s", parsed->as.symbol);
            break;

        case EXPR_LIST:
            printf("(");
            for (size_t i = 0; i < parsed->as.list.count; i++) {
                if (i > 0) {
                    printf(" ");  // Space between elements
                }
                // RECURSIVE CALL for each list item
                display_parsed_list(parsed->as.list.items[i]);
            }
            printf(")");
            break;

        default:
            printf("UNKNOWN");
            break;
    }
}


#endif

#ifndef HELPER_H
#define HELPER_H


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


typedef struct {
    int64_t *code;
    size_t size;
    size_t capacity;
} Int64_Array;


#define INT_TAG     0b00
#define CHAR_TAG    0b00001111
#define BOOL_TAG    0b0011111
#define MT_MASK     0b00101111


/*
 * Function Declaration
 */
int64_t tagInt(int64_t integer);
int64_t tagChar(int64_t chars);
int64_t tagBool(int64_t bools);
int64_t untagInt(int64_t integer);
bool is_symbol_char(char c);
bool is_symbol_start(char c);







/*
 * HELPER functions
 */
bool is_symbol_start(char c) {
    // First character of symbol (cannot start with digit)
    return isalpha(c) ||
           c == '+' || c == '-' || c == '*' ||
           c == '<' || c == '>' || c == '=';
}

bool is_symbol_char(char c) {
    // Any character within a symbol (digits allowed)
    return is_symbol_start(c) || isdigit(c);
}


/*
 * Function to Tag and Untag the types
 */
int64_t tagInt(int64_t integer) {
    return (integer << 2) | INT_TAG;
}

int64_t tagChar(int64_t chars){
    return (chars << 8) | CHAR_TAG;
}

int64_t tagBool(int64_t bools) {
    return (bools << 7) | BOOL_TAG;
}

int64_t untagInt(int64_t integer) {
    return (integer >> 2) | INT_TAG;
}


/*
 * HELPER functions for initializing and managing the data array
 */
Int64_Array initializeInt64_arr() {
    Int64_Array code_array;
    code_array.size = 0;
    code_array.capacity = 1;
    code_array.code = calloc(code_array.capacity, sizeof(int64_t));
    return code_array;
}

void add_element(Int64_Array *code_array, int64_t code) {

    if (code_array->size >= code_array->capacity) {
        code_array->capacity = code_array->capacity * 2;

        code_array->code = realloc(code_array->code, code_array->capacity * sizeof(int64_t
));

        if (code_array->code == NULL) {
            printf("Error: allocation failure\n");
            exit(1);
        }
    }
    code_array->code[code_array->size++] = code;

}


#endif

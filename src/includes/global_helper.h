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
#define PAIR_TAG    0b001

/*
 * Function Declaration
 */
int64_t tagInt(int64_t integer);
int64_t tagChar(int64_t chars);
int64_t tagBool(int64_t bools);
int64_t tagPair(int64_t ptr);
int64_t untagInt(int64_t integer);
int64_t untagBool(int64_t bools);
int64_t untagChar(int64_t chars);
bool isInt(int64_t integer);
bool isChar(int64_t chars);
bool isBool(int64_t bools);
bool isPair(int64_t ptr);
bool isValidType(int64_t val);
bool is_symbol_char(char c);
bool is_symbol_start(char c);
void add_at_index(Int64_Array *code_array, int64_t code, size_t index);







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
 * Functions to Tag types
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

int64_t tagPair(int64_t ptr) {
    return (ptr << 3) | PAIR_TAG;
}



/*
 * Functions to untag type
 */

int64_t untagInt(int64_t integer) {
    return (integer >> 2);
}

int64_t untagChar(int64_t chars) {
    return (chars >> 8);
}

int64_t untagBool(int64_t bools) {
    return (bools >> 7);
}

/*
 * Functions to check if valid type
 */
bool isInt(int64_t integer) {
    return (integer & 0b11) == INT_TAG; 
}

bool isChar(int64_t chars) {
    return (chars & 0b11111111) == CHAR_TAG;
}

bool isBool(int64_t bools) {
    return (bools & 0b1111111) == BOOL_TAG;
}

bool isValidType(int64_t val) {
    return isInt(val) || isChar(val) || isBool(val);
}

bool isPair(int64_t ptr) {
    return (ptr & 0b111) == PAIR_TAG;
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
            exit(-1);
        }
    }
    code_array->code[code_array->size++] = code;

}


void add_at_index(Int64_Array *code_array, int64_t code, size_t index) {
    if (index > code_array->size) {
        printf("Error: invalid index\n");
        exit(-1);
    }

    /*
    if (code_array->size >= code_array->capacity) {
        code_array->capacity = code_array->capacity * 2;

        code_array->code = realloc(code_array->code, code_array->capacity * sizeof(int64_t
));

        if (code_array->code == NULL) {
            printf("Error: allocation failure\n");
            exit(1);
        }
    }
    */
    code_array->code[index] = code;

}

#endif

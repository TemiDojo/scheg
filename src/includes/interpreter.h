#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <inttypes.h>
#include <stdio.h>
#include <stddef.h>
#include "global_helper.h"



#define BOX_TOP "╔══════════════════════════════╗"
#define BOX_SIDE "║"
#define BOX_DIVIDER "╠══════════════════════════════╣"
#define BOX_BOTTOM "╚══════════════════════════════╝"



int64_t data;
int64_t instr;
FILE *fp;
size_t codes_read;
//size_t stack_rsp;
Int64_Array stack;



void interpret();
void read_word();
int64_t get_instr();
void push(int64_t val);
void popN(size_t count);
int64_t pop();
void visualize_stack();



void push(int64_t val) {
    add_element(&stack, val);
    //size_t dummy = *stack_rsp;
    //visualize_stack();
}

void popN(size_t count) {
    //visualize_stack();
    stack.size-=count;
}

int64_t pop() {
    printf("...%d:%d...", stack.size,stack.size-1);
    int64_t retval = stack.code[stack.size-1];
    stack.size--;
    return retval;
}

int64_t get(int64_t index) {
    return stack.code[index];
}


void visualize_stack() {
    puts(BOX_TOP);
    for(size_t i = 0; i < stack.size; i++) {
        printf(BOX_SIDE "      0x%016" PRIx64 "      " BOX_SIDE "\n", stack.code[i]);
    }

    printf(BOX_BOTTOM " <- rsp\n");

}

#endif

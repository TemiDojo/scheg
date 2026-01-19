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
void push(int64_t data, size_t *stack_rsp);
void pop(size_t i, size_t *stack_rsp);
void visualize_stack();



void push(int64_t data, size_t *stack_rsp) {
    add_element(&stack, data);
    //size_t dummy = *stack_rsp;
    //visualize_stack();
    ++*stack_rsp;
}

void pop(size_t i, size_t *stack_rsp) {
    //visualize_stack();
    *stack_rsp-=i;
    stack.size-=i;
}

int64_t get(size_t index) {
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

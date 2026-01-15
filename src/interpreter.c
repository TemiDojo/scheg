#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "compiler.h"
#include "opcodes.h"
#include "types.h"
#include "interpreter.h"
#include <inttypes.h>


// global ptr

int main(void) {

    fp = fopen("test.scm", "r");
    if (fp == NULL) {
        printf("Error opening file for read\n");
        return -1;
    }   
    // Now create a virtual stack to push data
    stack = initializeInt64_arr();

    codes_read = fread(&code, sizeof(int64_t), 1, fp);
    printf("%d\n", codes_read);
    interpret();
    visualize_stack();
    fclose(fp);
}

void interpret() {
    puts("in the interpreter");

    while (true) {
        switch (code) {
            case LEG:
                puts("loading 64");
                read_word();
                push(code);
                break;
            case ZEG:
            default:
                break;
        }
        puts("in here");
        if (ftell(fp) == SEEK_END) {
            puts("yayy");
            break;
        }
    } 
}

void read_word() {
    codes_read = fread(&code, sizeof(int64_t), 1, fp);
    printf("%d\n", codes_read);
    printf("%ld", ftell(fp));
    if (ftell(fp) == SEEK_END) {
        puts("Yayyy");
    }
}

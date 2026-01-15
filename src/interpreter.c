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

    codes_read = fread(&instr, sizeof(int64_t), 1, fp);
    interpret();
    visualize_stack();
    fclose(fp);
}

void interpret() {

    bool stop = false;
    while (true) {
        switch (instr) {
            case LEG:
                read_word();
                push(data);
                break;
            case ZEG:
                stop = true;
            default:
                break;
        }
        if (stop) break;
    } 
}

void read_word() {
    codes_read = fread(&data, sizeof(int64_t), 1, fp);
    if (getc(fp) == EOF) {
        instr = (int64_t) ZEG;
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
}

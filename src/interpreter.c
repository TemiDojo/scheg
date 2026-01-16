#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "./includes/compiler.h"
#include "./includes/opcodes.h"
#include "./includes/types.h"
#include "./includes/interpreter.h"
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

    //codes_read = fread(&instr, sizeof(int64_t), 1, fp);
    interpret();
    visualize_stack();
    fclose(fp);
}

void interpret() {

    bool stop = false;
    while (true) {
        switch (get_instr()) {
            case LEG:
                puts("in leg");
                read_word();
                push(data);
                //read_word();
                break;
            case AEG1:
                puts("in add1");
                
                //exit(1);
                break;
            case ZEG:
                puts("in zeg");
                stop = true;
            default:
                puts("default");
                exit(1);
                break;
        }
        if (stop) break;
    } 
}

void read_word() {
    codes_read += fread(&data, sizeof(int64_t), 1, fp);
}

int64_t get_instr() {

    if (getc(fp) == EOF) {
        instr = (int64_t) ZEG;
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
    codes_read += fread(&instr, sizeof(int64_t), 1, fp);
    return instr;
}

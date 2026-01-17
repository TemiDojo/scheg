#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "./includes/compiler.h"
#include "./includes/opcodes.h"
#include "./includes/global_helper.h"
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
            // Load Instr
            case KEG:
                puts("in keg");
                read_word();
                push(data);
                break;
            case KLEG:
                puts("in kleg");
                read_word();
                printf("stack pos: %ld\n", data);
                break;
            case DEG:
                puts("in deg");
                read_word();
                printf("stack index to pop: %ld\n", data);
                break;
            // Unary Primitives
            case AEG1:
                puts("in add1");
                break;
            case SEG1:
                puts("in sub1");
                break;
            case CEG:
                puts("in ceg");
                break;
            case IEG:
                puts("in ieg");
                break;
            case NEG:
                puts("in neg");
                break;
            case sIEG:
                puts("in sieg");
                break;
            case sBEG:
                puts("in sbeg");
                break;
            case ZEG:
                puts("in zeg");
                break;
            // Binary instr
            case SEG:
                puts("in -");
                break;
            case AEG:
                puts("in +");
                break;
            case MEG:
                puts("in *");
                break;
            case LEG:
                puts("in <");
                break;
            case EEG:
                puts("in =");
                break;
            case RET:
                puts("in ret");
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
        instr = (int64_t) RET;
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
    codes_read += fread(&instr, sizeof(int64_t), 1, fp);
    return instr;
}

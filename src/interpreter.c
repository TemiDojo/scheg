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
    size_t stack_rsp = 0;
    int64_t ret_index = 0;
    int64_t env_diff = 0;
    int64_t res;
    int64_t arg1;
    int64_t arg2;
    bool stop = false;
    while (true) {
        switch (get_instr()) {
            // Load Instr
            case KEG:
                puts("in keg");
                read_word();
                push(data, &stack_rsp);
                //if (stack.size == 
                break;
            case KLEG:
                puts("in kleg");
                read_word(); // read the index
                int64_t env_pos = data;
                // get the value
                int64_t env_val = get(env_pos - env_diff);
                printf("env_val %ld\n", untagInt(env_val));
                // push onto the stack
                push(env_val, &stack_rsp); 
                printf("stack pos: %ld\n", data);
                break;
                /*
            case LLEG:
                puts("in lleg");
                read_word();
                printf("last env: %ld\n", data);
                break;
                */
            case DEG:
                puts("in deg");
                read_word();
                printf("stack index to pop: %ld\n", data);
                break;
            // Unary Primitives
            case AEG1:
                puts("in add1");
                // TODO: check if of type int/valid type
                // get the arg
                arg1 = untagInt(get(stack_rsp-1));
                pop(1, &stack_rsp);
                res = arg1 + 1;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
                break;
            case SEG1:
                puts("in sub1");
                // TODO: check if of type int/valid type
                arg1 = untagInt(get(stack_rsp-1));
                pop(1, &stack_rsp);
                res = arg1 - 1;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
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
                // TODO: check if of type int/valid type
                // get the two args
                arg1 = untagInt(get(stack_rsp-1));
                arg2 = untagInt(get(stack_rsp-2));
                pop(2, &stack_rsp);
                res = arg1 - arg2;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
                break;
            case AEG:
                puts("in +");
                // TODO: check if of type int/valid type
                // get the two args 
                arg1 = untagInt(get(stack_rsp-1)); // arg1
                arg2 = untagInt(get(stack_rsp-2));
                printf("stack_rsp: %ld, rsp: %ld\n", stack_rsp, stack.size);
                printf("arg1: %ld, arg2: %ld\n", arg1, arg2);
                pop(2, &stack_rsp);
                res = arg1 + arg2;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
                break;
            case MEG:
                puts("in *");
                // TODO: check if its of type int/valid type
                arg1 = untagInt(get(stack_rsp-1));
                arg2 = untagInt(get(stack_rsp-2));
                pop(2, &stack_rsp);
                res = arg1 * arg2;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
                break;
            case LEG:
                puts("in <");
                break;
            case EEG:
                puts("in =");
                break;
            case RET:
                puts("in ret");
                printf("RETURN: %ld\n", untagInt(get(ret_index)));
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

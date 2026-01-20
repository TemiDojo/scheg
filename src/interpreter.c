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

    fp = fopen("put.out", "r");
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
                read_word();
                push(data, &stack_rsp);
                break;
            case KLEG:
                read_word(); // read the index
                int64_t env_pos = data;
                // get the value
                int64_t env_val = get(env_pos - env_diff);
                // push onto the stack
                push(env_val, &stack_rsp); 
                break;
                /*
            case LLEG:
                puts("in lleg");
                read_word();
                printf("last env: %ld\n", data);
                break;
                */
            case DEG:
                // clear the stack
                env_diff = 0;
                pop(stack_rsp, &stack_rsp);

                break;
            // Unary Primitives
            case AEG1:  // add1
                // TODO: check if of type int/valid type
                // get the arg
                arg1 = untagInt(get(stack_rsp-1));
                pop(1, &stack_rsp);
                res = arg1 + 1;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                //env_diff++;
                break;
            case SEG1:  // sub1
                // TODO: check if of type int/valid type
                arg1 = untagInt(get(stack_rsp-1));
                pop(1, &stack_rsp);
                res = arg1 - 1;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                //env_diff++;
                break;
            case CEG:   // char2int
                arg1 = get(stack_rsp-1);
                pop(1, &stack_rsp);
                res = (arg1 >> 6) | INT_TAG;
                push(res, &stack_rsp);
                ret_index = stack_rsp - 1;
                break;
            case IEG:   // int2char 
                arg1 = get(stack_rsp-1);
                pop(1, &stack_rsp);
                res = (arg1 << 6) | CHAR_TAG;
                push(res, &stack_rsp);
                ret_index = stack_rsp - 1;
                break;
            case NEG:   // neg
                arg1 = untagInt(get(stack_rsp-1));
                res = -arg1;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                break;
            case sIEG:  // integer?
                arg1 = get(stack_rsp-1);
                pop(1, &stack_rsp);
                if(isInt(arg1)) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res, &stack_rsp);
                ret_index = stack_rsp - 1;
                //env_diff++;
                break;
            case sBEG:  // boolean?
                arg1 = get(stack_rsp-1);
                pop(1, &stack_rsp);
                if (isBool(arg1)) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res, &stack_rsp);
                ret_index = stack_rsp - 1;
                //env_diff++;
                break;
            case ZEG:   // zero?
                arg1 = untagInt(get(stack_rsp-1));
                pop(1, &stack_rsp);
                if (arg1 == 0) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res, &stack_rsp);
                ret_index= stack_rsp - 1;
                break;
            // Binary instr
            case SEG:
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
                // TODO: check if of type int/valid type
                // get the two args 
                arg1 = untagInt(get(stack_rsp-1)); // arg1
                arg2 = untagInt(get(stack_rsp-2));
                pop(2, &stack_rsp);
                res = arg1 + arg2;
                push(tagInt(res), &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
                break;
            case MEG:
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
                // real number required
                arg1 = untagInt(get(stack_rsp-1));
                arg2 = untagInt(get(stack_rsp-2));
                pop(2, &stack_rsp);
                if (arg1 < arg2) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res, &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
                break;
            case EEG:
                // real number required 
                arg1 = untagInt(get(stack_rsp-1));
                arg2 = untagInt(get(stack_rsp-2));
                pop(2, &stack_rsp);
                if (arg1 == arg2) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res, &stack_rsp);
                ret_index = stack_rsp - 1;
                env_diff++;
                break;
            case RET:
                // TODO: check the type before return
                printf("RETURN: %c\n", untagChar(get(ret_index)));
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

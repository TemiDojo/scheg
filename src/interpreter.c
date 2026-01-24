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
        return -2;
    }   
    // Now create a virtual stack to push data
    stack = initializeInt64_arr();

    puts("********** BEGIN INTERPRETING ***********");
    interpret();
    puts("*********** INTERPRETER DONE ************");
    //visualize_stack();
    fclose(fp);
}

void interpret() {
    size_t jump_loc = 0;
    int64_t stack_rsp = -1;
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
                puts("KEG");
                read_word();
                push(data);
                stack_rsp++;
                break;
            case KLEG:
                puts("KLEG");
                read_word(); // read the index
                int64_t env_pos = data;
                // get the value
                int64_t env_val;
                if (env_pos <= 0) {
                    env_val = get(env_diff);
                } else {
                    env_val = get(env_pos - env_diff);
                }
                // push onto the stack
                push(env_val); 
                stack_rsp++;
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
                pop(stack_rsp);
                stack_rsp-=stack_rsp;
                break;
            case cJEG:
                puts("cJEG");
                arg1 = get(stack_rsp);
                //arg1 = 0;
                read_word();
                if (isBool(arg1)) {
                    arg1 = untagBool(arg1);
                    if (arg1 == 0) {
                        jump_loc = (data * 8) - (codes_read * 8);
                        fseek(fp, jump_loc, SEEK_CUR);
                    } 
                }
                break;
            case JEG:
                puts("JEG");
                read_word();
                jump_loc = (data * 8) - (codes_read * 8);
                fseek(fp, jump_loc, SEEK_CUR);

                break;
            // Unary Primitives
            case AEG1:  // add1
                // TODO: check if of type int/valid type
                // get the arg
                puts("AEG");
                arg1 = untagInt(get(stack_rsp));
                pop(1);
                stack_rsp--;
                res = arg1 + 1;
                push(tagInt(res));
                stack_rsp++;
                ret_index = stack_rsp;
                //env_diff++;
                break;
            case SEG1:  // sub1
                // TODO: check if of type int/valid type
                puts("SEG1");
                arg1 = untagInt(get(stack_rsp));
                pop(1);
                stack_rsp--;
                res = arg1 - 1;
                push(tagInt(res));
                stack_rsp++;
                ret_index = stack_rsp;
                //env_diff++;
                break;
            case CEG:   // char2int
                puts("CEG");
                arg1 = get(stack_rsp);
                pop(1);
                stack_rsp--;
                res = (arg1 >> 6) | INT_TAG;
                push(res);
                stack_rsp++;
                ret_index = stack_rsp;
                break;
            case IEG:   // int2char 
                puts("IEG");
                arg1 = get(stack_rsp);
                pop(1);
                stack_rsp--;
                res = (arg1 << 6) | CHAR_TAG;
                push(res);
                stack_rsp++;
                ret_index = stack_rsp;
                break;
            case NEG:   // neg
                puts("NEG");
                arg1 = untagInt(get(stack_rsp));
                res = -arg1;
                push(tagInt(res));
                stack_rsp++;
                ret_index = stack_rsp;
                break;
            case sIEG:  // integer?
                puts("sIEG");
                arg1 = get(stack_rsp-1);
                pop(1);
                stack_rsp--;
                if(isInt(arg1)) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                stack_rsp++;
                ret_index = stack_rsp;
                //env_diff++;
                break;
            case sBEG:  // boolean?
                puts("sBEG");
                arg1 = get(stack_rsp-1);
                pop(1);
                stack_rsp--;
                if (isBool(arg1)) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                stack_rsp++;
                ret_index = stack_rsp;
                //env_diff++;
                break;
            case ZEG:   // zero?
                puts("ZEG");
                arg1 = untagInt(get(stack_rsp));
                pop(1);
                stack_rsp--;
                if (arg1 == 0) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                stack_rsp++;
                ret_index= stack_rsp;
                break;
            // Binary instr
            case SEG:
                // TODO: check if of type int/valid type
                // get the two args
                puts("SEG");
                arg1 = untagInt(get(stack_rsp));
                arg2 = untagInt(get(stack_rsp-1));
                pop(2);
                stack_rsp-=2;
                res = arg1 - arg2;
                push(tagInt(res));
                stack_rsp++;
                ret_index = stack_rsp;
                env_diff++;
                break;
            case AEG:
                puts("ADD");
                // TODO: check if of type int/valid type
                // get the two args 
                arg1 = untagInt(get(stack_rsp)); // arg1
                arg2 = untagInt(get(stack_rsp-1));
                pop(2);
                stack_rsp-=2;
                res = arg1 + arg2;
                push(tagInt(res));
                stack_rsp++;
                ret_index = stack_rsp;
                env_diff++;
                break;
            case MEG:
                // TODO: check if its of type int/valid type
                puts("MEG");
                arg1 = untagInt(get(stack_rsp));
                arg2 = untagInt(get(stack_rsp-1));
                pop(2);
                stack_rsp-=2;
                res = arg1 * arg2;
                push(tagInt(res));
                stack_rsp++;
                ret_index = stack_rsp;
                env_diff++;
                break;
            case LEG:
                // real number required
                puts("LEG");
                arg1 = untagInt(get(stack_rsp));
                arg2 = untagInt(get(stack_rsp-1));
                pop(2);
                stack_rsp-=2;
                if (arg1 < arg2) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                stack_rsp++;
                ret_index = stack_rsp;
                env_diff++;
                break;
            case EEG:
                puts("EEG");
                // real number required 
                arg1 = untagInt(get(stack_rsp));
                arg2 = untagInt(get(stack_rsp-1));
                pop(2);
                stack_rsp-=2;
                if (arg1 == arg2) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                stack_rsp++;
                ret_index = stack_rsp;
                env_diff++;
                break;
            case RET:
                // TODO: check the type before return
                printf("RETURN: %c\n", untagChar(get(ret_index)));
                printf("RETURN: %ld\n", untagInt(get(ret_index)));
                stop = true;
            default:
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

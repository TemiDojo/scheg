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
static int64_t heap[0x1000] = {};

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Error: invalid arg ./interpret <file_pathname>\n");
        return -1;
    }
    char *file_path = argv[1];
    fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("Error opening file for read\n");
        return -1;
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
    int64_t ret_index = 0;
    int64_t env_diff = 0;
    int64_t res;
    int64_t arg1;
    int64_t arg2;
    bool stop = false;
    size_t heap_loc = 0;

    while (true) {
        switch (get_instr()) {
            // Load Instr
            case KEG:
                puts("KEG");
                read_word();
                push(data);
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
                break;
            case FLEG:
                puts("FLEG");
                read_word();
                // pop the res
                int64_t dummy_ret = pop();
                // clear the env values
                popN(data);
                push(dummy_ret);
                ret_index = stack.size - 1;
                break;
            case SIKeEG:
                puts("SIKeEG");
                pop(); // pop the res
                break;
            case DEG:
                // clear the stack
                printf("RETURN: %c\n", untagChar(get(ret_index)));
                printf("RETURN: %ld\n", untagInt(get(ret_index)));
                if (isPair(get(ret_index))) {
                    int64_t *ptr = (int64_t *)get(ret_index >> 3); 
                    printf("RETURN: (%ld . %ld)\n", untagInt(ptr[0]), untagInt(ptr[1]));
                }

                env_diff = 0;
                popN(stack.size - 1);
                break;
            case cJEG:
                puts("cJEG");
                arg1 = pop();
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
                arg1 = untagInt(pop());
                res = arg1 + 1;
                push(tagInt(res));
                ret_index = stack.size - 1;
                //env_diff++;
                break;
            case SEG1:  // sub1
                // TODO: check if of type int/valid type
                puts("SEG1");
                arg1 = untagInt(pop());
                res = arg1 - 1;
                push(tagInt(res));
                ret_index = stack.size - 1;
                //env_diff++;
                break;
            case CEG:   // char2int
                puts("CEG");
                arg1 = pop();
                res = (arg1 >> 6) | INT_TAG;
                push(res);
                ret_index = stack.size - 1;
                break;
            case IEG:   // int2char 
                puts("IEG");
                arg1 = pop();
                res = (arg1 << 6) | CHAR_TAG;
                push(res);
                ret_index = stack.size - 1;
                break;
            case NEG:   // neg
                puts("NEG");
                arg1 = untagInt(pop());
                res = -arg1;
                push(tagInt(res));
                ret_index = stack.size - 1;
                break;
            case sIEG:  // integer?
                puts("sIEG");
                arg1 = pop();
                if(isInt(arg1)) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                ret_index = stack.size - 1;
                //env_diff++;
                break;
            case sBEG:  // boolean?
                puts("sBEG");
                arg1 = pop();
                if (isBool(arg1)) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                ret_index = stack.size - 1;
                //env_diff++;
                break;
            case ZEG:   // zero?
                puts("ZEG");
                arg1 = untagInt(pop());
                if (arg1 == 0) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                ret_index= stack.size - 1;
                break;
            // Binary instr
            case SEG:
                // TODO: check if of type int/valid type
                // get the two args
                puts("SEG");
                arg1 = untagInt(pop());
                arg2 = untagInt(pop());
                res = arg1 - arg2;
                push(tagInt(res));
                ret_index = stack.size - 1;
                env_diff++;
                break;
            case AEG:
                puts("ADD");
                // TODO: check if of type int/valid type
                // get the two args 
                arg1 = untagInt(pop()); // arg1
                arg2 = untagInt(pop());
                res = arg1 + arg2;
                push(tagInt(res));
                ret_index = stack.size - 1;
                env_diff++;
                break;
            case MEG:
                // TODO: check if its of type int/valid type
                puts("MEG");
                arg1 = untagInt(pop());
                arg2 = untagInt(pop());
                res = arg1 * arg2;
                push(tagInt(res));
                ret_index = stack.size - 1;
                env_diff++;
                break;
            case LEG:
                // real number required
                puts("LEG");
                arg1 = untagInt(pop());
                arg2 = untagInt(pop());
                if (arg1 < arg2) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                ret_index = stack.size - 1;
                env_diff++;
                break;
            case EEG:
                puts("EEG");
                // real number required 
                arg1 = untagInt(pop());
                arg2 = untagInt(pop());
                if (arg1 == arg2) {
                    res = (tagBool(1));
                } else {
                    res = (tagBool(0));
                }
                push(res);
                ret_index = stack.size - 1;
                env_diff++;
                break;
            case CONSEG:
                
                arg1 = pop();
                arg2 = pop();
                // after we pop we put on the heap
                int64_t* con_ptr = heap + heap_loc;
                con_ptr[++heap_loc] = arg1;
                con_ptr[++heap_loc] = arg2;

                // tag the ptr and push on stack
                push(tagPair((int64_t)con_ptr));
    

                break;
            case RET:
                // TODO: check the type before return
                //printf("RETURN: %c\n", untagChar(get(ret_index)));
                //printf("RETURN: %ld\n", untagInt(get(ret_index)));


                // after
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

// void peek_word() {
//
// }

int64_t get_instr() {

    if (getc(fp) == EOF) {
        instr = (int64_t) RET;
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
    codes_read += fread(&instr, sizeof(int64_t), 1, fp);
    return instr;
}

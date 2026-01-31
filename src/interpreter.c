#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdalign.h>
#include "./includes/compiler.h"
#include "./includes/opcodes.h"
#include "./includes/global_helper.h"
#include "./includes/interpreter.h"
#include <inttypes.h>


// global ptr
static char heap[0x1000] = {};

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
    char* con_ptr = heap;
    uintptr_t tag_val;

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
                printf("RETURN: ");
                int64_t result = get(ret_index);
                print_res(result);
                puts("");

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
                puts("CONSEG"); 

                arg1 = pop();
                arg2 = pop();
                // align the ptr first 
                if (((uintptr_t)con_ptr) % 8 != 0) {
                    con_ptr = (char *)(((uintptr_t)con_ptr + 7) & ~7);
                }

                memcpy(con_ptr, &arg1, sizeof(int64_t));
                con_ptr+=(sizeof(int64_t));
            
                memcpy(con_ptr, &arg2, sizeof(int64_t));
                con_ptr+=(sizeof(int64_t));

                tag_val = tagPair((uintptr_t) (con_ptr));

                if ((tag_val & 0x7) == 0b001) {
                    puts("TRUE");
                }
                push(tag_val);
                ret_index = stack.size -1;
                env_diff++;
                break;
            case CAREG:
                puts("CAREG");
                uintptr_t car_arg1 = pop();
                if (!isPair(car_arg1)) {
                }
                char * car_ptr = (char *)untagPair(car_arg1);
                int64_t car_val;  
                memcpy(&car_val, car_ptr - (1 * sizeof(int64_t)), sizeof(int64_t));
                push(car_val);
                ret_index = stack.size - 1;
                
                break;
            case CDREG:
                puts("CDREG");
                uintptr_t cdr_arg1 = pop();
                if (!isPair(cdr_arg1)) {
                }
                char *cdr_ptr = (char *)untagPair(cdr_arg1);
                int64_t cdr_val;
                memcpy(&cdr_val, cdr_ptr - (2 * sizeof(int64_t)), sizeof(int64_t));
                push(cdr_val);
                ret_index = stack.size - 1;

                break;
            case STREG:
                puts("STREG");
                read_word(); // read the size of the string

                for(size_t i = 0; i < data; i++) {
                    int64_t dumChar = pop();
                    if (!isChar(dumChar)) {
                        printf("Error: character required\n");
                        exit(-2);
                    }
                    *con_ptr++ = untagChar(dumChar);
                }

                // align the ptr first 
                if (((uintptr_t)con_ptr) % 8 != 0) {
                    con_ptr = (char *)(((uintptr_t)con_ptr + 7) & ~7);
                }
                memcpy(con_ptr, &data, sizeof(int64_t));
                con_ptr+=(sizeof(int64_t));

                tag_val = tagStr((uintptr_t) (con_ptr));

                push(tag_val);
                ret_index = stack.size - 1;
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

void unroll_cons(char *ptr) {
    uintptr_t arg1;
    uintptr_t arg2;

    memcpy(&arg1, ptr - (1 * sizeof(int64_t)), sizeof(int64_t));
    memcpy(&arg2, ptr- (2 * sizeof(int64_t)), sizeof(int64_t));

    if (!isPair(arg1) && !isPair(arg2)) {
        printf("(");
        print_res((int64_t) arg1);
        printf(" . ");
        print_res((int64_t) arg2);
        printf(")");
        return;
    }

    
    if (!isPair(arg1) && isPair(arg2)) {
        printf("(");
        print_res((int64_t)arg1);
        printf(" ");
        char *ptrl = (char *)(untagPair(arg2));
        unroll_cons(ptrl);

    } else if (isPair(arg1) && !isPair(arg2)) {
        char *ptrl = (char *)(untagPair(arg1));
        unroll_cons(ptrl);
        print_res((int64_t)arg2);
        printf(")");

    } else if (isPair(arg1) && isPair(arg2)) {
        printf("(");
        char *ptrl = (char *)untagPair(arg1);
        char *ptrr = (char *)untagPair(arg2);
        unroll_cons(ptrl);
        unroll_cons(ptrr);
    }

    printf(")");
}

void print_res(int64_t res){

    if (isInt(res)) {
        printf("%ld", untagInt(res));
    } else if(isChar(res)) {
        printf("%c", untagChar(res));
    } else if (isBool(res)) {
        printf("%ld", untagBool(res));
    } else if (isMtList(res)) {
        printf("()");
    } else if (isPair((uintptr_t) res)) {
        uintptr_t val = untagPair((uintptr_t) res);
        char *ptr = ((char *) val);
        unroll_cons(ptr);
    } else if (isStr((uintptr_t) res)) {
        int64_t str_size;
        uintptr_t val = untagStr((uintptr_t) res);
        char *ptr = ((char *) val);
        ptr = ptr - (1 * sizeof(int64_t));
        memcpy(&str_size, ptr, sizeof(int64_t));
        ptr = ptr - ((str_size / sizeof(int64_t) + 1) * sizeof(int64_t));
        char str_res[str_size+1] = {};

        for (int i = 0; i < str_size; i++) {
            str_res[str_size-(i+1)] = ptr[i];
        }
        str_res[str_size] = '\0';
        printf("%s", str_res);
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

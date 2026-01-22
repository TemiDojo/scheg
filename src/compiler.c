#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "./includes/parser.h"
#include "./includes/compiler.h"
#include "./includes/opcodes.h"
#include "./includes/global_helper.h"




int main(int argc, char **argv) {


    FILE *fptr = fopen("put.out", "w");
    if (fptr == NULL) {
        printf("File could not be created\n");
        return -1;
    } 

    if (argc == 2) {
        // then we check if the arg passed is a file
        char *file_path = argv[1];
        printf("file name: %s\n", file_path);
        int fd = open(file_path, O_RDONLY);
        if (fd < 0) {
            printf("Failed to open file\n");
            return -1;
        }
        // get file size with fstat
        struct stat bufstat;
        int sd = fstat(fd, &bufstat);
        if (sd < 0) {
            printf("Failed to fstat file\n");
            return -1;
        }
        int filesize = bufstat.st_size;

        // load the file into memory with mmap
        char *expr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
        if (expr == MAP_FAILED) {
            printf("Failed to mmap file\n");
            return -1;
        }





        size_t expr_len = strlen(expr);

        Parser p = new_parser(expr);
        while(p.pos < (expr_len-1)) {
            global_stackPos = -1;
            code_array = initializeInt64_arr();

            // printf("expression to parse: %s\n", p.source + p.pos);
            Expr *parsed = scheme_parse(&p);
            if (parsed != NULL) {

                display_parsed_list(parsed);

                Env env = initializeEnv();
                Compiler(parsed, &env);

                // emit an instruction after each compiled expression
                // so we interpreter resets the stack
                add_element(&code_array, DEG);

                fwrite(code_array.code, sizeof(int64_t), code_array.size, fptr);
                free(code_array.code);
                free_env(&env);
                printf("updated position is: %ld\n", p.pos);
                free_expr(parsed);
            }
        }


    } else {
    // recieve expr through commmand line

        char *expr = NULL;
        size_t len = 0;
        getline(&expr, &len, stdin);
        
        Parser p = new_parser(expr);
        while(p.pos < (strlen(expr)-1)) {
            global_stackPos = -1;
            code_array = initializeInt64_arr();

            // printf("expression to parse: %s\n", p.source);
            Expr *parsed = scheme_parse(&p);

            //display_parsed_list(parsed);
            if (parsed != NULL) {

                Env env = initializeEnv();
                Compiler(parsed, &env);

                // emit an instruction after each compiled expression
                // so we interpreter resets the stack
                add_element(&code_array, DEG);

                fwrite(code_array.code, sizeof(int64_t), code_array.size, fptr);
                free(code_array.code);
                free_env(&env);
                printf("updated position is: %ld\n", p.pos);

                
                free_expr(parsed);
            }
            //free(parsed);
        }

        free(expr);
    }

    fclose(fptr);

    return 0;

   

}

Expr* scheme_parse(Parser *p) {
    skip_whitespace(p);
    if (p->pos >= p->length) {
        return NULL;
    }

    char c = peek(p);
    char cN = advanceN(p);


    if (c == '(') {
        // just whitespace, nothing else
        return parse_expr(p);
    } else if (isdigit(c)) {    // integer
        return parse_number(p);
    } else if (c == '-'){
        if (isdigit(cN)) {
            return parse_number(p);
        } else {
            return parse_symbol(p);
        }
    } else if ((c == '#') && (cN == '\\')){     // character
        return parse_char(p);
    } else if (c == '#' && cN == '|'){
        skip_multiline_comments(p);
        return scheme_parse(p);
    } else if (c == '#'){       // Boolean
        return parse_bool(p);
    } else if (is_symbol_start(c)){
        return parse_symbol(p);
    } else if (c == ';') {
        // comments
        skip_comments(p);
        return scheme_parse(p);
    }  else {
        printf("Error: wrong expression '%c' \n", c);
        exit(1);
    }

}


void Compiler(Expr *parsed, Env *env) {

    if (parsed == NULL) {
        printf("NULL");
        return;
    }

    int64_t tag_num;
    switch (parsed->type) {
        case EXPR_INT: // integer
            add_element(&code_array, KEG);
            puts("in the int type");
            // tag the value
            tag_num = tagInt(parsed->as.int_val);
            add_element(&code_array, tag_num);
            stack_pointer++;
            global_stackPos++;
            break;
        case EXPR_CHAR: // characters
            add_element(&code_array, KEG);
            // tag the value
            tag_num = tagChar(parsed->as.char_val);
            add_element(&code_array, tag_num);
            global_stackPos++;
            break;
        case EXPR_BOOL: // booleans
            add_element(&code_array, KEG);
            // tag the value
            tag_num = tagBool(parsed->as.bool_val);
            add_element(&code_array, tag_num);
            global_stackPos++;
            break;
        case EXPR_SYMBOL:
            int64_t stack_pos = lookup(env, parsed->as.symbol);
            if (stack_pos == -1) {
                printf("Error: unbound variable: %s\n", parsed->as.symbol);
                exit(-2);
            } else {

                printf("found variable : %s\n", parsed->as.symbol);
                add_element(&code_array, KLEG);
                add_element(&code_array, stack_pos);
                stack_pointer++;
            }
            //global_stackPos++;
            break;
        case EXPR_LIST:
            compile_list(parsed, env);
            break; 
        default:
            printf("Error: unknown expression type\n");
            exit(-2);
            break;
    }
}

void compile_list(Expr *list, Env *env) {

    // empty list
    if (list->as.list.count == 0) {
        add_element(&code_array, KEG);
        add_element(&code_array, MT_MASK);
        return;
    }

    Expr *op = list->as.list.items[0];
    if (op->type != EXPR_SYMBOL) {
        printf("Error: expected operator to be a symbol\n");
        exit(-3);
        return;
    }

    const char *op_name = op->as.symbol;
    // unary primitives
    if (strcmp(op_name, "add1") == 0) {
        // we load some 
        compile_add1(list, env);
    } else if(strcmp(op_name, "sub1") == 0) {
        compile_sub1(list, env);
    } else if(strcmp(op_name, "integer->char") == 0) {
        compile_int2char(list, env);
    } else if(strcmp(op_name, "char->integer") == 0) {
        compile_char2int(list, env);
    } else if(strcmp(op_name, "null?") == 0) {
        compile_nullp(list, env);
    } else if(strcmp(op_name, "zero?") == 0) {
        compile_zerop(list, env);
    } else if(strcmp(op_name, "not") == 0) {
        compile_not(list, env);
    } else if(strcmp(op_name, "integer?") == 0) {
        compile_intp(list, env);
    } else if(strcmp(op_name, "boolean?") == 0) {
        compile_boolp(list, env);
    } 
    // Binary primitives
    else if(strcmp(op_name, "+") == 0) {
        compile_add(list, env);
    } else if (strcmp(op_name, "-") == 0) {
        compile_sub(list, env);
    } else if(strcmp(op_name, "*") == 0) {
        compile_mul(list, env);
    } else if(strcmp(op_name, "<") == 0) {
        compile_le(list, env);
    } else if(strcmp(op_name, "=") == 0) {
        compile_eq(list, env);
    // Local variables
    } else if(strcmp(op_name, "let") == 0) {
        Env envnew = initializeEnv();
        envnew.parent = env;
        compile_let(list, &envnew);
        free_env(&envnew);
    // conditionals
    } else if(strcmp(op_name, "if") == 0) {
        compile_if(list, env);
    } else {
        printf("Error: unknown operator '%s'\n", op_name);
        exit(-3);
    }
}

/*
 * Unary Primitives
 */
void compile_add1(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: add1 expects 1 argument\n");
        exit(-4);
        return;
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type in here, only compile int
    // or list expr that will return int
    Compiler(arg, env);

    add_element(&code_array, AEG1);
}

void compile_sub1(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: sub1 expects 1 argument\n");
        exit(-4);
        return;
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type in here, only compile int
    // or list expr that will return int
    Compiler(arg, env);

    add_element(&code_array, SEG1);
}

void compile_int2char(Expr *list, Env *env){
    if (list->as.list.count != 2) {
        printf("Error: int2char expects 1 argument\n");
        exit(-4);
        return;
    }
    Expr *arg = list->as.list.items[1];
    // check the type in here only compile int
    if (arg->type == EXPR_SYMBOL) {
        if (lookup(env, arg->as.symbol) == -1) {
           printf("Error: variable not bound\n"); 
           exit(-4);
        }
    } else if (arg->type != EXPR_INT && arg->type != EXPR_LIST) {
        printf("Error: int2char expects a int\n");
        exit(-4);
    }

    // or list epxr that will return int
    Compiler(arg, env);

    add_element(&code_array, IEG);
}

void compile_char2int(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: char2int expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the type in here, only compile char
    // or list expr that will return char
    Compiler(arg, env);

    add_element(&code_array, CEG);
}

void compile_nullp(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: nullp expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg, env);

    add_element(&code_array, NEG);
}

void compile_zerop(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: zerop expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg, env);

    add_element(&code_array, ZEG);
}

void compile_not(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: not expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type in here, only compile int
    // or list expr that will return int
    // or a symbol that does exist in the env
    Compiler(arg, env);

    add_element(&code_array, NEG);
}

void compile_intp(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: intp expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type, only compile int or
    // list epxr that will return int
    Compiler(arg, env);
    
    add_element(&code_array, sIEG);
}

void compile_boolp(Expr *list, Env *env) {
    if (list->as.list.count != 2) {
        printf("Error: boolp expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg, env);

    add_element(&code_array, sBEG);
}


/*
 * Binary Primitives
 */
void compile_sub(Expr *list, Env *env) {
    if (list->as.list.count != 3) {
        printf("Error: - expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env);

    add_element(&code_array, SEG);
}


void compile_add(Expr *list, Env *env) {
    if (list->as.list.count != 3) {
        printf("Error: + expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env);
    
    /*
    if (env->count > 0) {
        add_element(&code_array, LLEG);
        printf("wtf: %ld\n", env->val[env->count - 1].stack_location);
        add_element(&code_array, env->val[env->count - 1].stack_location);
    }*/
    
    add_element(&code_array, AEG);
}


void compile_mul(Expr *list, Env *env) {
    if (list->as.list.count != 3) {
        printf("Error: * expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env);

    add_element(&code_array, MEG);
}


void compile_le(Expr *list, Env *env) {
    if (list->as.list.count != 3) {
        printf("Error: < expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env);

    add_element(&code_array, LEG);
}


void compile_eq(Expr *list, Env *env) {
    if (list->as.list.count != 3) {
        printf("Error: = expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env);

    add_element(&code_array, EEG);
}


/*
 * Local variables
 */
void compile_let(Expr *list, Env *env) {
    if (list->as.list.count <= 1) {
        printf("Error: syntax-error: malformed let\n");
        exit(-6);
    }
    if (list->as.list.items[1]->as.list.count > 2 || list->as.list.items[1]->as.list.count <= 0) {
        printf("Error: wrong let expression\n");
        exit(-6);
    }
    Expr *arg1 = list->as.list.items[1]; 
    for(size_t i = 0; i < arg1->as.list.count; i++) {
        if (arg1->as.list.items[i]->type != EXPR_LIST) {
            printf("Error: wrong let arg type expression\n");
            exit(-6);
        }
        // evaluate the right most arg
        Expr *arg = arg1->as.list.items[i]->as.list.items[1];
        Compiler(arg, env);
        // let should support every type
        add_binding(env, arg1->as.list.items[i]->as.list.items[0]->as.symbol, global_stackPos);
    }
   // free(env);
   for(size_t i = 2; i < list->as.list.count; i++) {
       Expr *arg2 = list->as.list.items[i];
       Compiler(arg2, env);
   }

}

/*
 * Conditionals
 */
void compile_if(Expr *list, Env *env) {

    if (list->as.list.count != 4) {
        printf("Error: if expects 3 args: test conseq altern\n");
        exit(-7);
    }
    // Expr *list_expr
    // evaluate the test experssion
    Expr *test = list->as.list.items[1];
    Expr *conseq = list->as.list.items[2];
    Expr *altern = list->as.list.items[3];



    // emit test bool val
    Compiler(test, env);
    add_element(&code_array, cJEG); // jump opcode
                                    // 
    size_t idx = code_array.size;
    add_element(&code_array, 0); // jump location to l0


    // emit l1 label consequent
    Compiler(conseq, env);
    // emit jump to end of if
    add_element(&code_array, JEG); // jump opcode
    size_t idx2 = code_array.size;
    add_element(&code_array, 0); // jump location to end of if

    // if false jump to altern
    int64_t l0_label = code_array.size;
    Compiler(altern, env);
    // emit cjump to l0
    add_at_index(&code_array, l0_label, idx);
    // emit jump to end of if
    add_at_index(&code_array, code_array.size, idx2);

}

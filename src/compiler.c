#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "./includes/parser.h"
#include "./includes/compiler.h"
#include "./includes/opcodes.h"
#include "./includes/global_helper.h"


void compile_stream(Parser *p, FILE *fptr) {
    int expr_count = 0;
    while (p->pos < p->length) {
        flow_segment = initializeInt64_arr();

        printf("\n========== EXPRESSION %d ==========\n", ++expr_count);

        Expr *parsed = scheme_parse(p);
        if (parsed == NULL) {
            puts("NULL expression, skipping");
            free_expr(parsed);
            continue;
        }

        puts("── Parsed:");
        display_parsed_list(parsed);
        printf("\n");

        Env *env      = initializeEnv();
        Env *labelEnv = initializeEnv();

        puts("── Compiling...");
        Compiler(parsed, env, labelEnv, flow_segment);
        printf("✓ flow_segment size: %zu words\n", flow_segment->size);
        printf("✓ label_segment size: %zu words\n", label_segment->size);

        add_element(flow_segment, DEG);

        printf("── Writing flow_segment (%zu words) to file\n", flow_segment->size);
        fwrite(flow_segment->code, sizeof(int64_t), flow_segment->size, fptr);

        freeInt64_Array(flow_segment);
        free_env(env);
        for (size_t i = 0; i < labelEnv->count; i++) {
            free_expr(labelEnv->val[i].closure);
        }
        free_env(labelEnv);
        free_expr(parsed);
    }
    printf("\n========== COMPILE STREAM DONE ==========\n");
    printf("total expressions: %d\n", expr_count);
}

int main(int argc, char **argv) {
    puts("\n========== COMPILER STARTING ==========");

    label_segment = initializeInt64_arr();
    add_element(label_segment, LABELEGS);

    char  *source   = NULL;
    size_t src_len  = 0;
    char  *out_path = "put.out";

    if (argc > 1) {
        int    opt;
        char  *file_path = NULL;
        while ((opt = getopt(argc, argv, "c:o:")) != -1) {
            switch (opt) {
                case 'c': file_path = optarg; break;
                case 'o': out_path  = optarg; break;
                default:
                    fprintf(stderr, "Usage: %s [-c <file>] [-o <output>]\n", argv[0]);
                    exit(EXIT_FAILURE);
            }
        }
        if (file_path == NULL) {
            fprintf(stderr, "Error: missing -c <file>\n");
            return -1;
        }
        printf("── Reading from file: %s\n", file_path);

        int fd = open(file_path, O_RDONLY);
        if (fd < 0) { perror("open"); return -1; }

        struct stat st;
        if (fstat(fd, &st) < 0) { perror("fstat"); return -1; }
        src_len = st.st_size;
        printf("── File size: %zu bytes\n", src_len);

        source = mmap(NULL, src_len, PROT_READ, MAP_PRIVATE, fd, 0);
        if (source == MAP_FAILED) { perror("mmap"); return -1; }
        close(fd);
    } else {
        puts("── Reading from stdin");
        ssize_t n = getdelim(&source, &src_len, 0, stdin);
        if (n < 0) { perror("getdelim"); return -1; }
        src_len = n;
        printf("── Read %zu bytes\n", src_len);
    }

    printf("── Output file: %s\n", out_path);
    FILE *fptr = fopen(out_path, "w");
    if (fptr == NULL) { perror("fopen"); return -1; }

    // write placeholder header
    puts("── Writing label_offset placeholder header");
    int64_t label_offset = 0;
    fwrite(&label_offset, sizeof(int64_t), 1, fptr);

    puts("\n========== PARSING & COMPILING ==========");
    Parser p = new_parser(source);
    p.length  = src_len;
    compile_stream(&p, fptr);

    // write label segment
    label_offset = ftell(fptr);
    printf("\n── Writing label_segment at file offset: %ld (%zu words)\n",
           label_offset, label_segment->size);
    fwrite(label_segment->code, sizeof(int64_t), label_segment->size, fptr);

    // patch header with real label offset
    printf("── Patching header with label_offset: %ld\n", label_offset);
    fseek(fptr, 0, SEEK_SET);
    fwrite(&label_offset, sizeof(int64_t), 1, fptr);

    freeInt64_Array(label_segment);
    fclose(fptr);
    printf("── Closed output file: %s\n", out_path);

    if (argc > 1) munmap(source, src_len);
    else          free(source);

    puts("\n========== COMPILER DONE ==========");
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
        advance(p);
        advance(p);
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
    } else if (c == '\"') {
        return parse_string(p);
    } else if (c == '\'' && cN == '#') {
        advance(p);
        advance(p);
        return parse_vector(p);
    }  else {
        printf("Error: wrong expression '%c' \n", c);
        exit(-2);
    }

}


void Compiler(Expr *parsed, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (parsed == NULL) {
        printf("NULL");
        return;
    }

    int64_t tag_num;
    switch (parsed->type) {
        case EXPR_INT: // integer
            add_element(segment, KEG);
            // tag the value
            tag_num = tagInt(parsed->as.int_val);
            add_element(segment, tag_num);
            inc_loc(env);
            break;
        case EXPR_CHAR: // characters
            add_element(segment, KEG);
            // tag the value
            tag_num = tagChar(parsed->as.char_val);
            add_element(segment, tag_num);
            inc_loc(env);
            break;
        case EXPR_BOOL: // booleans
            add_element(segment, KEG);
            // tag the value
            tag_num = tagBool(parsed->as.bool_val);
            add_element(segment, tag_num);
            inc_loc(env);
            break;
        case EXPR_SYMBOL:
            struct Ret r = getEnv(env, parsed->as.symbol);

            if (r.stack_pos == -1) {
                printf("Error: unbound variable: %s\n", parsed->as.symbol);
                exit(-2);
            }
            if (r.type == 0) {
                add_element(segment, KLEG);
                add_element(segment, r.stack_pos);

            } else if (r.type == 1) {
                add_element(segment, HEG);
                add_element(segment, r.stack_pos);
            } else if (r.type == 2) {
                add_element(segment, BLEG);
                add_element(segment, r.stack_pos);
            }
            inc_loc(env);
            break;
        case EXPR_LIST:
            compile_list(parsed, env, labelEnv, segment);
            break; 
        default:
            printf("Error: unknown expression type\n");
            exit(-2);
            break;
    }
}


void compile_list(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    
    // empty list
    if (list->as.list.count == 0) {
        add_element(segment, KEG);
        inc_loc(env);
        add_element(segment, MT_MASK);
        return;
    }


    Expr *op = list->as.list.items[0];
    if (op->type == EXPR_LIST) {

        for(size_t i = 1; i < list->as.list.count; i++) {
            Compiler(list->as.list.items[i], env, labelEnv, segment);
        }

        Compiler(op, env, labelEnv, segment);
        add_element(segment, CLEG);
        add_element(segment, list->as.list.count - 1);

        for(size_t i = 1; i < list->as.list.count; i++) {
            dec_loc(env);
        }
        dec_loc(env);
        inc_loc(env);

        return;

    }
    if (op->type != EXPR_SYMBOL) {
        printf("Error: expected operator to be a symbol '%s'\n", op->as.symbol );
        exit(-3);
    }

    const char *op_name = op->as.symbol;
    // unary primitives
    if (strcmp(op_name, "add1") == 0) {
        // we load some 
        compile_add1(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "sub1") == 0) {
        compile_sub1(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "integer->char") == 0) {
        compile_int2char(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "char->integer") == 0) {
        compile_char2int(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "null?") == 0) {
        compile_nullp(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "zero?") == 0) {
        compile_zerop(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "not") == 0) {
        compile_not(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "integer?") == 0) {
        compile_intp(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "boolean?") == 0) {
        compile_boolp(list, env, labelEnv, segment);
    } 
    // Binary primitives
    else if(strcmp(op_name, "+") == 0) {
        compile_add(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "-") == 0) {
        compile_sub(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "*") == 0) {
        compile_mul(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "<") == 0) {
        compile_le(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "=") == 0) {
        compile_eq(list, env, labelEnv, segment);
    // Local variables
    } else if(strcmp(op_name, "let") == 0) {
        Env *envnew = initializeEnv();
        envnew->parent = env;
        
        compile_let(list, envnew, labelEnv, segment);
        free_env(envnew);

    // conditionals
    } else if(strcmp(op_name, "if") == 0) {
        compile_if(list, env, labelEnv, segment);
    // Pairs - cons, car, cdr
    } else if(strcmp(op_name, "cons") == 0) {
        compile_cons(list, env, labelEnv, segment);
    } else if(strcmp(op_name, "car") == 0) {
        compile_car(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "cdr") == 0) {
        compile_cdr(list, env, labelEnv, segment);
    // String
    } else if (strcmp(op_name, "string") == 0) {
        compile_string(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "string-ref") == 0) {
        compile_stringRef(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "string-set!") == 0) {
        compile_stringSet(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "string-append") == 0) {
        compile_stringAppend(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "vector") == 0) {
        compile_vector(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "vector-ref") == 0) {
        compile_vectorRef(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "vector-set!") == 0) {
        compile_vectorSet(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "vector-append") == 0) {
        compile_vectorAppend(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "begin") == 0) {
        compile_begin(list, env, labelEnv, segment);
    } else if (strcmp(op_name, "lambda") == 0) {
        compile_code(list, env, labelEnv, label_segment);

        char *lambdaId = list->as.list.items[0]->as.symbol;
        int64_t lkId = lookup(labelEnv, lambdaId);
        Expr *closure = labelEnv->val[lkId].closure;

        display_parsed_list(closure);
        compile_closure(closure, env, labelEnv, segment);

    } else if (strcmp(op_name, "funcall") == 0) {
        funcall(list, env, labelEnv, segment);
    } else {
        char *symbol = strdup(op_name);
        int64_t lkId = lookup(labelEnv, symbol);
        if (lkId != -1) {
            Expr *closure = labelEnv->val[lkId].closure;
            compile_closure(closure, env, labelEnv, segment);
        } else if (lookup(env, symbol) != -1){
            compile_var(list,env, labelEnv, segment);
        } else {
            printf("Error: unknown operator '%s'\n", op_name);
            exit(-3);
        }
        free(symbol);
    }
}

/*
 * Unary Primitives
 */
void compile_add1(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: add1 expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type in here, only compile int
    // or list expr that will return int
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, AEG1);
}

void compile_sub1(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: sub1 expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type in here, only compile int
    // or list expr that will return int
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, SEG1);
}

void compile_int2char(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment){
    if (list->as.list.count != 2) {
        printf("Error: int2char expects 1 argument\n");
        exit(-4);
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
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, IEG);
}

void compile_char2int(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: char2int expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the type in here, only compile char
    // or list expr that will return char
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, CEG);
}

void compile_nullp(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: nullp expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, NEG);
}

void compile_zerop(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: zerop expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, ZEG);
}

void compile_not(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: not expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type in here, only compile int
    // or list expr that will return int
    // or a symbol that does exist in the env
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, NEG);
}

void compile_intp(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: intp expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    // check the arg type, only compile int or
    // list epxr that will return int
    Compiler(arg, env, labelEnv, segment);
    
    add_element(segment, sIEG);
}

void compile_boolp(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: boolp expects 1 argument\n");
        exit(-4);
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, sBEG);
}


/*
 * Binary Primitives
 */
void compile_sub(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 3) {
        printf("Error: - expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env, labelEnv, segment);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env, labelEnv, segment);

    add_element(segment, SEG);
    dec_loc(env);
}


void compile_add(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 3) {
        printf("Error: + expects 2 argument\n");
        exit(-5);
    }


    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env, labelEnv, segment);

    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env, labelEnv, segment);
    
    add_element(segment, AEG);
    dec_loc(env);
}


void compile_mul(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 3) {
        printf("Error: * expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env, labelEnv, segment);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env, labelEnv, segment);

    add_element(segment, MEG);
    dec_loc(env);
}


void compile_le(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 3) {
        printf("Error: < expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env, labelEnv, segment);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env, labelEnv, segment);

    add_element(segment, LEG);
    dec_loc(env);
}


void compile_eq(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 3) {
        printf("Error: = expects 2 argument\n");
        exit(-5);
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1, env, labelEnv, segment);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2, env, labelEnv, segment);

    add_element(segment, EEG);
    dec_loc(env);
}


/*
 * Local variables
 */

void compile_let(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count <= 1) {
        printf("Error: syntax-error: malformed let\n");
        exit(-6);
    }
    if (list->as.list.items[1]->as.list.count <= 0) {
        printf("Error: wrong let expression\n");
        exit(-6);
    }

    Expr *arg1 = list->as.list.items[1]; 
    int64_t id = arg1->as.list.count; 
    for(size_t i = 0; i < arg1->as.list.count; i++) {
        Expr *arg = arg1->as.list.items[i]->as.list.items[1];

        Compiler(arg, env->parent, labelEnv, segment);
        
        char *symbol = arg1->as.list.items[i]->as.list.items[0]->as.symbol;
        add_binding(env, symbol, id); 
        id--;
    }

    size_t i;
    for(i = 2; i < list->as.list.count; i++) {
        if (i >= list->as.list.count - 1) {
            Compiler(list->as.list.items[i], env, labelEnv, segment);
            dec_loc(env);
        }
        else {
            Compiler(list->as.list.items[i], env, labelEnv, segment);
            add_element(segment, SIKeEG); 
            dec_loc(env); 
        }
    }
    inc_loc(env);

    add_element(segment, FLEG);
    add_element(segment, arg1->as.list.count);
    
    for(size_t j = 0; j < arg1->as.list.count; j++) {
        dec_loc(env);
    }

}

/*
 * Conditionals
 */
void compile_if(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count != 4) {
        printf("Error: if expects 3 args: test conseq altern\n");
        exit(-6);
    }
    // Expr *list_expr
    // evaluate the test experssion
    Expr *test = list->as.list.items[1];
    Expr *conseq = list->as.list.items[2];
    Expr *altern = list->as.list.items[3];



    // emit test bool val
    Compiler(test, env, labelEnv, segment);
    add_element(segment, cJEG); // jump opcode
    dec_loc(env);
                                    // 
    size_t idx = flow_segment->size;
    add_element(segment, 0); // jump location to l0


    // emit l1 label consequent
    Compiler(conseq, env, labelEnv, segment);
    // emit jump to end of if
    add_element(segment, JEG); // jump opcode
    size_t idx2 = flow_segment->size;
    add_element(segment, 0); // jump location to end of if

    // if false jump to altern
    int64_t l0_label = flow_segment->size;
    Compiler(altern, env, labelEnv, segment);
    // emit cjump to l0
    add_at_index(flow_segment, l0_label, idx);
    // emit jump to end of if
    add_at_index(flow_segment, flow_segment->size, idx2);

}

/*
 * Pairs - cons, car, cdr
 */
void compile_cons(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count != 3) {
        printf("Error: cons expects 2 args\n");
        exit(-7);
    }

    Expr *arg1 = list->as.list.items[1];
    Expr *arg2 = list->as.list.items[2];

    Compiler(arg1, env, labelEnv, segment);
    Compiler(arg2, env, labelEnv, segment);

    // emit cons opcode
    add_element(segment, CONSEG);
    dec_loc(env);
}

void compile_car(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count != 2) {
        printf("Error: car expects 1 argument\n");
        exit(-7);
    }

    Expr *arg1 = list->as.list.items[1];

    Compiler(arg1, env, labelEnv, segment);

    add_element(segment, CAREG);

}

void compile_cdr(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count != 2) {
        printf("Error: cdr expects 1 argument\n");
        exit(-7);
    }

    Expr *arg1 = list->as.list.items[1];

    Compiler(arg1, env, labelEnv, segment);

    add_element(segment, CDREG);

}

/*
 * String
 */
void compile_string(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count < 2) {
        printf("Error: invalid arg\n");
        exit(-8);
    }

    for (size_t i = 1; i < list->as.list.count; i++) {
        Expr *arg = list->as.list.items[i];
        Compiler(arg, env, labelEnv, segment);
    }
    add_element(segment, STREG);
    add_element(segment, (int64_t)(list->as.list.count -1));

    for(size_t i = 1; i < list->as.list.count; i++) {
        dec_loc(env);
    }
    inc_loc(env);
}

void compile_stringRef(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count != 3) {
        printf("Error: stringRef expects 2 argument\n");
        exit(-8);
    }

    Expr *arg1 = list->as.list.items[1];
    Expr *arg2 = list->as.list.items[2];

    Compiler(arg1, env, labelEnv, segment);
    Compiler(arg2, env, labelEnv, segment);

    add_element(segment, REFEG);
    
    dec_loc(env);
}

void compile_stringSet(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count != 4) {
        printf("Error: stringSet expects 3 argument\n");
        exit(-8);
    }

    Expr *arg1 = list->as.list.items[1];
    Expr *arg2 = list->as.list.items[2];
    Expr *arg3 = list->as.list.items[3];

    Compiler(arg1, env, labelEnv, segment);
    Compiler(arg2, env, labelEnv, segment);
    Compiler(arg3, env, labelEnv, segment);

    add_element(segment, SETEG);

    dec_loc(env);
    dec_loc(env);
}

void compile_stringAppend(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count < 2) {
        printf("Error: invalid args\n");
        exit(-8);
    }

    for (size_t i = 1; i < list->as.list.count; i++) {
        Expr *arg = list->as.list.items[i];

        Compiler(arg, env, labelEnv, segment);
    }

    add_element(segment, APPEG);
    add_element(segment, (int64_t)(list->as.list.count - 1));

    for(size_t i = 1; i < list->as.list.count; i++) {
        dec_loc(env);
    }
    inc_loc(env);
}

/*
 *  Vector
 */
void compile_vector(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count < 2) {
        printf("Error: invalid args\n");
        exit(-9);
    }

    for (size_t i = 1; i < list->as.list.count; i++) {
        Expr *arg = list->as.list.items[i];

        Compiler(arg, env, labelEnv, segment);
    }

    add_element(segment, VECTEG);
    add_element(segment, (int64_t)(list->as.list.count - 1));

    for(size_t i = 1; i < list->as.list.count; i++) {
        dec_loc(env);
    }
    inc_loc(env);

}

void compile_vectorRef(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count != 3) {
        printf("Error: invalid args\n");
        exit(-9);
    }

    Expr *arg1 = list->as.list.items[1];
    Expr *arg2 = list->as.list.items[2];

    Compiler(arg1, env, labelEnv, segment);
    Compiler(arg2, env, labelEnv, segment);

    add_element(segment, VREFEG);
    
    dec_loc(env);

}

void compile_vectorSet(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count != 4) {
        printf("Error: vector_set! expects 3 argument\n");
        exit(-8);
    }

    Expr *arg1 = list->as.list.items[1];
    Expr *arg2 = list->as.list.items[2];
    Expr *arg3 = list->as.list.items[3];

    Compiler(arg1, env, labelEnv, segment);
    Compiler(arg2, env, labelEnv, segment);
    Compiler(arg3, env, labelEnv, segment);

    add_element(segment, VSETEG);

    dec_loc(env);
    dec_loc(env);
}

void compile_vectorAppend(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    if (list->as.list.count < 2) {
        printf("Error: invalid args\n");
        exit(-9);
    }

    for (size_t i = 1; i < list->as.list.count; i++) {
        Expr *arg = list->as.list.items[i];
        Compiler(arg, env, labelEnv, segment);
    }

    add_element(segment, VAPPEG);
    add_element(segment, (int64_t)(list->as.list.count - 1));

    for(size_t i = 1; i < list->as.list.count; i++) {
        dec_loc(env);
    }

    inc_loc(env);
}

/*
 * Begin
 */
void compile_begin(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    if (list->as.list.count <= 1) {
        printf("Error: syntax-error: malformed begin\n");
        exit(-9);
    }

    for (size_t i = 1; i < list->as.list.count; i++) {
        Expr *arg = list->as.list.items[i];
        if (i >= list->as.list.count - 1) {
            Compiler(arg, env, labelEnv, segment);
        } else {
            Compiler(arg, env, labelEnv, segment);
            add_element(segment, SIKeEG);
            dec_loc(env);
        }
    }

    add_element(segment, FLEG);
    add_element(segment, 0);

    for(size_t i = 0; i < list->as.list.count; i++) {
        dec_loc(env);
    }

}


/*
 * lambdas,code
 */
void collect_free_vars(Expr *expr, Env *boundedEnv, Env *freeEnv, Env *lexicalEnv) {

    if (expr == NULL) return;
    
    if (expr->type == EXPR_SYMBOL) {
        if (isbuiltin(expr->as.symbol)) return;
        
        if (lookup(boundedEnv, expr->as.symbol) != -1) return;
        
        if (lookup(freeEnv, expr->as.symbol) != -1) return;
        
        if (lookup(lexicalEnv, expr->as.symbol) != -1) {
            add_binding(freeEnv, expr->as.symbol, freeEnv->count);
        }
    }
    else if (expr->type == EXPR_LIST) {
        if (expr->as.list.count == 0) return;
        
        size_t sId = 0;
        Expr *op = expr->as.list.items[0];
        if (op->type == EXPR_SYMBOL && strcmp(op->as.symbol, "lambda") == 0) {
            Expr *inner_params = expr->as.list.items[1];
            Expr *inner_body = expr->as.list.items[2];

            Env *temp_bounded = initializeEnv();
            temp_bounded->parent = boundedEnv;
            for(size_t i = 0; i < inner_params->as.list.count; i++) {
                add_binding(temp_bounded, inner_params->as.list.items[i]->as.symbol,i);
            }

            collect_free_vars(inner_body, temp_bounded, freeEnv, lexicalEnv);
            free_env(temp_bounded);
            return;

        } else {

            if (op->type == EXPR_SYMBOL && isbuiltin(op->as.symbol)) {
                sId = 1; 
            }
            
            for (size_t i = sId; i < expr->as.list.count; i++) {
                collect_free_vars(expr->as.list.items[i], boundedEnv, freeEnv, lexicalEnv);
            }
        }
    }
}

/*
 * code
 */
void compile_code(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {
    Expr *param = list->as.list.items[1];
    Expr *body  = list->as.list.items[2];

    add_element(segment, JEG);
    size_t jump_patch = label_segment->size;
    add_element(segment, 0);  

    int64_t entry_point = segment->size;

    Env *boundedEnv = initializeEnv();
    boundedEnv->type = 2;
    for (size_t i = 0; i < param->as.list.count; i++) {
        add_binding(boundedEnv, param->as.list.items[i]->as.symbol, (int64_t)i);
    }

    Env *freeEnv = initializeEnv();
    freeEnv->type = 1;
    collect_free_vars(body, boundedEnv, freeEnv, env);

    boundedEnv->parent = freeEnv;
    freeEnv->parent = env;

    create_closure(list, freeEnv, labelEnv, entry_point);

    Compiler(body, boundedEnv, labelEnv, segment);
    dec_loc(boundedEnv);

    add_element(segment, RET);
    add_element(segment, param->as.list.count);

    add_at_index(segment, label_segment->size, jump_patch);

    freeEnv->parent = NULL;
    free_env(freeEnv);
    free_env(boundedEnv);
}

/*
 * closure
 */
Expr * create_closure(Expr *list, Env *env, Env *labelEnv, int64_t entry) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_LIST;
    list_expr->as.list.items = malloc(8 * sizeof(Expr*));
    list_expr->as.list.count = 0;
    list_expr->as.list.capacity = 8;

    Expr *opName = malloc(sizeof(Expr));
    opName->type = EXPR_SYMBOL;
    opName->as.symbol = strdup("closure");
    add_to_list(&list_expr->as.list, opName);

    Expr *lambdaLoc = malloc(sizeof(Expr));
    lambdaLoc->type = EXPR_INT;
    lambdaLoc->as.int_val = entry;
    add_to_list(&list_expr->as.list, lambdaLoc);

    for(size_t i = 0; i < env->count; i++) {
        Expr *freeArgs = malloc(sizeof(Expr));
        freeArgs->type = EXPR_SYMBOL;
        freeArgs->as.symbol = strdup(env->val[i].symbol);
        add_to_list(&list_expr->as.list, freeArgs);

    }

    free(list->as.list.items[0]->as.symbol); 
    list->as.list.items[0]->as.symbol = gen_lambdaName();
    init_binding(labelEnv, list->as.list.items[0]->as.symbol);
    labelEnv->val[labelEnv->count - 1].closure = list_expr;

   return list_expr; 
}

/*
 * compile_closure
 */
void compile_closure(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    for(size_t i = 2; i < list->as.list.count; i++) {
        Expr *freeArgs = list->as.list.items[i];
        Compiler(freeArgs, env, labelEnv, segment);
    }

    int64_t d = list->as.list.items[1]->as.int_val;
    add_element(segment, CLOSEURLEG);
    add_element(segment, (int64_t)d);
    add_element(segment, (int64_t)list->as.list.count - 2);

    for(size_t i = 2; i < list->as.list.count; i++) {
        dec_loc(env);
    }
    dec_loc(env);
    inc_loc(env);

}

/*
 * funcall
 */
void funcall(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    for(size_t i = 2; i < list->as.list.count; i++) {
        Expr *args = list->as.list.items[i];
        Compiler(args, env, labelEnv, segment);
    }
    
    Expr *arg = list->as.list.items[1];
    Compiler(arg, env, labelEnv, segment);
    dec_loc(env);

    add_element(segment, CLEG);
    add_element(segment, list->as.list.count - 2);

    for(size_t i = 2; i < list->as.list.count; i++) {
        dec_loc(env);
    }
    dec_loc(env);
    inc_loc(env);
}

/*
 * var call
 */
void compile_var(Expr *list, Env *env, Env *labelEnv, Int64_Array *segment) {

    for(size_t i = 1; i < list->as.list.count; i++) {
        Expr *args = list->as.list.items[i];
        Compiler(args, env, labelEnv, segment);
    }

    Expr *arg = list->as.list.items[0];
    Compiler(arg, env, labelEnv, segment);

    add_element(segment, CLEG);
    add_element(segment, list->as.list.count-1);

    for(size_t i = 1; i < list->as.list.count; i++) {
        dec_loc(env);
    }
    dec_loc(env);
    inc_loc(env);

}

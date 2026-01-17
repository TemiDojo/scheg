#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "./includes/parser.h"
#include "./includes/compiler.h"
#include "./includes/opcodes.h"
#include "./includes/types.h"
#include "./includes/global_helper.h"



//struct RetData ret_struct = {0};
//Int64_Array code_array;

int main(void) {
    //const char *scheme_expr = "#\\newline";
    //const char *scheme_expr = " 42";
    //const char *scheme_expr = "#t";
    //const char *scheme_expr = "()";
    //const char *scheme_expr = "(add1 2)";
    //const char *scheme_expr = "(add1 (sub1 (integer->char 50)))";
    //const char *scheme_expr = "(+ 1 (add1 (- 4 2)))";
    const char *scheme_expr = "(let ((a 2)))";
    //const char *scheme_expr = "(+ 1 (add1 (- 4 2)) 2)";
    //const char *scheme_expr = "(let ((a 0) (b 1)) (- a b)()) (- 1 2)";
    //const char *scheme_expr = "()";



    code_array = initializeInt64_arr();
    Parser p = new_parser(scheme_expr);
    Expr *parsed = scheme_parse(&p);
    display_parsed_list(parsed);
    Compiler(parsed);
    
    // Write to file
    FILE *fptr = fopen("test.scm", "w");
    if (fptr == NULL) {
        printf("File could not be created\n");
        return -1;
    } 
    fwrite(code_array.code, sizeof(int64_t), code_array.size, fptr);
    free(code_array.code);
    free(parsed);

    fclose(fptr);
   

}

Expr* scheme_parse(Parser *p) {
    skip_whitespace(p);

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
    } else if (c == '#'){       // Boolean
        return parse_bool(p);
    } else if (is_symbol_start(c)){
        return parse_symbol(p);
    } else {
        printf("Error: wrong expression '%c' \n", c);
        exit(1);
    }

}


void Compiler(Expr *parsed) {

    if (parsed == NULL) {
        printf("NULL");
        return;
    }

    int64_t tag_num;
    switch (parsed->type) {
        case EXPR_INT: // integer
            add_element(&code_array, KEG);
            // tag the value
            tag_num = tagInt(parsed->as.int_val);
            add_element(&code_array, tag_num);
            break;
        case EXPR_CHAR: // characters
            add_element(&code_array, KEG);
            // tag the value
            tag_num = tagChar(parsed->as.char_val);
            add_element(&code_array, tag_num);
            break;
        case EXPR_BOOL: // booleans
            add_element(&code_array, KEG);
            // tag the value
            tag_num = tagBool(parsed->as.bool_val);
            add_element(&code_array, tag_num);
            break;
        case EXPR_SYMBOL:
            puts("IN THE SYMBOL");
            printf("---- %s\n", parsed->as.symbol);
            break;
        case EXPR_LIST:
            compile_list(parsed);
            break; 
        default:
            printf("Error: unknown expression type\n");
            break;
    }
}

void compile_list(Expr *list) {

    // empty list
    if (list->as.list.count == 0) {
        add_element(&code_array, KEG);
        add_element(&code_array, MT_MASK);
        return;
    }

    Expr *op = list->as.list.items[0];
    if (op->type != EXPR_SYMBOL) {
        printf("Error: expected operator to be a symbol\n");
        return;
    }

    const char *op_name = op->as.symbol;
    // unary primitives
    if (strcmp(op_name, "add1") == 0) {
        // we load some 
        compile_add1(list);
    } else if(strcmp(op_name, "sub1") == 0) {
        compile_sub1(list);
    } else if(strcmp(op_name, "integer->char") == 0) {
        compile_int2char(list);
    } else if(strcmp(op_name, "char->integer") == 0) {
        compile_char2int(list);
    } else if(strcmp(op_name, "null?") == 0) {
        compile_nullp(list);
    } else if(strcmp(op_name, "zerop") == 0) {
        compile_zerop(list);
    } else if(strcmp(op_name, "not") == 0) {
        compile_not(list);
    } else if(strcmp(op_name, "integer?") == 0) {
        compile_intp(list);
    } else if(strcmp(op_name, "boolean?") == 0) {
        compile_boolp(list);
    } 
    // Binary primitives
    else if(strcmp(op_name, "+") == 0) {
        compile_add(list);
    } else if (strcmp(op_name, "-") == 0) {
        compile_sub(list);
    } else if(strcmp(op_name, "*") == 0) {
        compile_mul(list);
    } else if(strcmp(op_name, "<") == 0) {
        compile_le(list);
    } else if(strcmp(op_name, "=") == 0) {
        compile_eq(list);
    // Local variables
    } else if(strcmp(op_name, "let") == 0) {
        compile_let(list);
    } else {
        printf("Error: unknown operator '%s'\n", op_name);
    }


}

/*
 * Unary Primitives
 */
void compile_add1(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: add1 expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, AEG1);
}

void compile_sub1(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: sub1 expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, SEG1);
}

void compile_int2char(Expr *list){
    if (list->as.list.count != 2) {
        printf("Error: int2char expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, IEG);
}

void compile_char2int(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: char2int expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, CEG);
}

void compile_nullp(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: nullp expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, NEG);
}

void compile_zerop(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: zerop expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, ZEG);
}

void compile_not(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: not expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, NEG);
}

void compile_intp(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: intp expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);
    
    add_element(&code_array, sIEG);
}

void compile_boolp(Expr *list) {
    if (list->as.list.count != 2) {
        printf("Error: boolp expects 1 argument\n");
        return;
    }
    Expr *arg = list->as.list.items[1];
    Compiler(arg);

    add_element(&code_array, sIEG);
}


/*
 * Binary Primitives
 */
void compile_sub(Expr *list) {
    if (list->as.list.count != 3) {
        printf("Error: - expects 2 argument\n");
        return;
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2);

    add_element(&code_array, SEG);
}


void compile_add(Expr *list) {
    if (list->as.list.count != 3) {
        printf("Error: + expects 2 argument\n");
        return;
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2);

    add_element(&code_array, AEG);
}


void compile_mul(Expr *list) {
    if (list->as.list.count != 3) {
        printf("Error: * expects 2 argument\n");
        return;
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2);

    add_element(&code_array, MEG);
}


void compile_le(Expr *list) {
    if (list->as.list.count != 3) {
        printf("Error: < expects 2 argument\n");
        return;
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2);

    add_element(&code_array, LEG);
}


void compile_eq(Expr *list) {
    if (list->as.list.count != 3) {
        printf("Error: = expects 2 argument\n");
        return;
    }
    Expr *arg1 = list->as.list.items[2];
    Compiler(arg1);

    Expr *arg2 = list->as.list.items[1];
    Compiler(arg2);

    add_element(&code_array, EEG);
}


/*
 * Local variables
 */
void compile_let(Expr *list) {
    if (list->as.list.items[1]->as.list.count > 2 || list->as.list.items[1]->as.list.count <= 0) {
        printf("Error: wrong let expression\n");
        return;
    }
    Expr *arg1 = list->as.list.items[1]; 
    for(size_t i = 0; i < arg1->as.list.count; i++) {
        if (arg1->as.list.items[i]->type != EXPR_LIST) {
            printf("Error: wrong let arg type expression\n");
            return;
        }
        // evaluate the right most arg
        Expr *arg = arg1->as.list.items[i]->as.list.items[1];
        Compiler(arg);
    }


}

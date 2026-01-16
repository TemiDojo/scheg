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
    const char *scheme_expr = "(add1 (add1 (add1 50)))";
    //const char *scheme_expr = "(+ 1 (add1 (- 1) 2))";
    //const char *scheme_expr = "(let ((a 0) (b 1)) (- a b)()) (- 1 2)";
    //const char *scheme_expr = "()";


    //const char *scheme_expr = "add1 ";

    //scheme_parse(scheme_expr);
    //trim_str(scheme_expr);
    //listUnrolled_expr = calloc(1, sizeof(TypeData))

    //type = calloc(1, sizeof(union MultiType));
    code_array = initializeInt64_arr();
    //parsed_expr = calloc(1, sizeof(SchemeParseRet));
    Parser p = new_parser(scheme_expr);
    Expr *parsed = scheme_parse(&p);
    display_parsed_list(parsed);
    Compiler(parsed);
    printf("size: %ld\n", code_array.size);
    
    //Compiler(scheme_parse(&p));
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
        puts("parsing expression list");
        return parse_expr(p);
    } else if (isdigit(c)) {    // integer
        puts("parsing number");
        return parse_number(p);
    } else if (c == '-'){
        if (isdigit(cN)) {
            return parse_number(p);
        } else {
            return parse_symbol(p);
        }
    } else if ((c == '#') && (cN == '\\')){     // character
        puts("parsing char");
        return parse_char(p);
    } else if (c == '#'){       // Boolean
        puts("parsing bool");
        return parse_bool(p);
    } else if (is_symbol_start(c)){
        puts("parsing symbol");
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
    puts("in here");
    switch (parsed->type) {
        case EXPR_INT: // integer
            add_element(&code_array, LEG);
            // tag the value
            tag_num = tagInt(parsed->as.int_val);
            add_element(&code_array, tag_num);
            printf("Checking the result: %ld\n", code_array.code[1]);

            break;
        case EXPR_CHAR: // characters
            add_element(&code_array, LEG);
            // tag the value
            tag_num = tagChar(parsed->as.char_val);
            add_element(&code_array, tag_num);
            printf("Checking the result: %ld", code_array.code[1]);
            break;
        case EXPR_BOOL: // booleans
            add_element(&code_array, LEG);
            // tag the value
            tag_num = tagBool(parsed->as.bool_val);
            add_element(&code_array, tag_num);
            printf("Checking the result: %ld\n", code_array.code[1]);
            break;
        case EXPR_SYMBOL:
            printf("Checking the result: %s\n", parsed->as.symbol);
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
        add_element(&code_array, LEG);
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
        puts("add1");
        compile_add1(list);
    } else if(strcmp(op_name, "sub1") == 0) {
        puts("sub1");
    } else if(strcmp(op_name, "integer->char") == 0) {
        puts("integer->char");
    } else if(strcmp(op_name, "char->integer") == 0) {
        puts("char->integer");
    } else if(strcmp(op_name, "null?") == 0) {
        puts("null?");
    } else if(strcmp(op_name, "integer?") == 0) {
        puts("integer?");
    } else if(strcmp(op_name, "boolean?") == 0) {
        puts("boolean?");
    }


}


#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "./includes/compiler.h"
#include "./includes/opcodes.h"
#include "./includes/types.h"


//struct RetData ret_struct = {0};
Int64_Array code_array;

int main(void) {
    //const char *scheme_expr = "#\\newline";
    //const char *scheme_expr = " 42";
    //const char *scheme_expr = "#t";
    //const char *scheme_expr = "()";
    const char *scheme_expr = "(add1 2)";
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
    printf("%d\n", code_array.size);
    
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
            puts("case integer");
            add_element(&code_array, LEG);
            // tag the value
            tag_num = tagInt(parsed->as.int_val);
            //add_element(&code_array, tag_num);

            if (code_array.opcode_flag != 0) {
                add_element(&code_array, tag_num);
                if (code_array.arg_count > 0) {
                    code_array.arg_count--;
                }
                if (code_array.arg_count == 0) {
                    add_element(&code_array, AEG1);
                    code_array.opcode_flag = 0;
                }
            } else {
                add_element(&code_array, tag_num);
            }
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
            const char *s1 = parsed->as.symbol;
            // unary primitives
            if (strcmp(s1, "add1") == 0) {
                // we load some 
                puts("add1");
                code_array.opcode_flag = (int64_t) AEG1;
                code_array.arg_count = 1;
            }
            break;
        case EXPR_LIST:
            if (parsed->as.list.count == 0) {
                add_element(&code_array, LEG);
                int64_t mask_num = (int64_t) MT_MASK;
                add_element(&code_array, mask_num);
                printf("Checking the result: %ld\n", code_array.code[1]);
            } else {
                for (size_t i = 0; i < parsed->as.list.count; i++) {

                    Compiler(parsed->as.list.items[i]);
                    
                }


            }
            break; 
        default:
            break;
    }
}


Expr* parse_number(Parser *p) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_INT;
    list_expr->as.int_val = 0;
    bool neg = false;
    if (peek(p) == '-') {
        neg = true;
        advance(p);
    }

    while(isdigit(peek(p))) {
        //printf("%c", p);    
        list_expr->as.int_val = list_expr->as.int_val * 10 + (advance(p) - '0');
    }

    if (neg) {
        list_expr->as.int_val = -(list_expr->as.int_val);
        return list_expr;
    }
    printf("%ld\n", list_expr->as.int_val);
    return list_expr;

}

Expr* parse_char(Parser *p) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_CHAR;
    char c;
    const char *s1 = p->source + (p->pos+2);
    if (peek(p) >= 33 && peek(p) <= 126 && strlen(s1) == 1) {
       c = peek(p);
       list_expr->as.char_val = (int64_t) c;
       return list_expr;
    } else {
        if (strcmp(s1, "newline") == 0) {
            printf("in the newline\n");
            list_expr->as.char_val = (int64_t) '\n';
            return list_expr;
        } else if (strcmp(s1, "space") == 0) {
            list_expr->as.char_val = (int64_t) ' ';
            return list_expr;
        }
    }

    printf("Error: expected a character\n");
    exit(1);
}


Expr* parse_bool(Parser *p) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_BOOL;
    char c = advanceN(p);
    printf("bool: %c\n", c);
    if (c == 't') {
        list_expr->as.bool_val = 0;
        return list_expr;
    } else if (c == 'f') {
        list_expr->as.bool_val = 1;
        return list_expr;
    }

    printf("Error: expected a boolean \n");
    exit(1);
}


Expr* parse_expr(Parser *p) {
    advance(p); // consume '('

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_LIST;
    list_expr->as.list.items = malloc(8 * sizeof(Expr*));
    list_expr->as.list.count = 0;
    list_expr->as.list.capacity = 8;
    
    while(1) {
        skip_whitespace(p);
        if (peek(p) == ')') {
            advance(p); // consume ');
            printf("%ld", list_expr->as.list.count);
            if (list_expr->as.list.count == 0) {
                puts("empty list");
            }
            puts("closing list expr");
            break;
        }

        Expr *item = scheme_parse(p);
        add_to_list(&list_expr->as.list, item);
    }
    return list_expr;
}



Expr* parse_symbol(Parser *p) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_SYMBOL;

    //char *symbol = malloc(64 *sizeof(char));
    char c = peek(p);
    int i = 0;
    while(c != '\0' && c != ' ' && c != ')') {
        list_expr->as.symbol[i] = c;
        i++;
        advance(p);
        c = peek(p);
    }
    printf("Symbol parsed: %s\n",list_expr->as.symbol);
    // parser not in charge of that
    return list_expr;

}

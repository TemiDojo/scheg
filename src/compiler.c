#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "compiler.h"
#include "opcodes.h"
#include "types.h"


//struct RetData ret_struct = {0};
SchemeParseRet *parsed_expr;
struct TypeData *listUnrolled_expr;
Int64_Array code_array;
//int64_t *code;

int main(void) {
    //const char *scheme_expr = "#\\newline";
    //const char *scheme_expr = " 42";
    const char *scheme_expr = "#t";
    //const char *scheme_expr = "()";

    //scheme_parse(scheme_expr);
    //trim_str(scheme_expr);
    //listUnrolled_expr = calloc(1, sizeof(TypeData))

    code_array = initializeInt64_arr();
    parsed_expr = calloc(1, sizeof(SchemeParseRet));
    Parser p = new_parser(scheme_expr);
    Compiler(scheme_parse(&p));
    // Write to file
    FILE *fptr = fopen("test.scm", "w");
    if (fptr == NULL) {
        printf("File could not be created\n");
        return -1;
    } 
    fwrite(code_array.code, sizeof(int64_t), code_array.size, fptr);

}

SchemeParseRet *scheme_parse(Parser *p) {
    skip_whitespace(p);

    char c = peek(p);
    char cN = advanceN(p);

    if (c == '\0') {
        // just whitespace, nothing else
        printf("Error: unexpected end of input\n");
        exit(1);
    } else if (isdigit(c) || c == '-') {    // integer
        int64_t res = parse_number(p);
        parsed_expr->ret_tag = INTEGER;
        parsed_expr->retstruct.integer = res;
        printf("parsed number is: %d\n", res);
    } else if ((c == '#') && (cN == '\\')){     // character
        int64_t char_res = parse_char(p);
        parsed_expr->ret_tag = CHARS;
        parsed_expr->retstruct.character = char_res;
        printf("parsed char is: %c", char_res);
    } else if (c == '#'){       // Boolean
        int64_t bool_res = parse_bool(p);
        parsed_expr->ret_tag = BOOLS;
        parsed_expr->retstruct.bools = bool_res;
    } else if ((c == '(') && (cN == ')')) {     // empty list
        parsed_expr->ret_tag = MT_LIST;
        parsed_expr->retstruct.mt_list = MT_MASK;
    } else if (c == '(') {      // list expr
        listUnrolled_expr = calloc(1, sizeof(struct TypeData));
        parsed_expr->retstruct.list_ret = listUnrolled_expr;
        parse_expr(p);
    }

    return parsed_expr;
}


void Compiler(SchemeParseRet *parsed) {


    int64_t tag_num;

    switch (parsed->ret_tag) {
        case INTEGER: // integer
            add_element(&code_array, LOAD64);
            // tag the value
            tag_num = tagInt(parsed->retstruct.integer);
            add_element(&code_array, tag_num);
            printf("Checking the result: %d\n", code_array.code[1]);
            break;
        case CHARS: // characters
            add_element(&code_array, LOAD64);
            // tag the value
            tag_num = tagChar(parsed->retstruct.character);
            add_element(&code_array, tag_num);
            printf("Checking the result: %d", code_array.code[1]);
            break;
        case BOOLS: // booleans
            add_element(&code_array, LOAD64);
            // tag the value
            tag_num = tagBool(parsed->retstruct.bools);
            add_element(&code_array, tag_num);
            printf("Checking the result: %d\n", code_array.code[1]);
            break;
        case MT_LIST:
            add_element(&code_array, LOAD64);
            int64_t mask_num = parsed->retstruct.mt_list;
            add_element(&code_array, mask_num);
            printf("Checking the result: %d\n", code_array.code[1]);
            break; 
        default:
            break;
    }
}

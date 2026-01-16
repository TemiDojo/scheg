#ifndef PARSER_H
#define PARSER_H

#include "compiler.h"
#include "global_helper.h"

// parser struct
typedef struct {
    const char *source;
    int pos;
    int length;
} Parser;



/*
 * Function Declaration
 */
Parser new_parser(const char *source);
Expr* scheme_parse(Parser *p);
struct TypeData *Parse(const char *scheme_expr);
void skip_whitespace(Parser *p);
char peek(Parser *p);
char advance(Parser *p);
char advanceN(Parser *p);
Expr* parse_number(Parser *p);
Expr* parse_char(Parser *p);
Expr* parse_bool(Parser *p);
Expr* parse_expr(Parser *p);
Expr* parse_symbol(Parser *p);


/*
 * Parser Class initialization
 */
Parser new_parser(const char *source) {
    Parser p;
    p.source = source;
    p.pos = 0;
    p.length = strlen(source);
    return p;
}

/*
 * Parser HELPERS
 */
char peek(Parser *p) {
    if(p->pos >= p->length) {
        return '\0';
    }
    return p->source[p->pos];
}

char advance(Parser *p) {
    return p->source[p->pos++];
}

char advanceN(Parser *p) {
    int dummy = p->pos;
    dummy++;
    return p->source[dummy];
}

void skip_whitespace(Parser *p) {
    while(p->pos < p->length) {
        char c = peek(p);
        if (c == ' ') {
            advance(p);
        } else {
            break;
        }
    }
}

/*
 * Main Parser logic
 *
 */
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
    const char *op_name = p->source + (p->pos+2);
    if (peek(p) >= 33 && peek(p) <= 126 && strlen(op_name) == 1) {
       c = peek(p);
       list_expr->as.char_val = (int64_t) c;
       return list_expr;
    } else {
        if (strcmp(op_name, "newline") == 0) {
            printf("in the newline\n");
            list_expr->as.char_val = (int64_t) '\n';
            return list_expr;
        } else if (strcmp(op_name, "space") == 0) {
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


#endif

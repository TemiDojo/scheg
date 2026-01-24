#ifndef PARSER_H
#define PARSER_H

#include "compiler.h"
#include "global_helper.h"

// parser struct
typedef struct {
    const char *source;
    size_t pos;
    size_t length;
} Parser;



/*
 * Function Declaration
 */
Parser new_parser(char *source);
Expr* scheme_parse(Parser *p);
struct TypeData *Parse(const char *scheme_expr);
void skip_whitespace(Parser *p);
void skip_comments(Parser *p);
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
Parser new_parser(char *source) {
    Parser p;
    p.source = source;
    p.pos = 0;
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
    // if (p->pos >= p->length) {
    //     return '\0';
    // }
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
        if (c == ' ' || c == '\n' || c == '\t') {
            advance(p);
        } else {
            break;
        }
    }
}

void skip_comments(Parser *p) {
    advance(p); // consume ;
    while(peek(p) != '\n') {
        advance(p);
    }
    advance(p); 
}

void skip_multiline_comments(Parser *p) {
    advance(p); // consume #
    advance(p); // consume |
    while(peek(p) != '|' && advanceN(p) != '#') {
        advance(p);
    }
    advance(p); // consume #
    advance(p); // consume |
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
    return list_expr;

}

Expr* parse_char(Parser *p) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_CHAR;
    advance(p);
    advance(p);
    if (strncmp(p->source + p->pos, "newline", 7) == 0) {
        list_expr->as.char_val = (int64_t) '\n';
        p->pos+=7;
    } else if (strncmp(p->source + p->pos, "space", 5) == 0) {
        list_expr->as.char_val = (int64_t) ' ';
        p->pos += 5;
    } else {
        char c = advance(p);
        if (peek(p) == ')' || peek(p) == ';' || peek(p) == ' ' || peek(p) == '\n'){
            list_expr->as.char_val = (int64_t) c;
        } else {
            printf("Error: invalid char\n");
            exit(-1);
        }
    }
    /*
    if (peek(p) == ';') {
        skip_comments(p);
    }
    */

    return list_expr;
}


Expr* parse_bool(Parser *p) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_BOOL;
    advance(p);
    char c = advance(p);
    if (c == 't') {
        list_expr->as.bool_val = 1;
    } else if (c == 'f') {
        list_expr->as.bool_val = 0;
    } else {
        printf("Error: expected a boolean\n");
        exit(-1);
    }
    return list_expr;

}

Expr* parse_symbol(Parser *p) {

    Expr *list_expr = malloc(sizeof(Expr));
    list_expr->type = EXPR_SYMBOL;

    size_t cap = 16;
    //char *symbol = malloc(64 *sizeof(char));
    list_expr->as.symbol = malloc(cap *sizeof(char));
    if (list_expr->as.symbol == NULL) {
        printf("Error: allocation failure\n");
        exit(-1);
    }
    char c = peek(p);
    size_t i = 0;

    while(c != '\0' && c != ' ' && c != '\n' && c != ')' && c != ';') {
        if (i >= cap - 1) {
            cap = cap * 2;
            list_expr->as.symbol = realloc(list_expr->as.symbol, cap * sizeof(char));
        }
        if (list_expr->as.symbol == NULL) {
            printf("Error: allocation failure\n");
            exit(-1);
        }
        list_expr->as.symbol[i] = c;
        i++;
        advance(p);
        c = peek(p);
    }
    list_expr->as.symbol[i] = '\0';
    if (c == ';') {
        skip_comments(p);
    }
    //printf("Symbol parsed: %s\n",list_expr->as.symbol);
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
        if (p->pos >= p->length) { 
            printf("Error: invalid expression");
            exit(-1);
        } else if (peek(p) == ')') {
            advance(p); // consume ');
                        /*
            if (list_expr->as.list.count == 0) {
                puts("empty list");
            }
            */
            break;
        } else if (peek(p) == ';') {
            skip_comments(p);
            continue;
        }

        Expr *item = scheme_parse(p);
        add_to_list(&list_expr->as.list, item);
    }
    return list_expr;
}


#endif

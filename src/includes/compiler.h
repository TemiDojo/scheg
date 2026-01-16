#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// forward declaration
typedef struct Expr Expr;

typedef enum {
    EXPR_INT,
    EXPR_CHAR,
    EXPR_BOOL,
    EXPR_SYMBOL,
    EXPR_LIST
} ExprType;

// A list of expressions (for nested lists)
typedef struct {
    Expr **items; // array of pointers to expression
    size_t count;
    size_t capacity;
} ExprList;

// The expression itself
struct Expr {
    ExprType type;
    union {
        int64_t int_val;        // EXPR_INT
        int64_t char_val;       // EXPR_CHAR
        int64_t bool_val;       // EXPR_BOOL
        int64_t mt_list;
        char symbol[64];        // EXPR_SYMBOL
        ExprList list;          // EXPR_LIST
    } as;
};

// parser struct
typedef struct {
    const char *source;
    int pos;
    int length;
} Parser;


//
typedef struct {
    int64_t *code;
    size_t size;
    size_t capacity;
    int64_t opcode_flag;
    size_t arg_count;
} Int64_Array;


#define INT_TAG     0b00
#define CHAR_TAG    0b00001111
#define BOOL_TAG    0b0011111
#define MT_MASK     0b00101111

static const char *const unary[] = {
    "add1", "sub1", "integer->char", "char->integer",
    "null?", "zero", "not", "integer?", "boolean?"};
    
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
void Compiler(Expr *parsed);
void add_to_list(ExprList *list, Expr *item);
int64_t tagInt(int64_t integer);
int64_t tagChar(int64_t chars);
int64_t tagBool(int64_t bools);
int64_t tagEmptyList();
int64_t untagInt(int64_t integer);
bool is_symbol_char(char c);
bool is_symbol_start(char c);
void display_parsed_list(Expr *parsed);



int64_t untagInt(int64_t integer) {
    return (integer >> 2) | INT_TAG;
}

void display_parsed_list(Expr *parsed) {
    if (parsed == NULL) {
        printf("NULL");
        return;
    }
    
    switch (parsed->type) {
        case EXPR_INT:
            printf("%lld", (long long)parsed->as.int_val);
            break;
            
        case EXPR_CHAR:
            // Print character in Scheme format
            if (parsed->as.char_val == '\n') {
                printf("#\\newline");
            } else if (parsed->as.char_val == ' ') {
                printf("#\\space");
            } else {
                printf("#\\%c", (char)parsed->as.char_val);
            }
            break;
            
        case EXPR_BOOL:
            if (parsed->as.bool_val == 0) {
                printf("#t");
            } else {
                printf("#f");
            }
            break;
            
        case EXPR_SYMBOL:
            printf("%s", parsed->as.symbol);
            break;
            
        case EXPR_LIST:
            printf("(");
            for (size_t i = 0; i < parsed->as.list.count; i++) {
                if (i > 0) {
                    printf(" ");  // Space between elements
                }
                // RECURSIVE CALL for each list item
                display_parsed_list(parsed->as.list.items[i]);
            }
            printf(")");
            break;
            
        default:
            printf("UNKNOWN");
            break;
    }
}

bool is_symbol_start(char c) {
    // First character of symbol (cannot start with digit)
    return isalpha(c) ||
           c == '+' || c == '-' || c == '*' ||
           c == '<' || c == '>' || c == '=';
}

bool is_symbol_char(char c) {
    // Any character within a symbol (digits allowed)
    return is_symbol_start(c) || isdigit(c);
}



int64_t tagInt(int64_t integer) {
    return (integer << 2) | INT_TAG;
}

int64_t tagChar(int64_t chars){
    return (chars << 8) | CHAR_TAG;
}

int64_t tagBool(int64_t bools) {
    return (bools << 7) | BOOL_TAG;
}


Parser new_parser(const char *source) {
    Parser p;
    p.source = source;
    p.pos = 0;
    p.length = strlen(source);
    return p;
}

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


Int64_Array initializeInt64_arr() {
    Int64_Array code_array;
    code_array.size = 0;
    code_array.capacity = 1;
    code_array.opcode_flag = 0;
    code_array.arg_count = 0;
    code_array.code = calloc(code_array.capacity, sizeof(int64_t));
    return code_array;
}

void add_element(Int64_Array *code_array, int64_t code) {
    
    if (code_array->size >= code_array->capacity) {
        code_array->capacity = code_array->capacity * 2; 

        code_array->code = realloc(code_array->code, code_array->capacity * sizeof(int64_t));

        if (code_array->code == NULL) {
            printf("Error: allocation failure\n");
            exit(1);
        }
    }
    code_array->code[code_array->size++] = code;

}


void add_to_list(ExprList *list, Expr *item) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, list->capacity * sizeof(Expr*));
        if (list->items == NULL) {
            printf("Error: allocation failure\n");
            exit(1);
        }
    }

    list->items[list->count++] = item;

}




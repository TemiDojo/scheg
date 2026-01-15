// parser struct
typedef struct {
    const char *source;
    int pos;
    int length;
} Parser;

// multitype struct for the expanded expression
union MultiType {
    int64_t integer;                    // 0
    char *str;                      // 1
};


struct TypeData {
    int type_tag;
    union MultiType type;

};

//
union Ret {
    int64_t integer;                // 0
    int64_t character;              // 1
    int64_t bools;                  // 2
    int64_t mt_list;                // 3
    struct TypeData *list_ret;      // 4
};

//
typedef struct {
   int ret_tag;
   union Ret retstruct;
} SchemeParseRet;

//
typedef struct {
    int64_t *code;
    size_t size;
    size_t capacity;
} Int64_Array;

#define INT_TAG     0b00
#define CHAR_TAG    0b00001111
#define BOOL_TAG    0b0011111
#define MT_MASK     0b00101111
    
Parser new_parser(const char *source);
SchemeParseRet *scheme_parse(Parser *p);
struct TypeData *Parse(const char *scheme_expr);
void skip_whitespace(Parser *p);
void unroll_expr();
char peek(Parser *p);
char advance(Parser *p);
char advanceN(Parser *p);
int64_t parse_number(Parser *p);
int64_t parse_char(Parser *p);
int64_t parse_bool(Parser *p);
void parse_expr(Parser *p);
void Compiler(SchemeParseRet *parsed);
int64_t tagInt(int64_t integer);
int64_t tagChar(int64_t chars);
int64_t tagBool(int64_t bools);
int64_t tagEmptyList();

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

int64_t parse_number(Parser *p) {
    
    int64_t result = 0;
    bool neg = false;
    if (peek(p) == '-') {
        neg = true;
        advance(p);
    }

    while(isdigit(peek(p))) {
       result = result * 10 + (advance(p) - '0');
    }
    if (peek(p) == ' ') {
        skip_whitespace(p);
        char c = peek(p);
        if (c == '\0') {
            if (neg) return -result;
            return result;
        } else {
            printf("Error: expected a number\n");
            exit(1);
        }
    } else if (peek(p) == '\0') {
        if (neg) return -result;
        return result;
    }
    printf("Error: expected a number\n");
    exit(1);

}

int64_t parse_char(Parser *p) {

    char c;
    const char *s1 = p->source + (p->pos+2);
    if (peek(p) >= 33 && peek(p) <= 126 && strlen(s1) == 1) {
       c = peek(p); 
       return (int64_t) c;
    } else {
        if (strcmp(s1, "newline") == 0) {
            printf("in the newline\n");
            return (int64_t) '\n';
        } else if (strcmp(s1, "space") == 0) {
            return (int64_t) ' ';
        }
    }

    printf("Error: expected a character\n");
    exit(1);
}

int64_t parse_bool(Parser *p) {

    char c = advanceN(p);
    printf("bool: %c\n", c);
    if (c == 't') {
        return 0;
    } else if (c == 'f') {
        return 1;
    }

    printf("Error: expected a boolean \n");
    exit(1);
}


void parse_expr(Parser *p) {
   char *expr;
   memcpy(&expr, p->source, p->length);
   unroll_expr(&expr);

}

void unroll_expr() {



}


Int64_Array initializeInt64_arr() {
    Int64_Array code_array;
    code_array.size = 0;
    code_array.capacity = 1;
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

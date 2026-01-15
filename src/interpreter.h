#include <inttypes.h>



#define BOX_TOP "╔══════════════════════════════╗"
#define BOX_SIDE "║"
#define BOX_DIVIDER "╠══════════════════════════════╣"
#define BOX_BOTTOM "╚══════════════════════════════╝"



int64_t data;
int64_t instr;
FILE *fp;
size_t codes_read;
Int64_Array stack;



void interpret();
void read_word();
void push(int64_t data);
void pop();



void push(int64_t data) {
    add_element(&stack, data);
}


void visualize_stack() {
    puts(BOX_TOP);
    for(size_t i = 0; i < stack.size; i++) {
        printf(BOX_SIDE "      0x%016" PRIx64 "      " BOX_SIDE "\n", stack.code[i]);
    }

    printf(BOX_BOTTOM " <- rsp\n");

}

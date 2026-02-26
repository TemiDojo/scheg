### ERROR CODES && WHERE TO FIND IF YOU'RE SO LOST:(
- `-1` 
    - src/includes/compiler.h
    - src/includes/parser.h
    - src/includes/global_helper.h
- `-2` 
    - scheme_parse(Parser *p)
    - Compiler(Expr *parsed, Env *env)
    - src/interpreter.c
- `-3` 
    - compile_list(Expr *list, Env *env) -> src/compiler.c
- `-4` 
    - All unary operators function -> src/compiler.c
- `-5` 
    - Binary operators function -> src/compiler.c
- `-6` 
    - compile_let(Expr *list, Env *env) -> src/compiler.c
    - compile_if(Expr *list, Env *env)  -> src/compiler.c
- `-7`
    - Pairs - cons, car, cdr -> src/compiler.c
- `-8`
    - String, string-ref, string-append, string-set -> src/compiler.c
- `9`
    - begin, vector, vector-set, vector-append, vector-ref -> src/compiler.c


### ABSURDLYeg COOLeg OPCODEeg &eg TRANSLATIONSeg:)
```c
#define KEG     0x1E    // LOAD
#define KLEG    0x2E    // LOAD local variable stack index onto the stack[but rlly I don't have to do that will remove eventually]
#define RET     0x3E    // RET :) only normal instr
#define DEG     0x4E    // DEG clears the stack after a root expression
#define LLEG    0x5E    // I forgot what this does actually
#define cJEG    0x6E    // conditional jump
#define JEG     0x7E    // just jump

// unary primitives
#define SEG1    0x2E5   // subtract 1
#define AEG1    0x3E5   // add 1
#define CEG     0x4E5   // char->integer
#define IEG     0x5E5   // integer->char
#define ZEG     0x6E5   // zero?
#define NEG     0x7E5   // not
#define sIEG    0x8E5   // integer?
#define sBEG    0x9E5   // boolean?
// forgot to implement null? :) but I actually have the
// code to compile it in compiler.c
// will add later and call NUUEG

// binary primitives
#define SEG     0x21E5  // -
#define AEG     0x31E5  // +
#define MEG     0x41E5  // *
#define LEG     0x51E5  // <
#define EEG     0x61E5  // =

// Pairs
#define CONSEG  0x22E5  // (cons _ _)
#define CAREG   0x23E5
#define CDREG   0x24E5

// String
#define STREG   0x25E5  // String
#define REFEG   0x26E5  // string-ref
#define SETEG   0x27E5  // string-set
#define APPEG   0x28E5  // string-append

// Vector
#define VECTEG  0x29E5  // vector
#define VREFEG  0x20E5  // vector-ref
#define VSETEG  0x30E3  // vector-set
#define VAPPEG  0x32E5  // vector-append

// LabelCall
#define LABELEGS    0x33E5  // start of the label section
#define CLOSEURLEG  0x35E5  // closure
#define CLEG        0x36E5  // call opcode
#define HEG         0x37E5  // push/get opcode from closure
#define BLEG        0x38E5  // load from lambda args
```

### HOW TO RUN
#### Step 1
JUST RUN `make clean && make`
#### Step 2
Make produces `compile` & `interpret` executables. Pass in your scheme expression to `./compile` through either a file (`./compile -c <file-pathname> -o <output_filename.out>`) or the cmd-line (`./compile`). After a successful run it produces a output with the default name `put.out` or if the `-o` flag is specified, `<output_filename.out>`. 
#### Step 3
Finally to test the interpreter, run `./interpret <output_filename.out>`.
#### Step 4
wait for the EGciting results

### HOW TO TEST
A script called `test.sh` is already provided in the `src` folder. Run the script and it will automatically test all the testcases provided in the test folders against the compiler and interpreter. Additionally, uncomment any lines in the `src/tests/test1` & `src/tests/test2` files, you wish to test and run the test script or test is manually.

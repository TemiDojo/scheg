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

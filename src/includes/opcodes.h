#ifndef OPCODES_DEF_H 
#define OPCODES_DEF_H

#define KEG     0x1E    // LOAD
#define KLEG    0x2E    // LOAD local variable stack index onto the stack[but rlly I don't have to do that will remove eventually]
#define RET     0x3E    // RET :) only normal instr
#define DEG     0x4E    // DEG clears the stack after a root expression
#define FLEG    0x9E    // 
#define SIKeEG  0x8E
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
// will add later

// binary primitives
#define SEG     0x21E5  // -
#define AEG     0x31E5  // +
#define MEG     0x41E5  // *
#define LEG     0x51E5  // <
#define EEG     0x61E5  // =
                        //
// Pairs
#define CONSEG  0x22E5  // (cons _ _)
#define CAREG   0x23E5
#define CDREG   0x24E5

// String
#define STREG   0x25E5

#endif

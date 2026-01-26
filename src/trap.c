#include <stdio.h>

#include "trap.h"
#include "lc3.h"
#include "memory.h"

void trap_puts();
void trap_getc();
void trap_out();
void trap_in();
void trap_putsp();
void trap_halt();

void trap_handler(uint16_t instr, int* running){
    
    reg[R_R7] = reg[R_PC];

    switch (instr & 0xFF)
    {
        case TRAP_GETC:
            trap_getc();
            break;
        case TRAP_OUT:
            trap_out();
            break;
        case TRAP_PUTS:
            trap_puts();
            break;
        case TRAP_IN:
            trap_in();
            break;
        case TRAP_PUTSP:
            trap_putsp();
            break;
        case TRAP_HALT:
            trap_halt(running);
            break;
    }
}

/* output a null-terminated string  */
void trap_puts(){
    
    uint16_t* c = memory + reg[R_R0];
    while (*c)
    {
        putc((char)*c, stdout);
        ++c;
    }
    fflush(stdout);
}

void trap_getc(){

    reg[R_R0] = (uint16_t)getchar();
    update_flags(R_R0);
}

void trap_out(){
    
    putc((char)reg[R_R0], stdout);
    fflush(stdout);
}

void trap_in(){

   printf("Enter a character: ");
   char c = getchar();

   putc(c, stdout);
   fflush(stdout);

   reg[R_R0] = (uint16_t)c;
   update_flags(R_R0);
}

void trap_putsp(){
    
    uint16_t* c = memory + reg[R_R0];

    while (*c)
    {
        char byte_one = (*c) & 0xFF;
        char byte_two = (*c >> 8) & 0xFF;

        putc(byte_one, stdout);
        if (byte_two) putc(byte_two, stdout);

        c++;
    }
    fflush(stdout);
}

void trap_halt(int* running){
    puts("HALT");
    fflush(stdout);
    *running = 0;
}
#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>


void trap_handler(uint16_t instr, int * running);

#endif
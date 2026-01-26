#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include "lc3.h"

extern uint16_t memory[MEMORY_MAX];

void mem_write(uint16_t address, uint16_t val);
uint16_t mem_read(uint16_t address);
uint16_t check_key();

#endif
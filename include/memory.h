#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include "lc3.h"

extern uint16_t memory[MEMORY_MAX];

void mem_write(uint16_t address, uint16_t val);
uint16_t mem_read(uint16_t address);
uint16_t check_key();
uint16_t sign_extend(uint16_t x, int bit_count);
uint16_t swap16(uint16_t x);

#endif
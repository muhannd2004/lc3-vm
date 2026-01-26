#include "lc3.h"
#include "memory.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

uint16_t memory[MEMORY_MAX];

void mem_write(uint16_t address, uint16_t val){
    memory[address] = val;
}

uint16_t mem_read(uint16_t address){

    if (address == MR_KBSR)
    {
      if (check_key())
      {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
      }else{
            memory[MR_KBSR] = 0;
      }
      
    }
    
    return memory[address];
}

uint16_t check_key(){
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

uint16_t sign_extend(uint16_t x, int bit_count){

    if (x & ( 1u << (bit_count - 1)))
    {   
        x |= (0xFFFF << bit_count);
    }

    return x;
}

uint16_t swap16(uint16_t x){
    return (x << 8 | x >> 8);
}

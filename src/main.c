#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#include "lc3.h"
#include "memory.h"
#include "input.h"
#include "trap.h"

uint16_t reg[R_COUNT];


int read_image(const char* image_path);
void read_image_file(FILE *file);

void update_flags(uint16_t r);

void add_operation(uint16_t instr);
void and_operation(uint16_t instr);
void not_operation(uint16_t instr);
void ldi_operation(uint16_t instr);
void branch_operation(uint16_t instr);
void jmp_operation(uint16_t instr);
void jsr_operation(uint16_t instr);
void ld_operation(uint16_t instr);
void ldr_operation(uint16_t instr);
void lea_operation(uint16_t instr);
void store_operation(uint16_t instr);
void store_indirect_operation(uint16_t instr);
void store_reg_operation(uint16_t instr);

void main(int argc, const char* argv[]){

    /* argument handling */
    if(argc < 2){
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    for (int i = 0; i < argc; i++)
    {
        if (!read_image(argv[i]))
        {
            printf("failed to read image");
            exit(1);
        }
        
    }

    /* setup */
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    /* 0x3000 is the deafult starting postions */
    enum {PC_START = 0x3000};

    reg[R_PC] = PC_START;

    int running = 1;

    while (running)
    {
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >> 12;

              switch (op)
        {
            case OP_ADD:
                add_operation(instr);
                break;
            case OP_AND:
                and_operation(instr);
                break;
            case OP_NOT:
                not_operation(instr);
                break;
            case OP_BR:
                branch_operation(instr);
                break;
            case OP_JMP:
                jmp_operation(instr);
                break;
            case OP_JSR:
                jsr_operation(instr);
                break;
            case OP_LD:
                ld_operation(instr);
                break;
            case OP_LDI:
                ldi_operation(instr);
                break;
            case OP_LDR:
                ldr_operation(instr);
                break;
            case OP_LEA:
                lea_operation(instr);
                break;
            case OP_ST:
                store_operation(instr);
                break;
            case OP_STI:
                store_indirect_operation(instr);
                break;
            case OP_STR:
                store_reg_operation(instr);
                break;
            case OP_TRAP:
                trap_handler(instr, &running);
                break;
            case OP_RES:
            case OP_RTI:
            default:
                abort();
                break;
        }
    }
    
    restore_input_buffering();
}


int read_image(const char* image_path){

    FILE *file = fopen(image_path, "rb");
    if (!file) return 0;

    read_image_file(file);
    fclose(file);
    return 1;
}

void read_image_file(FILE *file){

    uint16_t origin;
    fread(&origin, sizeof(uint16_t), 1, file);
    origin =swap16(origin);

    uint16_t max_read = MEMORY_MAX - origin;
    uint16_t *p = memory + origin;

    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    while (read-- > 0)
    {
        *p = swap16(*p);
        p++;
    }
    
}

void update_flags(uint16_t r){
    if (reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15) /* a 1 in the left-most bit indicates negative */
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

void add_operation(uint16_t instr){

    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t rs1 = (instr >> 6) & 0x7;

    if ((instr >> 5) & 0x1)
    {
        reg[rd] = reg[rs1] + sign_extend(instr & 0x1F, 5);
    }else{
        uint16_t i_rs2 = instr & 0x7;
        reg[rd] = reg[rs1] + reg[i_rs2];
    }

    update_flags(rd);
    
}

void and_operation(uint16_t instr){
    
    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t rs1 = (instr >> 6) & 0x7;

    if ((instr >> 5) & 0x1)
    {
        reg[rd] = reg[rs1] & sign_extend(instr & 0x1F, 5);
    }else{
        uint16_t i_rs2 = instr & 0x7;
        reg[rd] = reg[rs1] & reg[i_rs2];
    }

    update_flags(rd);
}

void not_operation(uint16_t instr){

    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t rs1 = (instr >> 6) & 0x7;

    reg[rd] = ~reg[rs1];

    update_flags(rd);
}

void ldi_operation(uint16_t instr){

    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t offset = sign_extend(instr & 0x1FF, 9);
    
    reg[rd] = mem_read(mem_read(reg[R_PC] + offset));

    update_flags(rd);
}

void branch_operation(uint16_t instr){

    uint16_t cond = (instr >> 9) & 0x7; 
    
    if (cond & reg[R_COND])
    {
        uint16_t offset = sign_extend(instr & 0x1FF, 9); 
        reg[R_PC] += offset;
    }
        
}

void jmp_operation(uint16_t instr){

    uint16_t base_r = (instr >> 6) & 0x7;

    reg[R_PC] = reg[base_r];
}

void jsr_operation(uint16_t instr){

    reg[R_R7] = reg[R_PC];

    if ((instr >> 11) & 0x1)
    {
        uint16_t offset = sign_extend(instr & 0x7FF, 11);
        reg[R_PC] += offset;
    }else{
        reg[R_PC] = reg[(instr >> 6) & 0x7];
    }
    
}

void ld_operation(uint16_t instr){

    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t address = reg[R_PC] + sign_extend(instr & 0x1FF, 9);

    reg[rd] = mem_read(address);

    update_flags(rd);
}

void ldr_operation(uint16_t instr){

    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t base_r = (instr >> 6) & 0x7;
    
    uint16_t address = reg[base_r] + sign_extend(instr & 0x3F, 6);

    reg[rd] = mem_read(address);

    update_flags(rd);
}

void lea_operation(uint16_t instr){
    
    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t address = reg[R_PC] + sign_extend(instr & 0x1FF, 9);

    reg[rd] = address;

    update_flags(rd);

}

void store_operation(uint16_t instr){

    uint16_t rd = (instr >> 9) & 0x7;

    uint16_t address = reg[R_PC] + sign_extend(instr & 0x1FF, 9);

    mem_write(address, reg[rd]);

}

void store_indirect_operation(uint16_t instr){

    uint16_t rd = (instr >> 9) & 0x7;

    uint16_t ind_address = reg[R_PC] + sign_extend(instr & 0x1FF, 9);
    uint16_t address = mem_read(ind_address);

    mem_write(address, reg[rd]);
}

void store_reg_operation(uint16_t instr){
    
    uint16_t rd = (instr >> 9) & 0x7;
    uint16_t baser_r = (instr >> 6) & 0x7;

    uint16_t address = reg[baser_r] + sign_extend(instr & 0x3F, 6);

    mem_write(address, reg[rd]);
}

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>

#include "lc3.h"

uint16_t memory[MEMORY_MAX];

uint16_t reg[R_COUNT];

struct termios original_tio;

void restore_input_buffering();
void disable_input_buffering();
void handle_interrupt(int signal);
int read_image(const char* image_path);

int main(int argc, const char* argv[]){

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


    
    return 0;
}

uint16_t swap16(uint16_t x){
    return (x << 8 | x >> 8);
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


void disable_input_buffering(){
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;

    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering(){
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}


void handle_interrupt(int signal){
    restore_input_buffering();
    printf("\n");
    exit(-2);
}
#ifndef INPUT_H
#define INPUT_H

#include <termios.h>

extern struct termios original_tio;

void restore_input_buffering();
void disable_input_buffering();
void handle_interrupt(int signal);


#endif
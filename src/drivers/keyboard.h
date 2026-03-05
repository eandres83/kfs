#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <utils.h>
#include "io.h"
#include "vga.h"
#include "arch/i386/idt.h"

#define SHIFT_KEY	-1
#define CTRL_KEY	-2
#define CAPSLOCK_KEY	-3
#define LEFT_ARROW_KEY	-10
#define RIGHT_ARROW_KEY	-11

void	keyboard_callback(registers_t *regs);
void	init_keyboard();

#endif

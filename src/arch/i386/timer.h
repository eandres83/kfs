#ifndef TIMER_H
#define TIMER_H

#include <utils.h>
#include "arch/i386/idt.h"
#include "task/task.h"

void 	init_timer(uint32_t hz_value);
void timer_callback(registers_t *regs);

#endif

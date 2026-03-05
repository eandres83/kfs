#ifndef TIMER_H
#define TIMER_H

#include <utils.h>
#include "arch/i386/idt.h"

void 	init_timer(uint32_t hz_value);

#endif

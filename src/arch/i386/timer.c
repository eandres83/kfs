#include "arch/i386/timer.h"

uint32_t tick = 0;

static void timer_callback(registers_t *regs)
{
	(void)regs;
	tick++;
	terminal_writestring("Tick: ");
	kputnbr(tick, 10);
	terminal_putchar('\n');
}

void	init_timer(uint32_t hz_value)
{
	register_interrupt_handler(32, &timer_callback);

	uint32_t frequency = 1193182 / hz_value;

	outb(0x43, 0x36);

	uint8_t lobyte = (uint8_t)frequency & 0xFF;
	uint8_t hibyte = (uint8_t)(frequency >> 8) & 0xFF;

	outb(0x40, lobyte);
	outb(0x40, hibyte);
}


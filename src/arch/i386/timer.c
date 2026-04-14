#include "timer.h"
#include "task/task.h"

uint32_t tick = 0;

void timer_callback(registers_t *regs)
{
	(void)regs;
	tick++;

	yield();
}

void	init_timer(uint32_t hz_value)
{
	register_interrupt_handler(32, &timer_callback);

	// calculate the hardware divisor (PIT base frequency / desired frequency)
	uint32_t frequency = 1193182 / hz_value;

	// send 0x36: Channel 0, lobyte/hibyte access, square wave mode
	outb(0x43, 0x36);

	// the divisor is 16 bits, so we split it and send it as two 8-bit packets
	uint8_t lobyte = (uint8_t)frequency & 0xFF;
	uint8_t hibyte = (uint8_t)(frequency >> 8) & 0xFF;

	outb(0x40, lobyte);
	outb(0x40, hibyte);
}


#include "ide.h"

// IMPORTANTE: TO DO
// Implementar escane PIC, si no posible fallo al intentar utilizar
// este kernel en otros dispositivos que no sea qemu

static void	ide_wait()
{
//	kprintf("Status: 0x%x\n", inb(0x1F7));
	uint8_t data = inb(0x1F7);
	while ((data & 0x80) != 0 || (data & 0x40) == 0)
		data = inb(0x1F7);
}

// lba28 -> logical block addressing for data sector on a storage device
void	ide_read_sector(uint32_t lba, uint8_t *buffer)
{
	// 0xE0 for the master
	outb(0x1F6, 0xE0 | (lba >> 24 & 0x0F));

	ide_wait();

	outb(0x1F2, 0x01); // cuantos sectore leer
	// que sector leer
	outb(0x1F3, lba);
	outb(0x1F4, lba >> 8);
	outb(0x1F5, lba >> 16);

	// read sectors command
	outb(0x1F7, 0x20);

	while ((inb(0x1F7) & 0x80) != 0);
	while ((inb(0x1F7) & 0x08) == 0);

	uint16_t *ptr = (uint16_t*)buffer;
	for (int i = 0; i < 256; i++)
		ptr[i] = inw(0x1F0);
}

void 	ide_write_sector(uint32_t lba, uint8_t *buffer)
{
	outb(0x1F6, 0xE0 | (lba >> 24 & 0x0F));

	ide_wait();

	outb(0x1F2, 0x01);

	outb(0x1F3, lba);
	outb(0x1F4, lba >> 8);
	outb(0x1F5, lba >> 16);

	outb(0x1F7, 0x30);

	while ((inb(0x1F7) & 0x80) != 0);
	while ((inb(0x1F7) & 0x08) == 0);

	uint16_t *data = (uint16_t*)buffer;
	for (int i = 0; i < 256; i++)
		outw(0x1F0, data[i]);

	outb(0x1F7, 0xE7);
	while ((inb(0x1F7) & 0x80) != 0);
}


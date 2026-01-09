#include "drivers/vga.h"
#include <utils.h>
#include "drivers/io.h"

size_t		terminal_row;
size_t		terminal_column;
uint8_t		terminal_color;
uint16_t	*terminal_buffer = (uint16_t*)VGA_MEMORY;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

static void	terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

static void	update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	// 1. send Low Byte
	outb(0x3D4, 0x0F); // VGA registro 15
	outb(0x3D5, (uint8_t)(pos & 0xFF));

	// 2. send High Byte
	outb(0x3D4, 0x0E); // VGA registro 14
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void	terminal_scroll(void)
{
	kmemcpy(terminal_buffer, terminal_buffer + VGA_WIDTH, (VGA_HEIGHT - 1)
	* VGA_WIDTH * 2); // kmemcpy 1 byte, terminal_buffer 2 bytes

	size_t	start_index = (VGA_HEIGHT - 1) * VGA_WIDTH;

	for (size_t x = 0; x < VGA_WIDTH; x++)
		terminal_buffer[start_index + x] = vga_entry(' ', terminal_color);

	terminal_row = VGA_HEIGHT - 1;
}

void	terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	update_cursor(0, 0);
}

void	terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void	terminal_putchar(char c)
{
	if (c == '\n')
	{
		terminal_column = 0;
		terminal_row++;
	}
	else
	{
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH)
		{
			terminal_column = 0;
			terminal_row++;
			kprintf("Numero de column %d\n", terminal_column);
		}
	}

	if (terminal_row == VGA_HEIGHT)
		terminal_scroll();

	update_cursor(terminal_column, terminal_row);
}

void	terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void	terminal_writestring(const char *data)
{
	terminal_write(data, kstrlen(data));
}


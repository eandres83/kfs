#ifndef IO_H
#define IO_H

#include <utils.h>

// Envia una palabra entero, 16 bits de golpe no 8 bits
static inline void outw(uint16_t port, uint16_t val)
{
	asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

// Envia el byte (val) a un puerto de hardware (port)
static inline void outb(uint16_t port, uint8_t val)
{
	/* "a"(val) -> Carga la variable 'val' en el registro AL
	   "Nd"(port) -> Carga 'port' en el registro DX
	   "outb %0, %1" -> Ejecuta la instruccion de ensamblador
	*/
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Leer datos del puerto
static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	// "=a"(ret): Queremos que el resultado se guarde en 'ret'
	// "inb %1, %0": Lee del puerto %1 y guardalo en %0
	asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return (ret);
}

static inline uint16_t inw(uint16_t port)
{
	uint16_t res;
	asm volatile ("inw %1, %0" : "=a" (res) : "Nd" (port));
	return (res);
}

#endif

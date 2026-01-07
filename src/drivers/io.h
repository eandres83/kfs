#ifndef IO_H
#define IO_H

#include <utils.h>


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

#endif

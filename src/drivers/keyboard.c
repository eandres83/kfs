#include <stdint.h>
#include "io.h"
#include "keyboard.h"

int	shift_status = 0;

static unsigned char kbdus[128] =
{
	0,
	0,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'-',
	'=',
	'\b',		// backspace
	'\t',		// tabulation
	'q',
	'w',
	'e',
	'r',
	't',
	'y',
	'u',
	'i',
	'o',
	'p',
	'[',
	']',
	'\n',		// enter
	CTRL_KEY,	// control
	'a',
	's',
	'd',
	'f',
	'g',
	'h',
	'j',
	'k',
	'l',
	';',
	'\'',
	'`',
	SHIFT_KEY,	// left shift
	'\\',
	'z',
	'x',
	'c',
	'v',
	'b',
	'n',
	'm',
	',',
	'.',
	'/',
	SHIFT_KEY,	// right shift
	'*',
	0,		// alt
	' ',		// space
	CAPSLOCK_KEY,	// caps lock
	0,		// F1
	0,		// F2
	0,		// F3
	0,		// F4
	0,		// F5
	0,		// F6
	0,		// F7
	0,		// F8
	0,		// F9
	0,		// F10
	0,		// num lock
	0,		// scroll lock
	0,		// HOME
	0,		// up arrow
	0,		// PAGEUP
	'-',
	LEFT_ARROW_KEY,	// left arrow
	0,
	RIGHT_ARROW_KEY,// right arrow
	'+',
	0,		// END
	0,		// down aroow	
	0,		// PAGEDOWN
	0,		// INSERT
	0,		// DEL
	0,
	0,
	0,
	0,		// F11
	0,		// F12
	0		// undefined keys
};

static const char shift_kbdus[128] = 
{
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A',
	'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X',
	'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
	'0', '.', '6', 0, 0, 0, 0, 0
};

char	keyboard_read_char()
{
	// Leer el estado del puerto, si el primer bit es un 1 significa que hay info.
	uint8_t status = inb(0x64);

	if (status & 0x01)
	{
		uint8_t data = inb(0x60);

		// 0x2A = Left shift Press, 0x36 = Right shift Prees
		if (data == 0x2A || data == 0x36)
		{
			shift_status = 1;
			return (0);
		}

		// 0xAA = Left shift release, 0xB6 = Right shift release
		if (data == 0xAA || data == 0xB6)
		{
			shift_status = 0;
			return (0);
		}

		// Comprobar si esta el bit 7 encendido
		if (data & 0x80)
			return (0);

		if (!shift_status)
			return (kbdus[data]);
		else
			return (shift_kbdus[data]);
	}
	return (0);
}


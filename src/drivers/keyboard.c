#include "keyboard.h"
#include "modules/events.h"

static int	shift_status = 0;

static const unsigned char kbdfr[128] =
{
	0, 27, '&', 0x82, '"', '\'', '(', '-', 0x8A, '_', 0x87, 0x85, ')', '=', '\b',
	'\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',
	CTRL_KEY, 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 0x97, 0xFD,
	SHIFT_KEY, '*', 'w', 'x',
	'c', 'v', 'b', 'n', ',', ';', ':', '!', SHIFT_KEY, '*', 0, ' ', CAPSLOCK_KEY,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	0,
	0,
	'-',
	LEFT_ARROW_KEY,
	0,
	RIGHT_ARROW_KEY,
	'+',
	0,
	0,
	0,
	0,
	0,
	0, 0, 0,
	0,
	0,
	0
};

static const unsigned char shift_kbdfr[128] =
{
	0, 27,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0xF8, '+', '\b',
	'\t', 'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 0xF9, 0x9C, '\n',
	0,
	'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', '%', 0xE6,
	0,
	'/', 'W', 'X', 'C', 'V', 'B', 'N', '?', '.', '/', 0x15,
	0,
	'*', 0,
	' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0
};

static const unsigned char kbdus[128] =
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

static const unsigned char shift_kbdus[128] = 
{
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A',
	'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X',
	'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',
	'0', '.', '6', 0, 0, 0, 0, 0
};

static const unsigned char *current_layout = kbdus;
static const unsigned char *current_shift_layout = shift_kbdus;

// Function to ensure that the shift_status variable is 0 and there is no garbage in the buffer
void	init_keyboard()
{
	shift_status = 0;

	register_interrupt_handler(33, &keyboard_callback);
}

void	keyboard_callback(registers_t *regs)
{
	(void)regs;
	uint8_t data = inb(0x60);

	// 0x2A = Left shift Press, 0x36 = Right shift Prees
	if (data == 0x2A || data == 0x36)
	{
		shift_status = 1;
		return ;
	}

	// 0xAA = Left shift release, 0xB6 = Right shift release
	if (data == 0xAA || data == 0xB6)
	{
		shift_status = 0;
		return ;
	}

	// Check if bit 7 is set, release the key
	if (data & 0x80)
		return ;

	if (!shift_status)
		tty_push_char(current_layout[data]);
	else
		tty_push_char(current_shift_layout[data]);

	execute_callback(EVENT_KEYBOARD, (void*)&data);
}

void	set_keyboard_layout(char *str)
{
	if (kstrcmp("fr", str) == 0)
	{
		current_layout = kbdfr;
		current_shift_layout = shift_kbdfr;
	}
	if (kstrcmp("us", str) == 0)
	{
		current_layout = kbdus;
		current_shift_layout = shift_kbdus;
	}
}


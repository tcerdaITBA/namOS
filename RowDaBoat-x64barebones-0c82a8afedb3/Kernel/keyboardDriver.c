#include "keyboardDriver.h"

#include "videoDriver.h"

#define BUFFSIZE 128

char get_key();

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

static int len = sizeof(kbdus);
static char buffer[BUFFSIZE];
static unsigned int ret_index = 0;
static unsigned int store_index = 0;
static int buff_size = 0;

void store_scancode() {
  char k = get_key();
  if (k < 128 && buff_size != BUFFSIZE) {  // buffer no lleno
    buff_size++;
    buffer[store_index++] = k;  // guarda el scancode de la tecla
	if (store_index == BUFFSIZE)
    store_index = 0;
  }
}

int get_char() {
  while (buff_size == 0)
	;
  char k = buffer[ret_index++];
  buff_size--;
  if (ret_index == BUFFSIZE)
    ret_index = 0;
  if (k < len && kbdus[k] != 0)
    return kbdus[k];
  return -1;
}

#include <stdint.h>
#include "syscalls.h"
#include "stdio.h"

extern char bss;
extern char endOfBinary;

void * memset(void * destiny, int32_t c, uint64_t length);

int main() {
	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);

	char str[] = "HOLA DESDE USERLAND";
	int len = sizeof(str);

	write(STDOUT, str, len);
	write(1, str, len);
	write(2, str, len);

	return 0;
}

void * memset(void * destiation, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destiation;

	while(length--)
		dst[length] = chr;

	return destiation;
}

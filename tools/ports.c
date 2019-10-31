#include "../include/ports.h"



void outb(unsigned short port, unsigned char value){
	__asm volatile("outb %1, %0"::"dN"(port), "a"(value));
}

void outw(unsigned short port, unsigned short value){
	__asm volatile("outw %1, %0"::"dN"(port), "a"(value));
}

unsigned char inb(unsigned short port){
	unsigned char ret;
	__asm volatile("inb %1, %0":"=a"(ret):"dN"(port));
	return ret;
}

unsigned short inw(unsigned short port){
	unsigned short ret;
	__asm volatile("inw %1, %0":"=a"(ret):"dN"(port));
	return ret;
}
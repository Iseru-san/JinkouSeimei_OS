#pragma once



void outb(unsigned short port, unsigned char value);
void outw(unsigned short port, unsigned short value);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);
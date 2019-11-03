#include "../include/ports.h"
#include "../include/stdarg.h"



static short *video_address = (short *)0x000B8000; // ����� �����������
static short width = 80; // ������ ��������� � ��������
static short size = 2000; // ������ (������� �������� ���������� �� ������)
static short position = 0; // ������� �������
static short color = 0x0F00; // ���� ���� � ������� ����������� �� ������
static char tab_width = 4; // ������ ���������



// ������� ������
void terminal_clear(){
	short i = size;
	while(i--)
		video_address[i] = color | ' ';
}

// ������������� ���� ���������� �������� � ���� �� ����
void terminal_set_color(short color_background, short color_symbols){
	color = color_background;
	color <<= 4;
	color |= color_symbols;
	color <<= 8;
}



// ������������� ����� ������� �������
static void set_cursor_position(short new_position){
	position = new_position;
	outb(0x03D4, 14);
	outb(0x03D5, position >> 8);
	outb(0x03D4, 15);
	outb(0x03D5, position);
}

// ��������� ������ �� ���� ������ �����
static void move_up(){
	short i;
	short a = 0;
	short b = width;
	i = size - width;
	// ������� ����� ������������ �� ���� ������ �����, ������� ������ ������
	while(i--)
		video_address[a++] = video_address[b++];
	i = width;
	// � ����� ��������� ������ ����������� ���������
	while(i--)
		video_address[a++] = color | ' ';
	set_cursor_position(size - width);
}

// ����� ����������� ����� ������ �� �����
static void primitive_print(const char *s){
	while(*s){
		video_address[position] = color | *s;
		position++;
		s++;
	}
}

// ��������� ������� ����� �� ������
static void simple_print(const char *s){
	while(*s){
		if(position >= size)
			move_up();
		if(*s == '\n'){
			int n = width - position % width;
			while(n--)
				video_address[position++] = color | ' ';
			s++;
			continue;
		}
		if(*s == '\t'){
			int t = tab_width - position % tab_width;
			while(t--)
				video_address[position++] = color | ' ';
			s++;
			continue;
		}
		video_address[position] = color | *s;
		position++;
		s++;
	}
}

// �������� ����� �� ������������� ����� ��� �����
static void print_integer(unsigned int n, unsigned int base){
	char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
	char buffer[65];
	short i = sizeof(buffer) - 1;
	if(base >= sizeof(alphabet) - 1)
		return;
	buffer[i] = '\0';
	do{
		buffer[--i] = alphabet[n % base];
		n = n / base;
	}while(n && i);
	primitive_print(&buffer[i]);
}



// ��������������� ����� ������ �� �����
void print(const char *formated_string, ...){
	char *s = (char *)formated_string;
	int buffer_int;
	va_list ap;
	va_start(ap, formated_string);

	while(*s){
		if(position >= size)
			move_up();
		if(*s == '\n'){
			int n = width - position % width;
			while(n--)
				video_address[position++] = color | ' ';
			s++;
			continue;
		}
		if(*s == '\t'){
			int t = tab_width - position % tab_width;
			while(t--)
				video_address[position++] = color | ' ';
			s++;
			continue;
		}
		// ��������� ��������� � ������
		if(*s == '%'){
			s++;
			// ���� ���� ���������� ��������
			switch(*s){
				case 'd': // �������� ����� �����
					buffer_int = va_arg(ap, int);
					if(buffer_int < 0){
						video_address[position++] = color | '-';
						buffer_int = -buffer_int;
					}
					print_integer(buffer_int, 10);
					break;
				case '%': // ������ "�������"
					video_address[position++] = color | *s;
					break;
				case 's': // ������� ������
					simple_print(va_arg(ap, char*));
					break;
				case 'c': // ������ ������
					video_address[position] = color | (char)va_arg(ap, int);
					position++;
					break;
				case 'h': // ����������������� �����
					print_integer(va_arg(ap, int), 16);
					break;
				case 'u': // ����������� ����� ���������� �����
					print_integer(va_arg(ap, int), 10);
					break;
				case 'o': // ������������ �����
					print_integer(va_arg(ap, int), 8);
					break;
				case 'b': // �������� �����
					print_integer(va_arg(ap, int), 2);
					break;
			}
			s++;
			continue;
		}
		video_address[position] = color | *s;
		position++;
		s++;
	}
	set_cursor_position(position);
	va_end(ap);
}
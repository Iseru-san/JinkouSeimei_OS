#include "../include/ports.h"
#include "../include/stdarg.h"



static short *video_address = (short *)0x000B8000; // Адрес видеопамяти
static short width = 80; // Ширина терминала в символах
static short size = 2000; // Размер (сколько символов поместится на экране)
static short position = 0; // Позиция курсора
static short color = 0x0F00; // Цвет фона и символа печатаемого на экране
static char tab_width = 4; // Размер табуляции



// Очистка экрана
void terminal_clear(){
	short i = size;
	while(i--)
		video_address[i] = color | ' ';
}

// Устанавливает цвет печатаемых символов и цвет их фона
void terminal_set_color(short color_background, short color_symbols){
	color = color_background;
	color <<= 4;
	color |= color_symbols;
	color <<= 8;
}



// Устанавливает новую позицию курсора
static void set_cursor_position(short new_position){
	position = new_position;
	outb(0x03D4, 14);
	outb(0x03D5, position >> 8);
	outb(0x03D4, 15);
	outb(0x03D5, position);
}

// Прокрутка текста на одну строку вверх
static void move_up(){
	short i;
	short a = 0;
	short b = width;
	i = size - width;
	// Сначала текст перемещается на одну строку вверх, затирая первую строку
	while(i--)
		video_address[a++] = video_address[b++];
	i = width;
	// А затем последняя строка заполняется пробелами
	while(i--)
		video_address[a++] = color | ' ';
	set_cursor_position(size - width);
}

// Самый примитивный вывод текста на экран
static void primitive_print(const char *s){
	while(*s){
		video_address[position] = color | *s;
		position++;
		s++;
	}
}

// Печататет простой текст на экране
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

// Печатает целое не отрицательное число как текст
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



// Форматированный вывод текста на экран
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
		// Обработка параметра в строке
		if(*s == '%'){
			s++;
			// Если надо напечатать параметр
			switch(*s){
				case 'd': // Знаковое целое число
					buffer_int = va_arg(ap, int);
					if(buffer_int < 0){
						video_address[position++] = color | '-';
						buffer_int = -buffer_int;
					}
					print_integer(buffer_int, 10);
					break;
				case '%': // Символ "процент"
					video_address[position++] = color | *s;
					break;
				case 's': // Простая строка
					simple_print(va_arg(ap, char*));
					break;
				case 'c': // Просто символ
					video_address[position] = color | (char)va_arg(ap, int);
					position++;
					break;
				case 'h': // Шестнадцатиричное число
					print_integer(va_arg(ap, int), 16);
					break;
				case 'u': // Беззнаковое целое десятичное число
					print_integer(va_arg(ap, int), 10);
					break;
				case 'o': // Восьмиричное число
					print_integer(va_arg(ap, int), 8);
					break;
				case 'b': // Двоичное число
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
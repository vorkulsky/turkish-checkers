#define byte unsigned char
#define Dmy 1
// com.asm
	extern "C" Out_Chr();
	extern "C" Get_Chr();
	extern "C" Ser_Ini();
	extern "C" Ser_Rst();
// int9h.asm
	extern "C" Key_Ini();
	extern "C" Key_Rst();
	extern "C" Key_Is_Esc();
// mouse.asm
	extern "C" Mouse_Ini();
	extern "C" Mouse_Rst();
	extern "C" Mouse_Hide();
	extern "C" Mouse_Show();
// timer.asm
	extern "C" Timer_Ini();
	extern "C" Timer_Rst();

void graph_ini();
void graph_rst();
void rectangle(int x1, int x2, int y1, int y2, byte color);
void paint_empty_board();
void debug_ini();
void debug_print(char* str, int size, byte color);
void mainloop();
void send_str(char* str, int size);
void connect();
void timer_ini();
void read_all();

enum connect_status {START, C0PING};

byte Page;
byte timer18 = 0;
byte timer55 = 0;
connect_status cs = START;

void main() {
	Key_Ini();
	graph_ini();
	Mouse_Ini();
	paint_empty_board();
	debug_ini();
	Ser_Ini();
	timer_ini();

	mainloop();

	Timer_Rst();
	Ser_Rst();
	Mouse_Rst();
	graph_rst();
	Key_Rst();
}

void mainloop() {
	while(1) {
		Key_Is_Esc();
		asm jc end_mainloop
		//debug_print(&timer55, 2, Dmy);
		connect();
	}
end_mainloop:
}

void graph_ini() {
	// Переход в 10h графический режим
	asm {
		mov ah, 00h
		mov al, 10h
		int 10h
		mov ah, 0fh
		int 10h
		mov Page, bh
	}
}

void graph_rst() {
	// Возвращение стандартного видео-режима	
	asm {
		mov ah, 00h
		mov al, 3h
		int 10h
	}
}

void rectangle(int x1, int x2, int y1, int y2, byte color) {
	for (int i=y1; i<=y2; i++) {
		for (int j=x1; j<=x2; j++) {
			asm {
				mov ah, 0ch
				mov al, color
				mov cx, j
				mov dx, i
				mov bh, Page
				int 10h
			}
		
		}
	}
}

void paint_empty_board() {
	Mouse_Hide();
	byte color = 6;

	int x1, x2, y1, y2;
	// Расчерчиваем по вертикали
	y1 = 0;
	y2 = 345;
	x1 = 0;
	x2 = x1 + 1;
	for (int i=0; i<=8; i++) {
		 rectangle(x1, x2, y1, y2, color);
		 x1 += 43;
		 x2 += 43;
	}

	// Расчерчиваем по горизонтали
	y1 = 0;
	y2 = y1 + 1;
	x1 = 0;
	x2 = 345;
	for (int j=0; j<=8; j++) {
		 rectangle(x1, x2, y1, y2, color);
		 y1 += 43;
		 y2 += 43;
	}

	Mouse_Show();
}

void debug_ini() {
	rectangle(386, 389, 0, 349, 8);
}

void debug_print_line(char* line, int size, byte color) {
	// Максимальная длина строки 29 символов
	const byte left = 50;
	const byte right = 79;
	const byte up = 0;
	const byte down = 24;
	const byte writex = 50;
	const byte writey = 24;
	asm {
		push ax; push bx; push cx; push dx; push sp; push bp; push si; push di; push es
	// Прокрутка на одну строку вверх перед печатью очередной
		mov ah, 6
		mov al, 1
		mov bx, 0000h
		mov ch, up
		mov cl, left
		mov dh, down
		mov dl, right
		int 10h
	// Печать линии
		mov ah, 13h
		mov al, 0
		mov bh, Page
		mov bl, color
		mov cx, size
		mov dl, writex
		mov dh, writey
		push cs
		pop es
		mov bp, line
		int 10h
		pop es; pop di; pop si; pop bp; pop sp; pop dx; pop cx; pop bx; pop ax
	}
}

void debug_print(char* str, int size, byte color) {
	// 29 - максимальное кличество символов, печатаемое на одной строке
	while (size > 0) {
		int part = size <= 29 ? size : 29;
		debug_print_line(str, part, color);
		str += part;
		size -= part;
	}
}

void send_str(char* str, int size) {
	char* str_end = str + size;
	byte b;
	while (str < str_end) {
		b = *str;
		asm mov al, b
send_via_com:
		Out_Chr();
		asm jc send_via_com
		str++;
	}
}

void timer_ini() {
	byte* timer18_ptr = &timer18;
	byte* timer55_ptr = &timer55;
	asm {
		push ax; push bx
		mov ax, timer18_ptr
		mov bx, timer55_ptr
	}
	Timer_Ini();
	asm {
		push bx; push ax
	}
}

void connect() {
	switch (cs) {
		case START: {
			timer18 = 0;
			cs = C0PING;
			debug_print("Start", 5, 8);
			break;
		}
		case C0PING: {
			if (timer18 >= 18) {
				timer18 = 0;
				send_str("C0", 2);
				debug_print("C0", 2, Dmy);
				break;
			}
		}
		default: {}
	}
}
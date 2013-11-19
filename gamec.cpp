#define byte unsigned char
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

// Грфика
void graph_ini();
void graph_rst();
void rectangle(int x1, int x2, int y1, int y2, byte color);
void paint_empty_board();

void mainloop();

void main() {
	Key_Ini();
	Ser_Ini();
	graph_ini();
	Mouse_Ini();
	paint_empty_board();

	mainloop();

	Mouse_Rst();
	graph_rst();
	Ser_Rst();
	Key_Rst();
}

void mainloop() {
	while(1) {
		Key_Is_Esc();
		asm jc end_mainloop
	}
end_mainloop:
}

// Грфика

byte Page;

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
				mov cx, i
				mov dx, j
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
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

void graph_ini();
void graph_rst();
void rectangle(int x1, int x2, int y1, int y2, byte color);
void paint_empty_board();
void debug_ini();
void debug_print(char* str, int size, byte color);
void mainloop();
void send_via_com();
byte* prepare_place_for_send(int size);
void copy_str_for_send(char* str, int size, byte* SB_pos);

byte Page;
const int SB_size = 1024;
byte Send_Buffer[SB_size];
byte* SB_head = Send_Buffer; // Указывает на начало еще не отправленной части.
byte* SB_mark = Send_Buffer; // Указывает на элемент за концом готовой для отправки части.
byte* SB_tail = Send_Buffer; // Указывает на элемент на элемент, с которого можно начинать писать.
								// Перед началом записи сдвигается на столько символов, сколько нужно записать за раз.
const byte* SB_end = Send_Buffer + SB_size; // Элемент за концом места под буфер

void main() {
	Key_Ini();
	Ser_Ini();
	graph_ini();
	Mouse_Ini();
	paint_empty_board();
	debug_ini();
	debug_print("Start", 5, 8);

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
	// Прокрутка на одну строку вверх перед печатью очережной
	const byte left = 50;
	const byte right = 79;
	const byte up = 0;
	const byte down = 24;
	const byte writex = 50;
	const byte writey = 24;
	asm {
		push ax
		push bx
		push cx
		push dx
		push sp
		push bp
		push si
		push di
		push es
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
		pop es
		pop di
		pop si
		pop bp
		pop sp
		pop dx
		pop cx
		pop bx
		pop ax
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

byte* prepare_place_for_send(int size) {
	// Резервируем место
	asm cli // Запрет прерываний
	byte* SB_pos = SB_tail; // Позиция для копирования очередного символа
	if (SB_tail + size < SB_end) {
		SB_tail += size;
	} else {
		SB_tail = SB_tail + size - SB_size;
	}
	asm sti // Разрешение прерываний
	return SB_pos;
}

void copy_str_for_send(char* str, int size, byte* SB_pos) {
	// Копируем строку
	int copied_num = 0;
	while (SB_pos < SB_end && copied_num < size) {
		*SB_pos = *(str + copied_num);
		copied_num++;
		SB_pos++;
	}
	if (copied_num != size) {
		SB_pos = Send_Buffer;
		while (copied_num != size) {
			*SB_pos = *(str + copied_num);
			copied_num++;
			SB_pos++;
		}
	}
	SB_mark = SB_tail;
}

void send_str(char* str, int size) {
// Считаем, что буфер никогда не может переполниться
	byte* SB_pos = prepare_place_for_send(size);
	copy_str_for_send(str, size, SB_pos);
	send_via_com();
}

void send_via_com() {
	byte b;
	asm cli // Запрет прерываний
	while (SB_head != SB_end && SB_head != SB_mark) {
		b = *SB_head;
		asm mov al, b
send_via_com_1:
		Out_Chr();
		asm jc send_via_com_1
		SB_head++;
	}
	if (SB_head != SB_mark) {
		SB_head = Send_Buffer;
		while (SB_head != SB_mark) {
			b = *SB_head;
			asm mov al, b
send_via_com_2:
			Out_Chr();
			asm jc send_via_com_2
			SB_head++;
		}
	}
	asm sti // Разрешение прерываний
}
#define byte unsigned char
#define Dmy 1
#define Dhis 2
// com.asm
	extern "C" Out_Chr();
	extern "C" Get_Chr();
	extern "C" Ser_Ini();
	extern "C" Ser_Rst();
// int9h.asm
	extern "C" Key_Ini();
	extern "C" Key_Rst();
	extern "C" Key_Is_Esc();
	extern "C" Get_Key();
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
void debug_print(byte* str, int size, byte color);
void mainloop();
void gameloop();
void send_str(byte* str, int size);
void connect();
void timer_ini();
void read_all();
byte random_gesture();
void game();
void connect_send_automat();
void connect_get_automat(byte c);
void rockPaperScissors(byte c);
void sendSS();
void error();
void skip_chat();
void handle_keyboard();
void ng_automat();
void get_command();
void get_automat(byte c);

enum chip {None, White, Black};
enum connect_send_status {CSSTART, C0PING, CxSEND};
enum connect_get_status {CGSTART, GetC, GetC0, GetC0C, GetC0C0, GetC0C0C, CGEXIT};
enum global_game_status {GGSTART, GGNEW, GGCONNECT, GGGAME};
enum ng_status {NGSTART, NGN, NGD};
enum get_status {GETSTART, GETS, GETERR, GETC, GETNEXT, GETN, GETD, GETF, GETE, GETLM, GETX, GETXX, GETLEN};
enum command {COMC0, COMCx, COMNG, COMD0, COMD2, COMD3, COMFF, COMEX};

byte Page;
byte STR[64];
byte hishod[32];
byte timer18 = 0;
byte timer55 = 0;
connect_send_status css = CSSTART;
connect_get_status cgs = CGSTART;
byte Cx;
byte HisCx;
chip MyColor = None;
global_game_status ggs = GGCONNECT;
byte NG_is_sent = 0;
byte NG_is_received = 0;
ng_status ngs = NGSTART;
get_status getst = GETSTART;
command comm;
byte XLen;
byte templen;
byte isNewCommand = 0;

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
connect:
	MyColor = None;
	while (MyColor == None) {
		Key_Is_Esc();
		asm jc end_mainloop
		connect();
	}
	gameloop();
	if (ggs == GGCONNECT) goto connect;
end_mainloop:
}

void gameloop() {
newgame:
	debug_print("New game", 8, 8);
	ggs = GGSTART;
	timer18 = 0;
	timer55 = 0;
	NG_is_sent = 0;
	NG_is_received = 0;
	ngs = NGSTART;
	getst = GETSTART;
	isNewCommand = 0;

	while (1) {
		Key_Is_Esc();
		asm jc send_EX
		sendSS();
		game();
		if (ggs == GGNEW) goto newgame;
		if (ggs == GGCONNECT) goto end_gameloop;
	}
send_EX:
	send_str("EX", 2);
	debug_print("EX", 2, Dmy);
end_gameloop:
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

void debug_print_line(byte* line, int size, byte color) {
	// Максимальная длина строки 29 символов
	const byte left = 50;
	const byte right = 79;
	const byte up = 0;
	const byte down = 24;
	const byte writex = 50;
	const byte writey = 24;
	Mouse_Hide();
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
	Mouse_Show();
}

void debug_print(byte* str, int size, byte color) {
	// 29 - максимальное кличество символов, печатаемое на одной строке
	while (size > 0) {
		int part = size <= 29 ? size : 29;
		debug_print_line(str, part, color);
		str += part;
		size -= part;
	}
}

void send_str(byte* str, int size) {
	byte* str_end = str + size;
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

byte random_gesture() {
	byte r;
	asm {
		mov ah, 0
		int 1Ah
		mov r, dl
	}
	byte random3 = r % 3;
	return random3;
}

void connect() {
	// Отправка
	connect_send_automat();

	// Получение
	byte c;
	Get_Chr();
	asm jc no_char
	asm mov c, al
	timer55 = 0;
	connect_get_automat(c);

no_char:
	if (timer55 > 18 && cgs != CGSTART) {
		cgs = CGSTART;
		css = CSSTART;
		timer55 = 0;
	}
}

void connect_send_automat() {
	switch (css) {
		case CSSTART: {
			timer18 = 0;
			css = C0PING;
			debug_print("Start", 5, 8);
			send_str("C0", 2);
			debug_print("C0", 2, Dmy);
			break;
		}
		case C0PING: {
			if (timer18 >= 18) {
				timer18 = 0;
				cgs = CGSTART;
				send_str("C0", 2);
				debug_print("C0", 2, Dmy);
			}
			break;
		}
		case CxSEND: {
			STR[0] = 'C';
			STR[1] = Cx + 49;
			send_str("C0", 2);
			debug_print("C0", 2, Dmy);
			send_str(STR, 2);
			debug_print(STR, 2, Dmy);
			// Если через 18 не будет ответного Cx, идем на начало.
			timer18 = 0;
			css = C0PING;
			break;
		}
		default: {}
	}
}

void connect_get_automat(byte c) {
	switch (cgs) {
		case CGSTART: {
			if (c == 'C') {
				cgs = GetC;
			} else {
				css = CSSTART;
			}
			break;
		}
		case GetC: {
			if (c == '0') {
				cgs = GetC0;
				css = CxSEND;
				debug_print("C0", 2, Dhis);
				Cx = random_gesture();
			} else {
				cgs = CGSTART;
				css = CSSTART;
			}
			break;
		}
		case GetC0: {
			if (c == 'C') {
				cgs = GetC0C;
			} else {
				cgs = CGSTART;
				css = CSSTART;
			}
			break;
		}
		case GetC0C: {
			if (c == '0') {
				cgs = GetC0C0;
				debug_print("C0", 2, Dhis);
			} else {
				if (c == '1' || c == '2' || c == '3') {
					rockPaperScissors(c);
				} else {
					cgs = CGSTART;
					css = CSSTART;
				}
			}
			break; 
		}
		case GetC0C0: {
			if (c == 'C') {
				cgs = GetC0C0C;
			} else {
				cgs = CGSTART;
				css = CSSTART;
			}
			break;
		}
		case GetC0C0C: {
			if (c == '0') {
				cgs = GetC0;
				debug_print("C0", 2, Dhis);
			} else {
				if (c == '1' || c == '2' || c == '3') {
					rockPaperScissors(c);
				} else {
					cgs = CGSTART;
					css = CSSTART;
				}
			}
			break; 
		}
		default: {}
	}
}

void rockPaperScissors(byte c) {
	cgs = CGEXIT;
	HisCx = c - 49;
	STR[0] = 'C';
	STR[1] = HisCx + 49;
	debug_print(STR, 2, Dhis);
	if (Cx == HisCx) {
		cgs = CGSTART;
		css = CSSTART;
		MyColor = None;
	} else {
		byte cnb = Cx * 10 + HisCx;
		// Первая цифра - я. У победителя черный.
		if (cnb == 12 || cnb == 23 || cnb == 31) {
			MyColor = Black;
		} else {
			MyColor = White;
		}
	}
}

void sendSS() {
	if (timer18 >= 18) {
		debug_print("SS", 2, Dmy);
		send_str("SS", 2);
		timer18 = 0;
	}
	if (timer55 >= 55) {
		ggs = GGCONNECT;
	}
}

void error() {
	debug_print("D0", 2, Dmy);
	send_str("D0", 2);
	ggs = GGNEW;
}

void skip_chat() {
	byte c = 0;
	while (c != '$') {
		Get_Chr();
		asm jc no_char
		asm mov c, al
no_char:
	}
}

void handle_keyboard() {
	byte key;
	Get_Key();
	asm jnc no_key
	asm mov key, al
	if (key == 0xA2) { //g
		if (NG_is_sent == 0) {
			debug_print("NG", 2, Dmy);
			send_str("NG", 2);
			NG_is_sent = 1;
		}
	}
	no_key:
}

void game() {
	handle_keyboard();

	if (NG_is_received == 0) {
		get_command();
		if (isNewCommand == 1) {
			isNewCommand = 0;
			if (getst != GETERR) {
				if (comm == COMNG) {
					NG_is_received = 1;
				} else if (comm == COMC0) {
					ggs = GGCONNECT;
				}
			} else {
				error();
			}
		}
	}

	if (NG_is_sent == 1 && NG_is_received == 1) {
		if (MyColor == White) MyColor = Black;
		else MyColor = White;
		ggs = GGGAME;
	}

}

void get_command() {
	isNewCommand = 0;

	while (1) {
		byte c;
		Get_Chr();
		asm jc no_char
		asm mov c, al
		debug_print(&c, 1, 8);
		timer55 = 0;
		get_automat(c);
		if (getst == GETSTART || getst == GETERR) {
			isNewCommand = 1;
			goto no_char;
		}
	}
no_char:
}

void get_automat(byte c) {
	if (getst == GETNEXT) getst = GETSTART;

	switch (getst) {
		case GETSTART: {
			switch (c) {
				case 'S': {
					getst = GETS;
					break;
				}
				case 'C': {
					getst = GETC;
					break;
				}
				case 'N': {
					getst = GETN;
					break;
				}
				case 'D': {
					getst = GETD;
					break;
				}
				case 'F': {
					getst = GETF;
					break;
				}
				case 'E': {
					getst = GETE;
					break;
				}
				case 'L':
				case 'M': {
					getst = GETLM;
					break;
				}
				case 'X': {
					getst = GETX;
					break;
				}
				default: {
					getst = GETERR;
				}
			}
			break;
		}
		case GETS: {
			if (c == 'S') {
				debug_print("SS", 2, Dhis);
				getst = GETNEXT;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETC: {
			if (c == '0') {
				debug_print("C0", 2, Dhis);
				comm = COMC0;
				getst = GETSTART;
			} else if (c == '1' || c == '2' || c == '3') {
				STR[0] = 'C';
				STR[1] = c;
				debug_print(STR, 2, Dhis);
				comm = COMCx;
				Cx = c - 49;
				getst = GETSTART;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETN: {
			if (c == 'G') {
				debug_print("NG", 2, Dhis);
				comm = COMNG;
				getst = GETSTART;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETD: {
			switch (c) {
				case '0': {
					debug_print("D0", 2, Dhis);
					comm = COMD0;
					getst = GETSTART;
					break;
				}
				case '2': {
					debug_print("D2", 2, Dhis);
					comm = COMD2;
					getst = GETSTART;
					break;
				}
				case '3': {
					debug_print("D3", 2, Dhis);
					comm = COMD3;
					getst = GETSTART;
					break;
				}
				default: {
					if (c > '3' && c <= '9') {
						STR[0] = 'D';
						STR[1] = c;
						debug_print(STR, 2, Dhis);
						getst = GETNEXT;
					} else {
						getst = GETERR;
					}
				}
			}
			break;
		}
		case GETF: {
			if (c == 'F') {
				debug_print("FF", 2, Dhis);
				comm = COMFF;
				getst = GETSTART;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETE: {
			if (c == 'X') {
				debug_print("EX", 2, Dhis);
				comm = COMEX;
				getst = GETSTART;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETLM: {
			if (c == '$') {
				debug_print("ML$", 3, Dhis);
				getst = GETNEXT;
			}
			break;
		}
		case GETX: {
			if (c == '0' || c == '1') {
				XLen = c - 49;
				getst = GETXX;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETXX: {
			if (c >= '0' && c <= '9') {
				XLen = XLen*10 + (c - 49);
				templen = 0;
				getst = GETLEN;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETLEN: {
			if (templen >= XLen*2) {
				STR[0] = 'X';
				STR[1] = XLen / 10 + 49;
				STR[2] = XLen % 10 + 49;
				for (int i=0; i< XLen*2; i++) {
					STR[i+3] = hishod[i];
				} 
				debug_print(STR, 3+XLen*2, Dhis);
			} else {
				hishod[templen] = c;
				templen++;
			}
			break;
		}
		default: {
			getst = GETERR;
		}
	}
}
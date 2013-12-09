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
	extern "C" Mouse_Clc();
	extern "C" Mouse_Clc_Restart();
// timer.asm
	extern "C" Timer_Ini();
	extern "C" Timer_Rst();

enum chip {None, White, Black, WDamka, BDamka};
enum connect_send_status {CSSTART, C0PING, CxSEND};
enum connect_get_status {CGSTART, GetC, GetC0, GetC0C, GetC0C0, GetC0C0C, CGEXIT};
enum global_game_status {GGSTART, GGNEW, GGCONNECT, GGStartStepWhite, GGStartStepBlack, GGStepWhite, GGStepBlack};
enum get_status {GETSTART, GETS, GETERR, GETC, GETNEXT, GETN, GETD, GETF, GETE, GETLM, GETX, GETXX, GETLEN};
enum command {COMC0, COMCx, COMNG, COMD0, COMD2, COMD3, COMFF, COMEX, COMX};
enum mouse_click_status {MCNone, MCBoard, MCNG, MCD2, MCD3, MCFF};
enum direction {DNone, DLeft, DRight, DUp, DDown};

void graph_ini();
void graph_rst();
void rectangle(int x1, int x2, int y1, int y2, byte color);
void paint_empty_board();
void paint_buttons();
void write_line(byte* line, int size, byte writex, byte writey, byte color);
void debug_ini();
void debug_print(byte* str, int size, byte color);
void debug_print_line(byte* str, int size, byte color);
void debug_print_byte(byte b, byte color);
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
void ng_automat();
void get_command();
void get_automat(byte c);
void apply_color(byte x, byte y);
void apply_select(byte x, byte y, byte select);
mouse_click_status get_click();
byte can_step(byte x, byte y, chip color); // color - цвет ходящего.
byte can_eat(byte x, byte y, chip color);
byte can_move(byte x, byte y, chip color);
byte anyone_can_eat(chip color);
byte damka_can_eat(byte x, byte y, chip color);
void step(chip color);
byte do_step(byte x, byte y, chip color);
byte do_eat(byte x, byte y, chip color);
byte do_move(byte x, byte y, chip color);
byte damka_do_move(byte x, byte y);
void start_step(chip color, chip player);
void start_his_step();
void start_my_step();
void board_init();
byte damka_do_eat(byte x, byte y, chip color);
byte usual_do_eat(byte x, byte y, chip color);
void usual_eat(byte x, byte y);
byte game_over();
void write_hod(byte x, byte y);
void formalize_hod();
void new_game();
void his_step();
void circle(int x1, int y1, int d, byte color);
void point(int x, int y, byte color);
void read_args();

byte Page;
byte STR[64];
byte hishod[35];
byte hishod_len;
byte hishod_part;
byte myhod[35];
byte myhod_len;
byte timer18 = 0;
byte timer55 = 0;
connect_send_status css = CSSTART;
connect_get_status cgs = CGSTART;
byte Cx;
byte HisCx;
chip MyColor = None;
chip HisColor = None;
global_game_status ggs = GGCONNECT;
byte NG_is_sent = 0;
byte NG_is_received = 0;
get_status getst = GETSTART;
command comm;
byte templen;
byte isNewCommand = 0;
chip board[8][8];
int click_x;
int click_y;
byte futureDamka;
byte selected_x;
byte selected_y;
direction forbidden_direction;
mouse_click_status mc;
byte drawn_proposed_by_me;
byte drawn_proposed_by_him;
byte argslen;
byte whetherC0;

void main() {
	Key_Ini();
	graph_ini();	
	paint_empty_board();
	paint_buttons();
	debug_ini();
	read_args();
	Mouse_Ini();
	timer_ini();
	Ser_Ini();

	mainloop();

	Ser_Rst();
	Timer_Rst();
	Mouse_Rst();
	graph_rst();
	Key_Rst();
}

void mainloop() {
connect:
	MyColor = None;
	timer18 = 0;
	whetherC0 = 0;
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
	debug_print("Ожидание начала новой игры", 26, 8);
	ggs = GGSTART;
	timer18 = 0;
	timer55 = 0;
	NG_is_sent = 0;
	NG_is_received = 0;
	getst = GETSTART;
	isNewCommand = 0;
	drawn_proposed_by_me = 0;
	drawn_proposed_by_him = 0;
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
			point(j, i, color);
		}
	}
}

#undef byte
void read_args() {
	asm mov al, byte ptr cs:[80h]
	asm mov argslen, al
	if (argslen) argslen--;
}
#define byte unsigned char

void point(int x, int y, byte color) {
	asm {
		mov ah, 0ch
		mov al, color
		mov cx, x
		mov dx, y
		mov bh, Page
		int 10h
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

void paint_buttons() {
	Mouse_Hide();
	const byte writex = 45;
	const byte writey = 5;
	const byte color = 7;
	const int size = 2;
	byte* line = "NGD2D3FF";
	for (int i=0; i<8; i+=2) {
		const int y_step = 28;
		rectangle(354, 379, 52+y_step*i, 52+y_step*i+2, 8);
		rectangle(354, 379, 102+y_step*i-2, 102+y_step*i, 8);
		rectangle(354, 354+2, 52+y_step*i, 102+y_step*i, 8);
		rectangle(379-2, 379, 52+y_step*i, 102+y_step*i, 8);
		write_line(line+i, size, writex, writey+2*i, color);
	}
	Mouse_Show();
}

void apply_color(byte x, byte y) {
	byte color;
	switch (board[y][x]) {
		case White: color = 8; break;
		case WDamka: color = 15; break;
		case Black: color = 1; break;
		case BDamka: color = 3; break;
		default: color = 0;
	}
	if (MyColor == White) {
		y = 7-y;
	} else {
		x = 7-x;
	}

	Mouse_Hide();
	circle(8 + x*43, 8 + y*43, 29, color);
	Mouse_Show();
}

void circle(int x1, int y1, int d, byte color) {
	int r = d/2;
	for (int y=0; y<d; y++) {
		for (int x=0; x<d; x++) {
			if ((x-r)*(x-r)+(y-r)*(y-r)<r*r) {
				point(x1+x, y1+y, color);
			}
		}
	}
}

void apply_select(byte x, byte y, byte select) {
	byte color = 0;
	if (select) {
		color = 14;
		selected_x = x;
		selected_y = y;
	}
	if (MyColor == White) {
		y = 7-y;
	} else {
		x = 7-x;
	}
	int x1 = 43 - 5 + x*43;
	int x2 = 43 - 2 + x*43;
	int y1 = 3 + y*43;
	int y2 = 6 + y*43;
	Mouse_Hide();
	rectangle(x1, x2, y1, y2, color);
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
		pusha
	// Прокрутка на одну строку вверх перед печатью очередной
		mov ah, 6
		mov al, 1
		mov bx, 0000h
		mov ch, up
		mov cl, left
		mov dh, down
		mov dl, right
		int 10h
		popa
	}
	write_line(line, size, writex, writey, color);
	Mouse_Show();
}

void write_line(byte* line, int size, byte writex, byte writey, byte color) {
	asm {
		pusha
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
		popa
	}
}

void debug_print_byte(byte b, byte color) {
	byte c = b + 48;
	debug_print(&c, 1, color);
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
		asm mov cx, 7400
	l:
		asm loop l
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
	byte random3 = r % 3 + 1;
	return random3;
}

void connect() {
	// Получение
	byte c;
	Get_Chr();
	asm jc no_char
	asm mov c, al
	if (argslen) debug_print(&c, 1, 8);
	timer55 = 0;
	connect_get_automat(c);

no_char:
	// Отправка
	connect_send_automat();
	
	if (timer55 > 18 && cgs != CGSTART) {
		cgs = CGSTART;
		css = CSSTART;
		whetherC0 = 0;
		timer55 = 0;
	}
}

void connect_send_automat() {
	switch (css) {
		case CSSTART: {
			if (timer18 > 18) {
				//debug_print('T', 1, 4);
				timer18 = 0;
				css = C0PING;
				debug_print("Установка соединения", 20, 8);
				send_str("C0", 2);
				debug_print("C0", 2, Dmy);
				whetherC0 = 1;
			}
			break;
		}
		case C0PING: {
			if (timer18 > 18) {
				//debug_print('T', 1, 4);
				timer18 = 0;
				cgs = CGSTART;
				send_str("C0", 2);
				debug_print("C0", 2, Dmy);
			}
			break;
		}
		case CxSEND: {
			if (whetherC0) {
				STR[0] = 'C';
				STR[1] = Cx + 48;
/*				send_str("C0", 2);
				debug_print("C0", 2, Dmy);*/
				send_str(STR, 2);
				debug_print(STR, 2, Dmy);
				// Если через 18 не будет ответного Cx, идем на начало.
				timer18 = 0;
			}
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
				whetherC0 = 0;
			}
			break;
		}
		case GetC: {
			if (c == '0') {
				cgs = GetC0;
				css = CxSEND;
				debug_print("C0", 2, Dhis);
				whetherC0 = 1;
				Cx = random_gesture();
			} else {
				cgs = CGSTART;
				css = CSSTART;
				whetherC0 = 0;
			}
			break;
		}
		case GetC0: {
			if (c == 'C') {
				cgs = GetC0C;
			} else {
				cgs = CGSTART;
				css = CSSTART;
				whetherC0 = 0;
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
					whetherC0 = 0;
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
				whetherC0 = 0;
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
					whetherC0 = 0;
				}
			}
			break; 
		}
		default: {}
	}
}

void rockPaperScissors(byte c) {
	cgs = CGEXIT;
	STR[0] = 'C';
	STR[1] = c;
	HisCx = c - 48;
	debug_print(STR, 2, Dhis);
	if (Cx == HisCx) {
		cgs = CGSTART;
		css = CSSTART;
		whetherC0 = 0;
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
		if (argslen >= 2) debug_print("SS", 2, Dmy);
		send_str("SS", 2);
		timer18 = 0;
	}
	if (timer55 >= 55) {
		debug_print("Потеряна связь с противником", 28, 4);
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

void get_command() {
	isNewCommand = 0;

	while (1) {
		byte c;
		Get_Chr();
		asm jc no_char
		asm mov c, al
		if (argslen == 2 || (argslen == 1 && c != 'S')) debug_print(&c, 1, 8);
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
				case 'S': getst = GETS; break;
				case 'C': getst = GETC; break;
				case 'N': getst = GETN; break;
				case 'D': getst = GETD; break;
				case 'F': getst = GETF; break;
				case 'E': getst = GETE; break;
				case 'L':
				case 'M': getst = GETLM;break;
				case 'X': getst = GETX; break;
				default:  getst = GETERR;
			}
			break;
		}
		case GETS: {
			if (c == 'S') {
				if (argslen >= 2) debug_print("SS", 2, Dhis);
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
			} else if (c >= '1' && c <= '3') {
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
					if (c >= '4' && c <= '9') {
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
				hishod_len = c - 48;
				getst = GETXX;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETXX: {
			if (c >= '0' && c <= '9') {
				hishod_len = hishod_len*10 + (c - 48);
				templen = 0;
				getst = GETLEN;
			} else {
				getst = GETERR;
			}
			break;
		}
		case GETLEN: {
			hishod[templen+3] = c;
			templen++;
			if (templen >= hishod_len*2) {
				hishod[0] = 'X';
				hishod[1] = hishod_len / 10 + 48;
				hishod[2] = hishod_len % 10 + 48;
				debug_print(hishod, 3+hishod_len*2, Dhis);
				comm = COMX;
				getst = GETSTART;
			}
			break;
		}
		default: {
			getst = GETERR;
		}
	}
}

mouse_click_status get_click() {
	int x, y;
	int y_step = 28*2;
	Mouse_Clc();
	asm {
		jnc no_clc;
		mov x, ax
		mov y, bx;
	}
	goto clc;
no_clc:
	return MCNone;
clc:
	if (x < 345 && y < 345) {
		x = x / 43;
		y = y / 43;
		if (MyColor == White) {
			click_x = x;
			click_y = 7-y;
		} else {
			click_x = 7-x;
			click_y = y;
		}
		return MCBoard;
	}
	if (x > 356 && x < 377) {	
		if (y > 54 && y < 100) return MCNG;
		if (y > 54+y_step && y < 100+y_step) return MCD2;
		if (y > 54+y_step*2 && y < 100+y_step*2) return MCD3;
		if (y > 54+y_step*3 && y < 100+y_step*3) return MCFF;
	}
 	return MCNone;
}

byte can_step(byte x, byte y, chip color) {
	if (can_eat(x, y, color)) return 1;
	if (can_move(x, y, color)) {
		if (!anyone_can_eat(color)) {
			return 1;
		}
	}
	return 0;
}

byte can_eat(byte x, byte y, chip color) {
	byte cell = board[y][x];
	if (color == White) {
		if (cell == White) {
			if (y<6 && board[y+2][x] == None && (board[y+1][x] == Black || board[y+1][x] == BDamka)) return 1;
			if (x>1 && board[y][x-2] == None && (board[y][x-1] == Black || board[y][x-1] == BDamka)) return 1;
			if (x<6 && board[y][x+2] == None && (board[y][x+1] == Black || board[y][x+1] == BDamka)) return 1;
		} else if (cell == WDamka) {
			return damka_can_eat(x, y, color);
		}
	} else {
		if (cell == Black) {
			if (y>1 && board[y-2][x] == None && (board[y-1][x] == White || board[y-1][x] == WDamka)) return 1;
			if (x>1 && board[y][x-2] == None && (board[y][x-1] == White || board[y][x-1] == WDamka)) return 1;
			if (x<6 && board[y][x+2] == None && (board[y][x+1] == White || board[y][x+1] == WDamka)) return 1;
		} else if (cell == BDamka) {
			return damka_can_eat(x, y, color);
		}
	}
	return 0;
}

byte damka_can_eat(byte x, byte y, chip color) {
	chip usual, damka; // Цвета противника
	if (color == White) {
		usual = Black;
		damka = BDamka;
	} else {
		usual = White;
		damka = WDamka;
	}
	byte xi, yi;
	if (forbidden_direction != DUp) {
		yi = y + 1;
		while (yi < 7) {
			byte cell = board[yi][x];
			if ((cell == usual || cell == damka) && board[yi+1][x] == None) return 1;
			if (cell != None) break;
			yi++;
		}
	}
	if (forbidden_direction != DDown) {
		yi = y - 1;
		while (yi > 0) {
			byte cell = board[yi][x];
			if ((cell == usual || cell == damka) && board[yi-1][x] == None) return 1;
			if (cell != None) break;
			yi--;
		}
	}
	if (forbidden_direction != DRight) {
		xi = x + 1;
		while (xi < 7) {
			byte cell = board[y][xi];
			if ((cell == usual || cell == damka) && board[y][xi+1] == None) return 1;
			if (cell != None) break;
			xi++;
		}
	}
	if (forbidden_direction != DLeft) {
		xi = x - 1;
		while (xi > 0) {
			byte cell = board[y][xi];
			if ((cell == usual || cell == damka) && board[y][xi-1] == None) return 1;
			if (cell != None) break;
			xi--;
		}
	}
	return 0;
}

byte anyone_can_eat(chip color) {
	for (byte y=0; y<8; y++)
		for (byte x=0; x<8; x++)
			if (can_eat(x, y, color))
				return 1;
	return 0;
}

byte can_move(byte x, byte y, chip color) {
	if (color == White) {
		if (board[y][x] == White || board[y][x] == WDamka) {
			if (y!=7 && board[y+1][x] == None) return 1;
			if (x!=0 && board[y][x-1] == None) return 1;
			if (x!=7 && board[y][x+1] == None) return 1; 
			return 0;
		}
		if (board[y][x] == WDamka) {
			if (y!=0 && board[y-1][x] == None) return 1;
		}
		return 0;
	} else {
		if (board[y][x] == Black || board[y][x] == BDamka) {
			if (y!=0 && board[y-1][x] == None) return 1;
			if (x!=0 && board[y][x-1] == None) return 1;
			if (x!=7 && board[y][x+1] == None) return 1; 
			return 0;
		}
		if (board[y][x] == BDamka) {
			if (y!=7 && board[y+1][x] == None) return 1;
		}
		return 0;
	}
}

byte do_step(byte x, byte y, chip color) {
	if (can_eat(selected_x, selected_y, color)) {
		if (do_eat(x, y, color))
			return 2; // 2 - ход закончился рубкой.
		return 0;
	} 
	return do_move(x, y, color);
}

byte do_eat(byte x, byte y, chip color) {
	if (board[selected_y][selected_x] == White || board[selected_y][selected_x] == Black) {
		return usual_do_eat(x, y, color);
	}
	return damka_do_eat(x, y, color);
}

byte damka_do_eat(byte x, byte y, chip color) {
	chip usual, damka; // Цвета противника
	if (color == White) {
		usual = Black;
		damka = BDamka;
	} else {
		usual = White;
		damka = WDamka;
	}
	byte xi, yi;
	byte enemy_x, enemy_y;
	byte enemy_count;

	if (y > selected_y && x == selected_x && forbidden_direction != DUp) {
		yi = selected_y + 1;
		enemy_count = 0;
		while (y > yi) {
			byte cell = board[yi][x];
			if (cell == None) {
				yi++; continue;
			}
			if ((cell == usual || cell == damka) && enemy_count == 0) {
				enemy_count = 1;
				enemy_x = x; enemy_y = yi;
				yi++; continue;
			}
			break;
		}
		if (yi == y && enemy_count == 1) {
			forbidden_direction = DDown;
			board[enemy_y][enemy_x] = None;
			apply_color(enemy_x, enemy_y);
			return 1;
		}
		return 0;
	}

	if (y < selected_y && x == selected_x && forbidden_direction != DDown) {
		yi = selected_y - 1;
		enemy_count = 0;
		while (y < yi) {
			byte cell = board[yi][x];
			if (cell == None) {
				yi--; continue;
			}
			if ((cell == usual || cell == damka) && enemy_count == 0) {
				enemy_count = 1;
				enemy_x = x; enemy_y = yi;
				yi--; continue;
			}
			break;
		}
		if (yi == y && enemy_count == 1) {
			forbidden_direction = DUp;
			board[enemy_y][enemy_x] = None;
			apply_color(enemy_x, enemy_y);
			return 1;
		}
		return 0;
	}

	if (x > selected_x && y == selected_y && forbidden_direction != DRight) {
		xi = selected_x + 1;
		enemy_count = 0;
		while (x > xi) {
			byte cell = board[y][xi];
			if (cell == None) {
				xi++; continue;
			}
			if ((cell == usual || cell == damka) && enemy_count == 0) {
				enemy_count = 1;
				enemy_x = xi; enemy_y = y;
				xi++; continue;
			}
			break;
		}
		if (xi == x && enemy_count == 1) {
			forbidden_direction = DLeft;
			board[enemy_y][enemy_x] = None;
			apply_color(enemy_x, enemy_y);
			return 1;
		}
		return 0;
	}

	if (x < selected_x && y == selected_y && forbidden_direction != DLeft) {
		xi = selected_x - 1;
		enemy_count = 0;
		while (x < xi) {
			byte cell = board[y][xi];
			if (cell == None) {
				xi--; continue;
			}
			if ((cell == usual || cell == damka) && enemy_count == 0) {
				enemy_count = 1;
				enemy_x = xi; enemy_y = y;
				xi--; continue;
			}
			break;
		}
		if (xi == x && enemy_count == 1) {
			forbidden_direction = DRight;
			board[enemy_y][enemy_x] = None;
			apply_color(enemy_x, enemy_y);
			return 1;
		}
		return 0;
	}

	return 0;
}

void usual_eat(byte x, byte y) {
	board[y][x] = None;
	apply_color(x, y);
	forbidden_direction = DNone;
}

byte usual_do_eat(byte x, byte y, chip color) {
	if (color == White) {
		if (x == selected_x && y > selected_y && y == selected_y+2 && (board[y-1][x] == Black || board[y-1][x] == BDamka)) {usual_eat(x, y-1); return 1;}
		if (y == selected_y && x > selected_x && x == selected_x+2 && (board[y][x-1] == Black || board[y][x-1] == BDamka)) {usual_eat(x-1, y); return 1;}
		if (y == selected_y && x < selected_x && x+2 == selected_x && (board[y][x+1] == Black || board[y][x+1] == BDamka)) {usual_eat(x+1, y); return 1;}
		return 0;
	} else {
		if (x == selected_x && y < selected_y && y+2 == selected_y && (board[y+1][x] == White || board[y+1][x] == WDamka)) {usual_eat(x, y+1); return 1;}
		if (y == selected_y && x > selected_x && x == selected_x+2 && (board[y][x-1] == White || board[y][x-1] == WDamka)) {usual_eat(x-1, y); return 1;}
		if (y == selected_y && x < selected_x && x+2 == selected_x && (board[y][x+1] == White || board[y][x+1] == WDamka)) {usual_eat(x+1, y); return 1;}
		return 0;
	}
}

byte do_move(byte x, byte y, chip color) {
	int diff_x, diff_y;
	diff_x = x - selected_x;
	diff_y = y - selected_y;
	if ((diff_x != 0 && diff_y != 0) || diff_x == diff_y) return 0;
	if (board[selected_y][selected_x] == White || board[selected_y][selected_x] == Black) {
		if (color == White || color == Black)
			if (diff_x == 1 || diff_x == -1) return 1;
		if (color == White && diff_y == 1) return 1;
		if (color == Black && diff_y == -1) return 1;
		return 0;
	}
	return damka_do_move(x, y);
}

byte damka_do_move(byte x, byte y) {
	byte xi, yi;
	yi = selected_y;
	while (yi < y) if (board[++yi][x] != None) return 0;
	yi = selected_y;
	while (yi > y) if (board[--yi][x] != None) return 0;
	xi = selected_x;
	while (xi < x) if (board[yi][++xi] != None) return 0;
	xi = selected_x;
	while (xi > x) if (board[yi][--xi] != None) return 0;
	return 1;
}

byte game_over() {
	byte count_white = 0;
	byte count_black = 0;
	byte count_wdamka = 0;
	byte count_bdamka = 0;
	byte can_move_white = 0;
	byte can_move_black = 0;
	for (byte y=0; y<8; y++)
		for (byte x=0; x<8; x++) {
			byte cell = board[y][x];
			switch (cell) {
				case White:
					count_white++;
					if (!can_move_white) can_move_white = can_move(x, y, White);
					break;
				case Black:
					count_black++;
					if (!can_move_black) can_move_black = can_move(x, y, Black);
					break;
				case WDamka:
					count_wdamka++;
					if (!can_move_white) can_move_white = can_move(x, y, White);
					break;
				case BDamka:
					count_bdamka++;
					if (!can_move_black) can_move_black = can_move(x, y, Black);
					break;
				default: {}
			}
		}
	if (count_black + count_bdamka == 0 || !can_move_black || (!count_wdamka && !count_bdamka && count_black == 1 && count_white > 1)) {
		debug_print("Победа белых!", 13, 4);
		return 1;
	}
	if (count_white + count_wdamka == 0 || !can_move_white || (!count_wdamka && !count_bdamka && count_white == 1 && count_black > 1)) {
		debug_print("Победа черных!", 14, 4);
		return 1;
	}
	if (!count_white && !count_black && count_wdamka == 1 && count_bdamka == 1) {
		debug_print("Ничья!", 6, 4);
		return 1;
	}
	return 0;
}

void write_hod(byte x, byte y) {
	myhod[3 + myhod_len*2] = x + 65;
	myhod[4 + myhod_len*2] = y + 49;
	myhod_len++;
}

void formalize_hod() {
	myhod[0] = 'X';
	myhod[1] = 48 + myhod_len / 10;
	myhod[2] = 48 + myhod_len % 10;
}

void board_init() {
	for (int y=0; y<8; y++) {
		for (int x=0; x<8; x++) {
			if (y==1 || y==2) {
				board[y][x] = White;
				apply_color(x, y);
			} else if (y==5 || y==6) {
				board[y][x] = Black;
				apply_color(x, y);
			} else {
				board[y][x] = None;
				apply_color(x, y);
			}
			apply_select(x, y, 0);
		}
	}
	Mouse_Clc_Restart();
	ggs = GGStartStepWhite;
}

void step(chip color) {
	chip damka;
	if (color == White) damka = WDamka;
	else damka = BDamka;

	if (mc == MCBoard) {
		if (board[click_y][click_x] == None) {
			byte step_status = do_step(click_x, click_y, color);
			if (step_status > 0) {
				write_hod(click_x, click_y);
				apply_select(selected_x, selected_y, 0);
				chip old_chip = board[selected_y][selected_x];
				board[selected_y][selected_x] = None;
				apply_color(selected_x, selected_y);
				board[click_y][click_x] = old_chip;
				if (step_status == 2 && can_eat(click_x, click_y, color)) { // 2 - возможен повторный ход, т.к. предыдущий был рубкой.
					apply_color(click_x, click_y);
					apply_select(click_x, click_y, 1);
					if ((color == White && click_y == 7) || (color == Black && click_y == 0)) futureDamka = 1;
				} else {
					if (futureDamka || (color == White && click_y == 7) || (color == Black && click_y == 0))
						board[click_y][click_x] = damka;
					apply_color(click_x, click_y);
					formalize_hod();
					debug_print(myhod, 3 + 2*myhod_len, Dmy);
					send_str(myhod, 3 + 2*myhod_len);
					forbidden_direction = DNone;
					if (game_over()) {
						ggs = GGNEW;
					} else {
						if (color == White) ggs = GGStartStepBlack;
						else ggs = GGStartStepWhite;
					}
				}
			}
		}
	}
}

void his_step() {
	chip damka;
	if (HisColor == White) damka = WDamka;
	else damka = BDamka;

	click_x = hishod[3 + 2*hishod_part] - 65;
	click_y = hishod[4 + 2*hishod_part] - 49;
	hishod_part++;

	if (board[click_y][click_x] == None) {
		byte step_status = do_step(click_x, click_y, HisColor);
		if (step_status > 0) {
			apply_select(selected_x, selected_y, 0);
			chip old_chip = board[selected_y][selected_x];
			board[selected_y][selected_x] = None;
			apply_color(selected_x, selected_y);
			board[click_y][click_x] = old_chip;
			if (step_status == 2 && can_eat(click_x, click_y, HisColor)) { // 2 - возможен повторный ход, т.к. предыдущий был рубкой.
				apply_color(click_x, click_y);
				apply_select(click_x, click_y, 1);
				if ((HisColor == White && click_y == 7) || (HisColor == Black && click_y == 0)) futureDamka = 1;
				if (hishod_part < hishod_len) {
					his_step();
				} else {
					debug_print("his_step error4", 15, 6);
					error();
				}
			} else {
				if (futureDamka || (HisColor == White && click_y == 7) || (HisColor == Black && click_y == 0))
					board[click_y][click_x] = damka;
				apply_color(click_x, click_y);
				forbidden_direction = DNone;
				if (hishod_part == hishod_len) {
					if (game_over()) {
						ggs = GGNEW;
					} else {
						if (HisColor == White) ggs = GGStartStepBlack;
						else ggs = GGStartStepWhite;
					}
					Mouse_Clc_Restart();
				} else {
					debug_print("his_step error3", 15, 6);
					error();
				}
			}
		} else {
			debug_print("his_step error2", 15, 6);
			error();
		}
	} else {
		debug_print("his_step error1", 15, 6);
		error();
	}
}

void start_my_step() {
	chip usual, damka;
	global_game_status st;
	if (MyColor == White) {
		usual = White;
		damka = WDamka;
		st = GGStepWhite;
	} else {
		usual = Black;
		damka = BDamka;
		st = GGStepBlack;
	}

	if (mc == MCBoard) {
		if (board[click_y][click_x] == usual || board[click_y][click_x] == damka) {
			if (can_step(click_x, click_y, MyColor)) {
				apply_select(click_x, click_y, 1);
				futureDamka = 0;
				forbidden_direction = DNone;
				myhod_len = 0;
				write_hod(click_x, click_y);
				ggs = st;
			}
		}
	}
}

void start_his_step() {
	if (comm != COMX || isNewCommand != 1) return;
	isNewCommand = 0;

	chip usual, damka;
	if (HisColor == White) {
		usual = White;
		damka = WDamka;
	} else {
		usual = Black;
		damka = BDamka;
	}

	click_x = hishod[3] - 65;
	click_y = hishod[4] - 49;
	hishod_part = 1;

	if (board[click_y][click_x] == usual || board[click_y][click_x] == damka) {
		if (can_step(click_x, click_y, HisColor)) {
			apply_select(click_x, click_y, 1);
			futureDamka = 0;
			forbidden_direction = DNone;
			his_step();
		} else {
			debug_print("start_his_step error2", 21, 6);
			error();
		}
	} else {
		debug_print("start_his_step error1", 21, 6);
		debug_print_byte(click_x, 6);
		debug_print_byte(click_y, 6);
		debug_print_byte(board[click_y][click_x] + 48, 6);
		error();
	}
}

void start_step(chip color, chip player) {
	if (player == HisColor && color != HisColor)
		error();
	if (color == MyColor) start_my_step();
	else start_his_step();
}

void new_game() {
	if (NG_is_sent == 1 && NG_is_received == 1) {
		if (MyColor == White) {MyColor = Black; HisColor = White;}
		else {MyColor = White; HisColor = Black;}
		board_init();
	}
}

void game() {
	mc = get_click();
	if (mc != MCNone) {
		switch (mc) {
			case MCBoard: {
				if (drawn_proposed_by_me || drawn_proposed_by_him) break;
				switch (ggs) {
					case GGStartStepWhite: start_step(White, MyColor); break;
					case GGStartStepBlack: start_step(Black, MyColor); break;
					case GGStepWhite: step(White); break;
					case GGStepBlack: step(Black); break;
					default: {}
				}
				break;
			}
			case MCNG: {
				if (ggs == GGSTART) {
					if (NG_is_sent == 0) {
						debug_print("NG", 2, Dmy);
						send_str("NG", 2);
						NG_is_sent = 1;
					}
					new_game();
				}
				break;
			}
			case MCD2: {
				if (ggs != GGSTART && ggs != GGNEW && ggs != GGCONNECT) {
					if (drawn_proposed_by_him) {
						debug_print("D2", 2, Dmy);
						send_str("D2", 2);
						debug_print("Я подтверждаю ничью", 19, 4);
						ggs = GGNEW;
					} else if (!drawn_proposed_by_me && ((ggs == GGStartStepWhite || ggs == GGStepWhite) && MyColor == White) ||
							   ((ggs == GGStartStepBlack || ggs == GGStepBlack) && MyColor == Black)) {
						debug_print("D2", 2, Dmy);
						send_str("D2", 2);
						debug_print("Я предлагаю ничью", 17, 4);
						drawn_proposed_by_me = 1;
					}
				}
				break;
			}
			case MCD3: {
				if (drawn_proposed_by_him) {
					debug_print("D3", 2, Dmy);
					send_str("D3", 2);
					debug_print("Я не согласен на ничью", 22, 4);
					drawn_proposed_by_him = 0;
				}
				break;
			}
			case MCFF: {
				if (ggs != GGSTART && ggs != GGNEW && ggs != GGCONNECT && !drawn_proposed_by_him && !drawn_proposed_by_me) {
					debug_print("FF", 2, Dmy);
					send_str("FF", 2);
					debug_print("Я сдаюсь", 8, 4);
					ggs = GGNEW;
				}
				break;
			}
			default: {}
		}
	}

	get_command();
	if (!isNewCommand) return;
	if (getst == GETERR) {
		debug_print("GETERR", 6, 4);
		error();
	}
	switch (comm) {
		case COMX: {
			if (drawn_proposed_by_me || drawn_proposed_by_him) {error(); break;}
			switch (ggs) {
				case GGStartStepWhite: start_step(White, HisColor); break;
				case GGStartStepBlack: start_step(Black, HisColor); break;
				case GGStepWhite: step(White); break;
				case GGStepBlack: step(Black); break;
				default: {}
			}
			break;
		}
		case COMNG: {
			if (ggs == GGSTART) {
				NG_is_received = 1;
				new_game();	
			} else error();
			break;
		}
		case COMD0: {
			if (ggs == GGSTART || ggs == GGNEW || ggs == GGCONNECT) ggs = GGCONNECT;
			else ggs = GGNEW;
			break;
		}
		case COMD2: {
			if (drawn_proposed_by_me) {
				debug_print("Противник согласился на ничью", 29, 4);
				ggs = GGNEW;
			} else if (ggs != GGSTART && ggs != GGNEW && ggs != GGCONNECT && !drawn_proposed_by_him && !drawn_proposed_by_me) {
				debug_print("Противник предложил ничью", 25, 4);
				drawn_proposed_by_him = 1;
			} else error();
			break;
		}
		case COMD3: {
			if (drawn_proposed_by_me) {
				debug_print("Противник отклонил ничью", 24, 4);
				drawn_proposed_by_me = 0;
			} else error();
			break;
		}
		case COMFF: {
			if (ggs == GGSTART || ggs == GGNEW || ggs == GGCONNECT) {error(); break;}
			debug_print("Противник сдался", 16, 4);
			ggs = GGNEW;
			break;
		}
		case COMEX: {
			debug_print("Противник ушел", 14, 4);
			ggs = GGCONNECT;
			break;
		}
		case COMC0: { 
			ggs = GGCONNECT;
			break;
		}
		case COMCx: { 
			ggs = GGCONNECT;
			break;
		}
		default: {}
	}
}
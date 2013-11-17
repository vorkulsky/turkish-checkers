/* com.asm */
#define BYTE unsigned char
extern Ser_Ini();
extern Ser_Rst();
extern Out_Chr();
extern Get_Chr();

/* int9h.asm */
extern Key_Ini();
extern Key_Rst();
extern Key_Is_Esc();

BYTE check_cf() {
	BYTE bCF = 1;
	asm jc loop1
	asm mov bCF, 0
	loop1:
	return bCF;
}

void main() {
	Key_Ini();
	Ser_Ini();
	while (1) {
		Key_Is_Esc();
		if (check_cf() == 1)
			goto endmain;
	}
endmain:
	Ser_Rst();
	Key_Rst();
}
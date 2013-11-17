/* com.asm */
extern Ser_Ini();
extern Ser_Rst();
extern Out_Chr();
extern Get_Chr();

/* int9h.asm */
extern Key_Ini();
extern Key_Rst();
extern Key_Is_Esc();

void do_something();

void main() {
	Key_Ini();
	Ser_Ini();
	while (1)
	{
		Key_Is_Esc();
		asm jc endmain
		do_something();
	}
endmain:
	Ser_Rst();
	Key_Rst();
}

void do_something() {
}
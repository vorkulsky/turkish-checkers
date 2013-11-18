.model tiny, C
.386
.code
locals
; game_c.c
	extrn main: near
; com.asm
	extrn Ser_Ini: near
	extrn Ser_Rst: near
; int9h.asm
	extrn Key_Ini: near
	extrn Key_Rst: near
	extrn Key_Is_Esc: near
; graphics.asm
	extrn Grph_Ini: near
	extrn Grph_Rst: near
	extrn Grph_Empty_Board: near
; mouse.asm
	extrn Mouse_Ini: near
	extrn Mouse_Rst: near
org 100h

Prog:
	call Key_Ini
	call Grph_Ini
	call Mouse_Ini
	call Grph_Empty_Board
	call Ser_Ini
	
	call esc_loop
	
	call Ser_Rst
	call Mouse_Rst
	call Grph_Rst
	call Key_Rst
	ret
	
esc_loop proc near
	call Key_Is_Esc
	jc @@end
	call main_loop
	jmp esc_loop
@@end:
	ret
esc_loop endp

main_loop proc near
	ret
main_loop endp

end Prog
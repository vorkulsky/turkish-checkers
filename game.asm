.model tiny, C
.386
.code
locals
; com.asm
	extrn Ser_Ini: near
	extrn Ser_Rst: near
	extrn Out_Chr: near
	extrn Get_Chr: near
; int9h.asm
	extrn Key_Ini: near
	extrn Key_Rst: near
	extrn Key_Is_Esc: near
org 100h

Prog:
	call Key_Ini
	call Ser_Ini
	
	call Ser_Rst
	call Key_Rst
	ret
end Prog
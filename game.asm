.model tiny, C
.386
.code
locals
; com.asm
	extrn main: near
org 100h

Prog:
	call main
	ret
end Prog
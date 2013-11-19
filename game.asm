.model tiny, C
.386
.code
locals
; gamec.cpp
	extrn main: near
org 100h

Prog:
	call main
	ret
end Prog
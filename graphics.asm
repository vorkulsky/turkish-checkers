.model tiny, C
.386
.code
	public Grph_Ini
	public Grph_Rst
	public Grph_Empty_Board
; mouse.asm
	extrn Mouse_Hide: near
	extrn Mouse_Show: near
org 5h

Grph_Ini proc near
	; Переход в 10h графический режим
	mov ah, 00h
	mov al, 10h
	int 10h
	mov ah, 0fh
	int 10h
	mov p, bh
	ret
Grph_Ini endp

Grph_Rst proc near
; Возвращение стандартного видео-режима	
	mov ah, 00h
	mov al, 3h
	int 10h
	ret
Grph_Rst endp

Grph_Empty_Board proc near
	call Mouse_Hide
	mov color, 6
	
; Расчерчиваем по вертикали
	mov y1, 0d
	mov y2, 345d
	mov x1, 0d
	mov x2, 0d
	add x2, 1
	mov cx, 9
@@vertical:
	call printrectangle
	add x1, 43d
	add x2, 43d
	loop @@vertical

; Расчерчиваем по горизонтали
	mov x1, 0d
	mov x2, 345d
	mov y1, 0d
	mov y2, 0d
	add y2, 1
	mov cx, 9
@@horizontal:
	call printrectangle
	add y1, 43d
	add y2, 43d
	loop @@horizontal
	
	call Mouse_Show
	ret
Grph_Empty_Board endp

printrectangle proc near
	push ax
	push bx
	push cx
	push dx
	mov cx, x1
	mov dx, y1
	mov ah, 0ch
	mov al, color
	mov bh, p
	@@pry:
		@@prx:	
			int 10h
			inc cx
			cmp cx, x2
			jle @@prx
		mov cx, x1
		inc dx
		cmp dx, y2
		jle @@pry
	pop dx
	pop cx
	pop bx
	pop ax
	ret
printrectangle endp

color db, 0
p db, 0
x1 dw, 0
x2 dw, 0
y1 dw, 0
y2 dw, 0

end
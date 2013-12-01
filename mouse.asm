.model tiny, C
.386
.code
	public Mouse_Ini
	public Mouse_Rst
	public Mouse_Clc
	public Mouse_Show
	public Mouse_Hide
org 5h

Mouse_Ini proc near
; Инициализация мыши
	mov ax, 00h
	int 33h
; Отобразить мышь
	call Mouse_Show
; Назначить действия
	mov ax, 0ch
	mov cx, 0000000000000101b
	push cs
	pop es
	mov dx, offset mouse
	int 33h
	ret
Mouse_Ini endp

Mouse_Rst proc near
; Возвращаем мышь
	mov ax, 0ch
	mov cx, 0
	mov bx, 0
	push bx
	pop es
	mov dx, 0
	int 33h
	ret
Mouse_Rst endp

Mouse_Clc proc near
; CF=1 - был клик, смотри координаты в x=ax, y=bx
	cmp lb, 1
	jne @@no_clc
	mov lb, 0
	mov ax, xl
	mov bx, yl
	stc ; CF=1
	ret
@@no_clc:
	clc ; CF=0
	ret
Mouse_Clc endp

Mouse_Show proc near
	push ax
	mov ax, 01h
	int 33h
	pop ax
	ret
Mouse_Show endp

Mouse_Hide proc near
	push ax
	mov ax, 02h
	int 33h
	pop ax
	ret
Mouse_Hide endp

mouse:
	cmp ax, 0000000000000001b
	jne @@els
	mov x, cx
	mov y, dx
	retf
@@els:
	cmp ax, 00000000000000100b
	jne @@els2
	mov lb, 1
	mov ax, x
	mov xl, ax
	mov ax, y
	mov yl, ax
	retf
@@els2:
	retf

x dw, 0
y dw, 0
xl dw, 0 ; Координаты в тот момент, когда в последний раз была отпущена ЛКМ (левая кнопка мыши)
yl dw, 0
lb db, 0 ; Была отпущена ЛКМ.

end
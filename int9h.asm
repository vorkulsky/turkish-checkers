.model tiny, C
.386
.code
	public Key_Ini
	public Key_Rst
	public Key_Is_Esc
org 5h

Key_Ini proc near
	push ax
	push bx
	push dx
	push es
; Получаем старый вектор в old_int9h
    mov ax, 3509h ; 35 - код команды получания вектора прерывания. 9 - номер вектора.
    int 21h
    mov word ptr old_int9h, bx
    mov word ptr old_int9h+2, es
; Устанавливаем новый обработчик int9h
    mov ax, 2509h
    mov dx, offset int9h
    int 21h
	pop es
	pop dx
	pop bx
	pop ax
	ret
Key_Ini endp

Key_Rst proc near
	push ax
	push dx
	push ds
; Возвращаем вектор клавиатуры	
	mov dx, word ptr cs:[old_int9h]
    mov ds, word ptr cs:[old_int9h+2]
    mov ax, 2509h
    int 21h
	pop ds
	pop dx
	pop ax
	ret
Key_Rst endp

Key_Is_Esc proc near
	; CF=1 YES
	cmp buffer[1], 1h
	jne no_key
	push ax
	mov al, buffer[0]
	mov buffer[0], 0
	cmp al, 81h
	pop ax
	je is_esc
no_key:
	clc ; CF=0
	ret
is_esc:
	stc ; CF=1
	ret
Key_Is_Esc endp

int9h proc near
	push ax
	push di
	push es
	in al, 60h
	push cs
	pop es
	mov di, offset buffer
	stosb
	mov al, 1
	stosb
	pop es
	pop di
	in al, 61h
	mov ah, al
	or al, 80h
	out 61h, al
	xchg ah, al
	out 61h, al
	mov al, 20h
	out 20h, al
	pop ax
	iret
int9h endp

buffer db 0,0
old_int9h dd ?

end
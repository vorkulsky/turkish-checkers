.model tiny, C
.386
.code
	public Timer_Ini
	public Timer_Rst
org 5h

Timer_Ini proc near
	mov timer1, ax
	mov timer2, bx
	push es
	push dx
; Получаем старый вектор в old_int8h.
    mov ax, 3508h ; 35 - код команды получания вектора прерывания. 8 - номер вектора.
   	int 21h
    mov word ptr old_int8h, bx
    mov word ptr old_int8h+2, es
; Устанавливаем новый обработчик int8h.
    mov ax, 2508h
    mov dx, offset int8h
    int 21h
    pop dx
    pop es
	ret
Timer_Ini endp

Timer_Rst proc near
	push dx
	push ds
	push ax
	mov dx, word ptr cs:[old_int8h]
    mov ds, word ptr cs:[old_int8h+2]
    mov ax, 2508h
   	int 21h
   	pop ax
   	pop ds
   	pop dx
	ret
Timer_Rst endp

int8h:
	push ax
	push si
	pushf
	call dword ptr cs:[old_int8h]
    mov si, timer1
    mov al, byte ptr [si]
	inc al
	mov byte ptr [si], al
	mov si, timer2
	mov al, byte ptr [si]
	inc al
	mov byte ptr [si], al
	pop si
	pop ax
	iret

timer1 dw 0 ; указатели на переменные в си-коде.
timer2 dw 0
old_int8h dd ?

end
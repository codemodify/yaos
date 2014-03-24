
%ifndef display_inc
%define display_inc

;*************************************************************************
;   clearScreen()
;
;   Uses BIOS routines to clear the screan by scrolling down one page
;
;*************************************************************************
clearScreen:
    push    ax
    push    bx
    push    cx
    push    dx

    mov     ax, 0x0600
    mov     bx, 0x0700
    mov     cx, 0x0000
    mov     dx, 0x184f
    int     0x10

    pop     dx
    pop     cx
    pop     bx
    pop     ax

    ret




;*************************************************************************
;   setCursorPosition()
;
;   Uses BIOS routines to set cursor position at X and Y coordinates
;
;   Input:
;   'dh' - X coordinate
;   'dl' - Y coordinate
;
;*************************************************************************
setCursorPosition:
    push    ax
    push    bx

    mov     ah, 0x02
    mov     bh, 0x00
    int     0x10

    pop     bx
    pop     ax

    ret




;*************************************************************************
;   displayMessage()
;
;   Uses BIOS routines to output a string.
;
;   Input:
;   "ds:si" - the string
;
;*************************************************************************
displayMessage:
    push    ax
    push    bx

    mov     ah, 0x0e
    mov     bx, 0x0007

    lodsb
    or      al, al
    jz      .doneDisplayMessage

    int     0x10
    jmp     displayMessage
    
    .doneDisplayMessage:
    pop     bx      
    pop     ax

    ret





%endif

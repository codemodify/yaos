;
; 8042 kbd controller ports:
; PORT   ACTION    PURPOSE
; ---------------------------------------------
; 0x60   READ      Output register for getting data from the keyboard
; 0x60   WRITE     Data register for sending kbd controller commands
; 0x64   READ      Status register that can be read at anytime for kbd status
; 0x64   WRITE     Commmand register used to set kbd controller options(like 
;                  the A20 gate)
; 

%ifndef keyboard_inc
%define keyboard_inc




;*************************************************************************
;   waitForKeyboardToClear()
;
;   Uses keyboard controller to check its status
;
;*************************************************************************
waitForKeyboardToClear:
    push    ax

    xor     al, al
    in      al, 0x64
    test    al, 0x2
    jnz     waitForKeyboardToClear

    pop     ax

    ret




;*************************************************************************
;   waitForKeyboardToFill()
;
;   Uses keyboard controller to check its status
;
;*************************************************************************
waitForKeyboardToFill:
    push    ax
    push    cx

    xor     cx, cx
    in      al, 0x64
    test    al, 0x1
    jnz     waitForKeyboardToFill

    pop     cx
    pop     ax

    ret




%endif

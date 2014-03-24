.ifndef display_inc
#.def display_inc

/*************************************************************************
*
*   clearScreen()
*
*   Uses BIOS routines to clear the screan by scrolling down one page.
*
*************************************************************************/
clearScreen:
        /* save */
        push    %ax
        push    %bx
        push    %cx
        push    %dx

        /* action */
        mov     $0x0600,    %ax
        mov     $0x0700,    %bx
        mov     $0x0000,    %cx
        mov     $0x184f,    %dx
        int     $0x10

        /* restore */
        pop     %dx
        pop     %cx
        pop     %bx
        pop     %ax
        ret


/*************************************************************************
*
*   setCursorPosition()
*
*   Uses BIOS routines to set cursor position at X and Y coordinates.
*
*   Input:
*   'dh' - X coordinate
*   'dl' - Y coordinate
*
*************************************************************************/
setCursorPosition:
        /* save */
        push    %ax
        push    %bx

        /* action */
        mov     $0x02,      %ah
        mov     $0x00,      %bh
        int     $0x10

        /* restore */
        pop     %bx
        pop     %ax
        ret


/*************************************************************************
*
*   displayChar()
*
*   Uses BIOS routines to output a char
*
*   Input:
*   "ds:si" - the string
*
**************************************************************************/
displayChar:
        /* save */
        push    %bx

        /* action */
        mov     $0x7,   %bx
        movb    $0xe,   %ah
        int     $0x10

        /* restore */
        pop     %bx
        ret


/*************************************************************************
*
*   displayMessage()
*
*   Uses BIOS routines to output a string.
*
*   Input:
*   "ds:si" - the string
*
**************************************************************************/
displayMessage:
        /* save */
        push    %ax
        push    %bx

        /* action */
        mov     $0x0e,      %ah
        mov     $0x0007,    %bx
    
        lodsb
        or      %al,        %al
        jz      .doneDisplayMessage
    
        int     $0x10
        jmp     displayMessage
        
        .doneDisplayMessage:

        /* restore */
        pop     %bx      
        pop     %ax
        ret


#.endef
.endif

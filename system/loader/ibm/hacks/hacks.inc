.ifndef hacks_inc

/*************************************************************************
*
*   prepareMachineForWork()
*
*   Implementing this as macro since there is no way to have a 'ret'
*   the segments will be changed, thus the context is outdated.
*
*************************************************************************/
.macro  prepareMachineForWork
    /*
        BIOS puts us at 0000:7C00h, 
        so set segment registers to point to the start of the bootsector.
        Therefore, we don't have to add 0:7C00h to all our data
    */
        mov     $0x7C00,    %ax
        mov     %ax,        %ds
        mov     %ax,        %es
        mov     %ax,        %fs
        mov     %ax,        %gs

    /*
        Setup the stack.
        0x9000 - seems to be the typical place for a stack
    */
        cli
        mov     $0x9000,    %ax
        mov     %ax,        %ss
        mov     $0xffff,    %sp
        sti
.endm


/*************************************************************************
*
*   enableGateA20()
*
*************************************************************************/
enableGateA20:
        ret

.endif

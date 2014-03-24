/*************************************************************************
*
*   The BIOS loads the bootsector at linear offset 0x7C00,
*   the state of the registers are:
*
*   DL = Boot drive, 1h = floppy1, 80h = primary harddisk, etc
*   CS = 0
*   IP = 0x7c00
*
**************************************************************************/

.code16

#.section    .data
    .set    loaderSignature,    0xaa55

#.section    .text
    _includeAreaStart:
            jmp         _includeAreaEnd
            .include    "bios/display.inc"
            .include    "hacks/hacks.inc"
    _includeAreaEnd:

    _codeAreaStart:
            prepareMachineForWork
            call        enableGateA20

            movb        $'F',   %al
            call        displayChar
            int         $0x00

            mov         $drive, %si
            call        displayMessage

    _codeAreaEnd:

    drive:  .ascii              "Drrrrrrrrrrrrrrrrrrive "

    .org    510
    .word   loaderSignature

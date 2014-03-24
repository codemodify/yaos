;
; Boot Process:
; ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
; o) Powerup.
; o) All registers set to zero.
; o) CPU is set to reset state.
; o) The address 0xffff is loaded into the code segment and
;    instructions from this address are executed.
;
; At 0xffff is the BIOS - the basic and primitive software.
; BIOS is embedded by the manufacturers, what is does is:
; o) runs checks for memory, serial ports, flopy, etc ...
; o) searches for 512-bytes of code with 0xaa55 ending signature 
;    called "boot-loader".
; o) loads the boot loader into memory at 0x7c00 address and executes it.
;
;
;;;;;;; BIOS looks for the 0xaa55 signature on the first sector(called boot sector) 
;;   ;; on each device that is connected. The search is done in a order specified 
;;   ;; in the BIOS configuration menu, that is adjustable by the user. 
;; N ;; The 0xaa55 - is called "boot signature".
;; O ;;
;; T ;; The code loaded by the BIOS is called "boot-strap loader".
;; E ;; It may or may not be a part of an OS, but it is installed from within an OS.
;; S ;; Ex: GRUB, LILO is not part of Windows, but it can load it.
;;   ;; Ex: ntldr - is part of Windows and is intalled at Windows setup.
;;   ;; Ex: freebsd bood loader - is part of the FreeBSD installed at FreeBSD setup.
;;;;;;;
;;;;;;; To install a boot loader - means to copy a small(512 bytes) chunk of code
;;;;;;; in the first sector of the Disk, that later is to be found by the BIOS
;;;;;;; and executed. The boot loader will continue the job started by BIOS and
;;;;;;; will load the OS itself.
;
;
; After BIOS loads and before it starts loading the boot-loader, the memory looks like this:
; 0x0000 to 0x0040 - Interrupt Service Routine Vectors.
; 0x0040 to 0x0100 - BIOS data area- is the area where the BIOS saves data about the memory available, devices connected etc...
; 0x0100 to 0x07c0 - Free, available for the OS and applications.
; 0x07c0 to 0x07e0 - is where the boot-strap loader is loaded by the BIOS.
; 0x7ce0 to 0xa000 - Free, available for the OS and applications.
; 0xA000 to 0xc000 - set aside for the video sub-system, we can write/draw to the screen directly bypassing the BIOS routines.
; 0xc000 to 0xf000 - is the location of the ROM BIOS, and we cannot write to that area under normal conditions.
; 0xf000 to 0xffff - is the base ROM system ROM (the top of which is where the first instruction is present on a re-boot).
;
;;;;;;; Typically boot loaders employ a second stage loader: init(for unixes) - with
;;   ;; more functionality than can be placed into a 512 byte block.
;;   ;;
;; N ;; To make this happen, there are 2 approches:
;; O ;; o) to assume that the second stage loader is located immediately after the
;; T ;;    bootsector (after 512 bytes)(after first stage loader) on the disk.
;; E ;;    The bootstrap only needs to know how many sectors to load and can immediately 
;; S ;;    load the appropriate sectors into memory and transfer control to the loaded file.
;;   ;;    The bad side:
;;   ;;         while placing the files to be loaded immediately after the first sector will 
;;;;;;;         make coding the bootsectoor easier, it becomes increasingly difficult to
;;;;;;;         implement a working file system on the disk afterward.
;;;;;;;
;;;;;;; o) to build a bootstrap program of greater complexity, by making it understand file systems.
;;;;;;;    This allows the second stage loader to be placed within the file system, so the first
;;;;;;;    stage loader will detect it there.




; The Boot Loader
    [BITS   16]
    [ORG    0x0000]
    
    jmp     bootLoader

tables:
    OEM_ID                  db "QUASI-OS"
    BytesPerSector          dw 0x0200
    SectorsPerCluster       db 0x01
    ReservedSectors         dw 0x0001
    TotalFATs               db 0x02
    MaxRootEntries          dw 0x00E0
    TotalSectorsSmall       dw 0x0B40
    MediaDescriptor         db 0xF0
    SectorsPerFAT           dw 0x0009
    SectorsPerTrack         dw 0x0012
    NumHeads                dw 0x0002
    HiddenSectors           dd 0x00000000
    TotalSectorsLarge       dd 0x00000000
    DriveNumber             db 0x00
    Flags                   db 0x00
    Signature               db 0x29
    VolumeID                dd 0xFFFFFFFF
    VolumeLabel             db "QUASI  BOOT"
    SystemID                db "FAT12   "

    
bootLoader:
    ; at the moment to transfer control from BIOS to boot-loader BIOS not
    ; setup DS, ES, FS, SS segments; we have to initiate them to 0x07C0.
    ; When we change the stack segment register, we have to stop all interrupts 
    ; as we won't know the current value of the SS and any interrupts happening 
    ; during the change can result in loss of data and even in crashing of the system.
    cli
    mov     ax, 0x07c0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    
    ; create stack
    mov     ax, 0x0000
    mov     ss, ax
    mov     sp, 0xffff
  
    ; make the anounce that we are running
    mov     si, _greetingString
    call    displayMessage

    ; What we do here is:
    ; A) enabling the addressing of more than 1MB of memory
    ; B) load the kernel binary data
    ; C) enter protected mode
    ; D) setup GDT, 
    ; E) setup IDT,
    ; F) 32bit stack(protected mode stack)
    ; G) start the loaded kernel
    
step_A:
    ; The A20 gate is a bit in the keyboard's controller that enables or 
    ; disables a mode called "wrap-around". The A20 gate simply allows the cpu 
    ; to manage memory through a 20-bit bus, thus attaining access to over the 1MB mark.
    ; If the A20 gate isn't enabled, it would wrap back around to the beginning of memory.
    cli
    call    waitForKeyboardToClear
    mov     al, 0xd1
    out     0x64, al
    call    waitForKeyboardToClear
    mov     al, 0xdf
    out     0x60, al
    call    waitForKeyboardToClear
        
    mov     al, 0xd0
    out     0x64, al
    call    waitForKeyboardToFill
    in      al, 0x60
    test    al, 2
    jnz     step_B
    
    sti
    mov     si, _a20errorString
    call    displayMessage
    jmp     dumbLoop

step_B:
    sti
    mov     si, _a20errorString
    call    displayMessage

    

dumbLoop:
    jmp     dumbLoop

    ; internal data used by the loader
    _greetingString      db      'greetings. boot-loader-stage1 here.',10,13,0
    _a20errorString      db      'error. A20 gate not open.',10,13,0

    ; use the BIOS routines for text output
    %include "bios/display.inc"
    %include "bios/keyboard.inc"

    ; fill the rest of the sector with zero's.
    times   510-($-$$) db 0                 

    ; put the boot-loader-signature at the end, so BIOS can recognize us.
    dw      0xAA55                          

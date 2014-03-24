;
; The FAT12 file system is commonly used on floppy disks, but not necessarily.
;
; There are two data blocks inherent to FAT12 formatted disks:
; o OEM_ID - no purpose other than to identify what software performed the disk format.
; o BIOS Parameter Block - the BPB is a record containing fields that describe the physical 
;                          attributes of the disk. These attributes can be used to determine 
;                          characteristics such as the disk's total capacity.
; 
; The BIOS immediately begins executing the code loaded at 0000:7C00, which includes the BPB. 
; Therefore, the first step for the bootstrap loader is to perform a JMP operation to the code 
; located below the BPB.
;
;
;
; FAT12 model to organize the disk
; ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
; o) A sequential array  - each data cell is called cluster.
;    Depending on how the disk was formatted, the number of sectors per cluster will vary. 
;
; o) A structure known as the FAT (File Allocation Table) keeps track of each cluster on the disk.
;    The FAT itself contains an integer value indicating the next cluster in the file. 
;    By following the chain of indexes until the EOF (End Of File) value is located, 
;    it is possible to locate the contents of any file regardless of fragmentation. 
;
; o) A structure called the Root Directory, responsible for indexing filenames against their first cluster. 
;
;
;
; Putting everything together, a file is located by searching for its name in the root directory 
; and then following a chain of indexes through the FAT to identify the which physical disk sectors 
; it is saved on.

%ifndef fat_12_inc
%define fat_12_inc

;*************************************************************************
;   readSectors()
;
;   Uses BIOS routines to read raw data from the disk into memory.
;   "cx"    -   number of sectors
;   "ax"    -   start adrress on the disk to read from
;   "es:bx" -   memory address to write to
;
;*************************************************************************
readSectors:
.MAIN
    mov     di, 0x0005                          ; five retries for error
.SECTORLOOP
    push    ax
    push    bx
    push    cx
    call    LBACHS
    mov     ah, 0x02                            ; BIOS read sector
    mov     al, 0x01                            ; read one sector
    mov     ch, BYTE [absoluteTrack]            ; track
    mov     cl, BYTE [absoluteSector]           ; sector
    mov     dh, BYTE [absoluteHead]             ; head
    mov     dl, BYTE [DriveNumber]              ; drive
    int     0x13                                ; invoke BIOS
    jnc     .SUCCESS                            ; test for read error
    xor     ax, ax                              ; BIOS reset disk
    int     0x13                                ; invoke BIOS
    dec     di                                  ; decrement error counter
    pop     cx
    pop     bx
    pop     ax
    jnz     .SECTORLOOP                         ; attempt to read again
    int     0x18
.SUCCESS
    mov     si, msgProgress
    call    DisplayMessage
    pop     cx
    pop     bx
    pop     ax
    add     bx, WORD [BytesPerSector]           ; queue next buffer
    inc     ax                                  ; queue next sector
    loop    .MAIN                               ; read next sector
    ret




;*************************************************************************
;   clusterToLBA()
;
;   Converts Microsoft's FAT cluster addressing scheme into a 
;   Logical Block Address for mapping the file to the disk.
;
;   LBA = (cluster - 2) * sectors_per_cluster
;
;*************************************************************************
clusterToLBA:
    sub     ax, 0x0002                          ; zero base cluster number
    xor     cx, cx
    mov     cl, BYTE [SectorsPerCluster]        ; convert byte to word
    mul     cx
    add     ax, WORD [datasector]               ; base data sector
    ret



    
;*************************************************************************
;   LBAtoCHS()
;
;   Converts the Logical Block Address into the Cylinder Head Sector 
;   format understood by the BIOS for directly accessing the file.
;   "ax"    -   LBA address
;
;   absolute sector = ( logical_sector /  sectors_per_track ) + 1
;   absolute head   = ( logical_sector /  sectors_per_track ) MOD number_of_heads
;   absolute track  =   logical_sector / ( sectors_per_track * number_of_heads )
;
;*************************************************************************
LBAtoCHS:
     xor     dx, dx                              ; prepare dx:ax for operation
     div     WORD [SectorsPerTrack]              ; calculate
     inc     dl                                  ; adjust for sector 0
     mov     BYTE [absoluteSector], dl
     xor     dx, dx                              ; prepare dx:ax for operation
     div     WORD [NumHeads]                     ; calculate
     mov     BYTE [absoluteHead], dl
     mov     BYTE [absoluteTrack], al
     ret




%endif
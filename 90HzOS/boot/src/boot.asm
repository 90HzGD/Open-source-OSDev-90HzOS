[BITS 16]
[org 0x7c00]

_start:
    jmp short start
    nop

CODE_SEG equ 0x8
DATA_SEG equ 0x10

Kernel_adr equ 0x1000
Kernel_Destination equ 0x100000
BOOT_DISK: db 0
LOAD_BYTES: dd 0

Usable_RAMSpace_Baseptr equ 0x8000
Usable_RAMSpace_length  equ 0x9000

Sectors_load_nbr: db 0

%define STACK_ADR 0x9000

start:
    cli
    ; Setup stack
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov bp, STACK_ADR
    mov sp, bp
    sti

    mov [BOOT_DISK], dl
    xor ax, ax
    int 0x13
    jc halt

    mov ah, 0x02
    mov dh, 0x00
    mov ch, 0x00
    mov bx, Kernel_adr
    mov cl, 0x02
    mov al, 0x20
    mov [Sectors_load_nbr], al
    mov dl, [BOOT_DISK]
    int 0x13

    mov si, Disk_Error
    jc halt
    mov si, 0

    in al, 0x92
    or al, 2
    out 0x92, al

    call Get_RAM_Info

    jmp Set_pm

halt:
    call Print_string16
    hlt
    jmp $                   ; Infinite loop for no crashes

GDT_Start:
    NULL_Descriptor:
        dd 0
        dd 0
    Code_Descriptor:
        dw 0xFFFF
        dw 0
        db 0
        db 0b10011010
        db 0b11001111
        db 0
    Data_Descriptor:
        dw 0xFFFF
        dw 0
        db 0
        db 0b10010010
        db 0b11001111
        db 0
GDT_End:

GDT_Descriptor:
    dw GDT_End - GDT_Start - 1  ; Size
    dd GDT_Start                ; Start

Set_pm:
    cli
    lgdt[GDT_Descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:Protected_Mode

VRAM_ADR32 equ 0xB8000

Print_string16:
    cmp [si], 0
    je return_str16
    jmp Print_char16
Print_char16:
    mov ah, 0x0E
    mov al, [si]
    int 0x10
    add si, 1
    jmp Print_string16
return_str16:
    ret

Get_RAM_Info:
    mov ebx, 0
    mov esi, 0
    RAMInfo_loop:
        mov di, RAMInfo_Buffer      ; es:di
        mov edx, 0x534D4150         ; Signature
        mov eax, 0xE820             ; Mode
        mov ecx, 0x14               ; Bytes we allow to the BIOS to write into
        int 0x15
        jc RAM_Error
        cmp ebx, 0
        je RAM_Info_return
        cmp eax, 0x534D4150
        jne RAM_Error
        mov eax, [RAMInfo_Buffer.type]
        cmp eax, 1
        je Add_RAM_Info
        jmp RAMInfo_loop
    Add_RAM_Info:
        mov eax, [RAMInfo_Buffer.base_low]
        jmp Update_URAMBaseptr
    Update_URAMBaseptr:
        mov [Usable_RAMSpace_Baseptr + esi], eax
        mov eax, [RAMInfo_Buffer.length_low]
        mov [Usable_RAMSpace_length + esi], eax
        add esi, 0x04
        jmp RAMInfo_loop
    RAM_Info_return:
        mov [Usable_RAMSpace_Baseptr + esi], 0
        mov [Usable_RAMSpace_length + esi], 0
        mov esi, 0
        mov eax, 0
        ret

RAM_Error:
    mov ah, 0x0E
    mov si, RAM_ERRstr
    call Print_string16
    .Err_loop:
        hlt
        jmp RAM_Error.Err_loop

RAM_ERRstr: db "RAM Err", 0

RAMInfo_Buffer:
    .base_low:       dd 0
    .base_high:      dd 0
    .length_low:     dd 0
    .length_high:    dd 0
    .type:           dd 0
RAMInfo_Buffer_End:

Disk_Error: db "Disk Err", 0

[BITS 32]
Protected_Mode:
    ; Init stack
    mov ax, DATA_SEG
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov ds, ax
    mov esp, 0x9000
    mov ebp, esp

    mov esi, Loading_string
    mov ecx, 0
    call Print_stringPM

    mov ecx, 0
    mov eax, 0
    call Multiply
    call Move_Kernel

    mov esp, 0x900000
    mov ebp, esp

    jmp CODE_SEG:Kernel_Destination

Move_Kernel:
    mov esi, Kernel_adr
    mov edi, Kernel_Destination
    mov ecx, [LOAD_BYTES]
    rep movsb
    ret

Multiply:
    cmp al, [Sectors_load_nbr]
    je return
    add al, 1
    add ecx, 512
    jmp Multiply
return:
    mov [LOAD_BYTES], ecx
    mov ecx, 0
    mov eax, 0
    ret

Print_stringPM:
    cmp [esi], 0
    je return_str
    jmp Print_charPM
Print_charPM:
    mov al, [esi]
    mov ah, 0x0F
    mov [VRAM_ADR32+ecx], al
    mov [VRAM_ADR32+ecx+1], ah
    add ecx, 2
    add esi, 1
    jmp Print_stringPM
return_str:
    ret

Loading_string: db "Loading Kernel", 0

times 510-($-$$) db 0
dw 0xAA55
[BITS 32]

global extended
global Scan_code
global Ctrl_pressed
global LeftShiftPressed
global RightShiftPressed
global ToogleCaps
global Alt_pressed
global Ctrl_combine
global combine_idx

extended: db 0
combine_idx: db 0
Ctrl_combine: times 6 db 0
Alt_pressed: db 0
ToogleCaps: db 0
RightShiftPressed: db 0
LeftShiftPressed: db 0
Scan_code: db 0
Ctrl_pressed: db 0
extern handle_kb

global inb
global outb
global load_idt
global enable_int
global kb_handler

load_idt:
    mov edx, [esp+4]
    lidt [edx]
    ret

enable_int:
    sti
    ret

kb_handler:
    pushad                  ; Save registers
    cld
    call handle_kb
    mov [Scan_code], al

    cmp al, 0
    je kb_handler_end
    cmp [Scan_code], 0x1D
    je ctr_pressed
    cmp [Scan_code], 0x9D
    je ctr_released
    cmp [Scan_code], 0x2A
    je LeftShift_pressed
    cmp [Scan_code], 0xAA
    je LeftShift_released
    cmp [Scan_code], 0x36
    je RightShift_pressed
    cmp [Scan_code], 0xB6
    je RightShift_released
    cmp [Scan_code], 0x3A
    je toogle_capslock
    cmp [Scan_code], 0x38
    je alt_pressed
    cmp [Scan_code], 0xB8
    je alt_released
kb_handler_end:
    popad
    iretd

ctr_pressed:
    mov [Ctrl_pressed], 1
    jmp kb_handler_end

ctr_released:
    mov [Ctrl_pressed], 0
    mov [combine_idx], 0
    mov [Ctrl_combine], 0
    jmp kb_handler_end

LeftShift_pressed:
    mov [LeftShiftPressed], 1
    jmp kb_handler_end

LeftShift_released:
    mov [LeftShiftPressed], 0
    jmp kb_handler_end

RightShift_pressed:
    mov [RightShiftPressed], 1
    jmp kb_handler_end

RightShift_released:
    mov [RightShiftPressed], 0
    jmp kb_handler_end

toogle_capslock:
    cmp [ToogleCaps], 0
    je toogle1
    jmp toogle0
toogle1:
    mov [ToogleCaps], 1
    jmp kb_handler_end
toogle0:
    mov [ToogleCaps], 0
    jmp kb_handler_end

alt_pressed:
    mov [Alt_pressed], 1
    jmp kb_handler_end

alt_released:
    mov [Alt_pressed], 0
    jmp kb_handler_end

inb:
    mov edx, [esp+4]        ; C Arg#1 == unsigned short port
    in eax, dx              ; Set in byte eax with port dx | eax = C ret value
    ret

outb:
    mov edx, [esp+4]        ; == unsigned short port
    mov eax, [esp+8]        ; == unsigned char value
    out dx, al              ; Set al value in out port
    ret

; Stack:
    ; 0: ret
    ; 4: arg1
    ; 8: arg2...
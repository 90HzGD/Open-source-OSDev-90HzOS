[BITS 32]

global exec
extern free_str

args_adr: dd 0

exec:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    mov [args_adr], eax

    push eax
    call edx

    add esp, 4

    mov eax, [args_adr]
    push eax
    call free_str

    add esp, 4
    ret
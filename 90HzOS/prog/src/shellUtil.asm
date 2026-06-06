[BITS 32]

global exec

exec:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    mov [esp + 4], eax
    call edx
    ret
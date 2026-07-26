[BITS 32]

global exec
extern freeze

exec:
    mov edx, [esp + 4]
    mov eax, [esp + 8]

    push eax
    call edx
    add esp, 4
    ret
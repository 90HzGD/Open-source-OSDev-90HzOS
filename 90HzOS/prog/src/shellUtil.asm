[BITS 32]

global exec
extern free_str
extern exec_err
extern freeze

args_adr: dd 0

exec:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    mov [args_adr], eax

    push eax
    call edx
    add esp, 4
    cmp [eax], 0
    jne com_error

exec_end:

    mov eax, [args_adr]
    push eax
    call free_str

    add esp, 4
    ret

com_error:
    push eax
    call exec_err
    add esp, 4
    jmp exec_end
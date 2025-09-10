; src/start.s
BITS 32
section .text
global _start

extern main

_start:
    ; 设置堆栈指针
    mov esp, 0x90000

    ; 调用C main函数
    call main

    ; 如果main返回，进入无限循环
    cli
.hang:
    hlt
    jmp .hang
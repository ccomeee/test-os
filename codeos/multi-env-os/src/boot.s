BITS 16
ORG 0x7c00

start:
    ; 設定段寄存器
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; 顯示載入消息
    mov si, msg
    call print_string

    ; 讀取第二階段 stage2.bin（假設 4 扇區）到 0x0600:0000
    mov ax, 0x0600
    mov es, ax
    xor bx, bx
    ; 保留 BIOS 提供的啟動磁碟代號 DL，不要覆寫
    mov dh, 0           ; 磁頭 0
    mov ch, 0           ; 柱面 0
    mov cl, 2           ; 從第2扇區開始
    mov al, 4           ; 讀取 4 扇區
    mov ah, 0x02
    int 0x13
    jc disk_error

    ; 跳轉到 stage2
    jmp 0x0600:0000

BITS 16
disk_error:
    mov si, error_msg
    call print_string
    mov al, ah
    call print_hex
    jmp $

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    mov bx, 0x0007
    int 0x10
    jmp print_string
.done:
    ret

print_hex:
    mov bl, al
    shr al, 4
    call print_hex_digit
    mov al, bl
    and al, 0x0F
    call print_hex_digit
    ret

print_hex_digit:
    cmp al, 10
    jl .digit
    add al, 'A' - 10
    jmp .print
.digit:
    add al, '0'
.print:
    mov ah, 0x0E
    int 0x10
    ret

msg db 'Stage1: load stage2...', 0
error_msg db ' Disk error: ', 0

times 510-($-$$) db 0
dw 0xAA55
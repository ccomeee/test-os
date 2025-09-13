BITS 16
ORG 0x7c00

start:
    ; 设置段寄存器
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; 显示加载消息
    mov si, msg
    call print_string

    ; 加载内核到 0x10000
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x0000
    
    ; 載入足夠的扇區
    mov dh, 0
    mov dl, 0
    mov ch, 0
    mov cl, 2
    mov ah, 0x02
    mov al, 50
    int 0x13
    jc disk_error
    
    ; 顯示載入成功
    mov si, load_success_msg
    call print_string

    ; 顯示準備跳轉到核心
    mov si, pm_msg
    call print_string

    ; 使用 BIOS 中斷顯示核心訊息
    mov si, kernel_msg1
    call print_string
    
    mov si, kernel_msg2
    call print_string

    ; 切換到 VBE 模式 0x117 (640x480x16bpp) 並啟用 LFB
    mov ax, 0x4F02
    mov bx, 0x4117         ; bit14=1 要求 Linear Framebuffer
    int 0x10

    ; 讀取 VBE 模式資訊 0x117 到 ES:DI = 0x9000:0000
    mov ax, 0x9000
    mov es, ax
    xor di, di
    mov ax, 0x4F01
    mov cx, 0x0117
    int 0x10

    ; 從 ES:DI 的 ModeInfo 結構擷取欄位，寫到實體 0x8000
    ; 我們的簡化結構：
    ; 0x8000: width  (2)
    ; 0x8002: height (2)
    ; 0x8004: bpp    (2)
    ; 0x8006: pitch  (2)
    ; 0x8008: physbase (4)

    mov bx, 0x8000
    ; 寫 width/height/bpp 為 0x117 既知值
    mov ax, 640
    mov [bx+0], ax
    mov ax, 480
    mov [bx+2], ax
    mov ax, 16
    mov [bx+4], ax
    ; 先取 LinBytesPerScanLine (0x58)，若為 0 則退回 BytesPerScanLine (0x10)
    mov ax, [es:di+0x58]
    test ax, ax
    jnz .have_pitch
    mov ax, [es:di+0x10]
.have_pitch:
    mov [bx+6], ax
    ; 寫 PhysBasePtr (offset 0x28, 4 bytes)
    mov eax, [es:di+0x28]
    mov [bx+8], eax

    ; 切换到保护模式
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:protected_mode

BITS 32
protected_mode:
    ; 设置段寄存器
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    
    ; 跳转到内核
    jmp 0x10000

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

print_to_vga:
    push ax
    push si
    push di
    push es
print_loop:
    lodsb
    or al, al
    jz print_done
    mov ah, 0x07  ; 白色文字，黑色背景
    stosw
    jmp print_loop
print_done:
    pop es
    pop di
    pop si
    pop ax
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

; GDT
gdt_start:
    dq 0x0000000000000000    ; 空描述符
gdt_code:
    dw 0xFFFF               ; 段界限 0-15
    dw 0x0000               ; 段基址 0-15
    db 0x00                 ; 段基址 16-23
    db 0x9A                 ; 代码段，可读/执行
    db 0xCF                 ; 粒度4KB，32位模式
    db 0x00                 ; 段基址 24-31
gdt_data:
    dw 0xFFFF               ; 段界限 0-15
    dw 0x0000               ; 段基址 0-15
    db 0x00                 ; 段基址 16-23
    db 0x92                 ; 数据段，可读/写
    db 0xCF                 ; 粒度4KB，32位模式
    db 0x00                 ; 段基址 24-31
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

msg db 'Loading kernel...', 0
load_success_msg db ' Loaded 50 sectors!', 0
pm_msg db ' Jumping to kernel...', 0
success_msg db ' Kernel loaded! Jumping to kernel...', 0
error_msg db ' Disk error: ', 0
kernel_msg1 db 'Hello from Multi-Env OS!', 0
kernel_msg2 db 'Kernel loaded successfully!', 0

times 510-($-$$) db 0
dw 0xAA55
BITS 16
ORG 0x0000

%define DAP     0x0600
%define TMPBUF  0x7A00
%define KERNEL_LOAD_SEG 0x1000
%define RES_LOAD_SEG    0x7000
%define STAGE2_SECTORS  4
%define MANIFEST_LBA    (1 + STAGE2_SECTORS)
%define MANIFEST_SECTORS 1
%define MANIFEST_HEADER_SIZE 12
%define MANIFEST_ENTRY_SIZE 32
%define MAX_STREAM_CHUNK_SECTORS 32
%define MAX_WALLPAPER_BYTES 0x90000

start2:
    cli
    push cs
    pop ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov dl, [boot_drive]
    mov ah, 0x41
    mov bx, 0x55AA
    int 0x13
    jc .no_extensions
    cmp bx, 0xAA55
    jne .no_extensions
    test cx, 0x01
    jz .no_extensions
    mov byte [ext_available], 1
    jmp .detect_geometry
.no_extensions:
    mov byte [ext_available], 0
.detect_geometry:
    mov dl, [boot_drive]
    mov ah, 0x08
    int 0x13
    jc .geom_error
    mov al, dh
    inc al
    mov [chs_heads], al
    mov al, cl
    and al, 0x3F
    mov [chs_sectors], al
    xor ax, ax
    mov al, ch
    xor bx, bx
    mov bl, cl
    and bl, 0xC0
    shl bx, 2
    or ax, bx
    inc ax
    mov [chs_cylinders], ax
    jmp .after_detect
.geom_error:
    mov byte [chs_heads], 16
    mov byte [chs_sectors], 63
    mov word [chs_cylinders], 1024
.after_detect:
    mov ah, 0x0e
    mov al, 'A'
    int 0x10
    mov ax, [chs_cylinders]
    call print_hex16
    movzx ax, byte [chs_heads]
    call print_hex16
    movzx ax, byte [chs_sectors]
    call print_hex16
    ; DS=ES 已指向本段

    call load_manifest
    mov ah, 0x0e
    mov al, 'm'
    int 0x10
    call load_kernel_stream
    mov ah, 0x0e
    mov al, 'k'
    int 0x10
    call load_wallpaper_stream
    jc after_nores
    mov ah, 0x0e
    mov al, 'w'
    int 0x10
    call setup_wallpaper_metadata
    jmp continue_boot

after_nores:
    call clear_wallpaper_metadata

continue_boot:
    ; 切 VBE 模式與寫 ModeInfo 至 0x8000（與原先相同）
    mov ax, 0x4F02
    mov bx, 0x4117
    int 0x10

    mov ax, 0x9000
    mov es, ax
    xor di, di
    mov ax, 0x4F01
    mov cx, 0x0117
    int 0x10

    mov bx, 0x8000
    mov ax, 640
    mov [bx+0], ax
    mov ax, 480
    mov [bx+2], ax
    mov ax, 16
    mov [bx+4], ax
    mov ax, [es:di+0x58]
    test ax, ax
    jnz .have_pitch
    mov ax, [es:di+0x10]
.have_pitch:
    mov [bx+6], ax
    mov ax, [es:di+0x28]
    mov [bx+8], ax
    mov ax, [es:di+0x2A]
    mov [bx+10], ax

    ; 進入保護模式並跳到 kernel 0x10000
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:pm2

BITS 32
pm2:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    jmp 0x10000

BITS 16
; Manifest & streaming loader routines
load_manifest:
    mov byte [kernel_entry_found], 0
    mov byte [wallpaper_entry_found], 0
    mov byte [wallpaper_available], 0
    mov word [RES_BYTES_LO], 0
    mov word [RES_BYTES_HI], 0

    mov ah, 0x0e
    mov al, 'B'
    int 0x10
    mov word [dap_count], MANIFEST_SECTORS
    mov word [dap_off], TMPBUF
    mov word [dap_seg], 0x0000
    mov eax, MANIFEST_LBA
    mov [dap_lba], eax
    mov dword [dap_lba+4], 0
    call lba_read
    jc disk_error2
    mov ah, 0x0e
    mov al, '1'
    int 0x10

    push ds
    mov ax, ds
    push ax
    pop es
    xor ax, ax
    mov ds, ax
    mov si, TMPBUF
    mov di, manifest_signature
    mov cx, 4
    repe cmpsb
    jne .error
    mov ah, 0x0e
    mov al, '2'
    int 0x10
    mov bx, [TMPBUF+4]
    mov ax, [TMPBUF+6]
    cmp ax, MANIFEST_ENTRY_SIZE
    jne .error
    mov si, TMPBUF
    add si, MANIFEST_HEADER_SIZE
.loop:
    cmp bx, 0
    je .done
    push bx
    push si
    mov di, manifest_name_kernel
    mov cx, 8
    repe cmpsb
    pop si
    je .store_kernel
    push si
    mov di, manifest_name_wallpaper
    mov cx, 8
    repe cmpsb
    pop si
    je .store_wall
    jmp .next
.store_kernel:
    push ds
    push es
    pop ds
    call store_kernel_entry
    mov ah, 0x0e
    mov al, 'K'
    int 0x10
    pop ds
    jmp .next
.store_wall:
    push ds
    push es
    pop ds
    call store_wallpaper_entry
    mov ah, 0x0e
    mov al, 'W'
    int 0x10
    pop ds
.next:
    pop bx
    dec bx
    add si, MANIFEST_ENTRY_SIZE
    jmp .loop
.done:
    pop ds
    mov ah, 0x0e
    mov al, '3'
    int 0x10
    cmp byte [kernel_entry_found], 1
    jne .err2
    ret
.error:
    pop ds
    mov ah, 0x0e
    mov al, 'E'
    int 0x10
.err2:
    jmp disk_error2

store_kernel_entry:
    mov byte [kernel_entry_found], 1
    mov eax, [ds:si+8]
    mov [kernel_lba], eax
    mov eax, [ds:si+12]
    mov [kernel_len], eax
    mov eax, [ds:si+16]
    mov [kernel_dest], eax
    mov eax, [ds:si+20]
    mov [kernel_flags], eax
    ret

store_wallpaper_entry:
    mov byte [wallpaper_entry_found], 1
    mov eax, [ds:si+8]
    mov [wallpaper_lba], eax
    mov eax, [ds:si+12]
    mov [wallpaper_len], eax
    mov eax, [ds:si+16]
    mov [wallpaper_dest], eax
    mov eax, [ds:si+20]
    mov [wallpaper_flags], eax
    ret

load_kernel_stream:
    mov ah, 0x0e
    mov al, 'C'
    int 0x10
    mov eax, [kernel_len]
    mov [stream_bytes], eax
    mov eax, [kernel_lba]
    mov [curr_lba], eax
    mov eax, [kernel_dest]
    mov [dest_lin], eax
    call stream_read
    ret

load_wallpaper_stream:
    mov ah, 0x0e
    mov al, 'D'
    int 0x10
    cmp byte [wallpaper_entry_found], 1
    jne .no_entry
    mov eax, [wallpaper_len]
    mov edx, MAX_WALLPAPER_BYTES
    cmp eax, edx
    jbe .len_ok
    mov eax, edx
.len_ok:
    mov [wallpaper_effective_len], eax
    mov [stream_bytes], eax
    mov eax, [wallpaper_lba]
    mov [curr_lba], eax
    mov eax, [wallpaper_dest]
    mov [dest_lin], eax
    call stream_read
    jc .fail
    mov eax, [wallpaper_effective_len]
    mov [RES_BYTES_LO], ax
    mov edx, eax
    shr edx, 16
    mov [RES_BYTES_HI], dx
    mov eax, [wallpaper_effective_len]
    add eax, 511
    shr eax, 9
    mov [res_sectors], ax
    cmp ax, 1152
    jbe .len_ok2
    mov ax, 1152
    mov [res_sectors], ax
    mov bx, ax
    shl bx, 9
    mov [RES_BYTES_LO], bx
    mov word [RES_BYTES_HI], 0
    movzx eax, bx
    mov [wallpaper_effective_len], eax
.len_ok2:
    mov byte [wallpaper_available], 1
    clc
    ret
.fail:
    stc
    ret
.no_entry:
    mov byte [wallpaper_available], 0
    mov word [RES_BYTES_LO], 0
    mov word [RES_BYTES_HI], 0
    mov word [res_sectors], 0
    stc
    ret

setup_wallpaper_metadata:
    mov di, 0x800C
    mov eax, [wallpaper_dest]
    mov dx, ax
    and dx, 0x000F
    mov [di], dx
    shr eax, 4
    mov [di+2], ax
    mov ax, [RES_BYTES_LO]
    mov [di+4], ax
    mov ax, [RES_BYTES_HI]
    mov [di+6], ax
    ret

clear_wallpaper_metadata:
    mov di, 0x800C
    xor ax, ax
    mov [di], ax
    mov [di+2], ax
    mov [di+4], ax
    mov [di+6], ax
    ret

stream_read:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
.stream_loop:
    mov eax, [stream_bytes]
    test eax, eax
    jz .stream_done
    mov ecx, eax
    add ecx, 511
    shr ecx, 9
    cmp ecx, 0
    jne .chunk_chk
    mov ecx, 1
.chunk_chk:
    cmp ecx, MAX_STREAM_CHUNK_SECTORS
    jbe .chunk_ok
    mov ecx, MAX_STREAM_CHUNK_SECTORS
.chunk_ok:
    mov [dap_count], cx
    mov eax, [dest_lin]
    mov edx, eax
    and edx, 0xF
    mov [dap_off], dx
    shr eax, 4
    mov [dap_seg], ax
    mov eax, [curr_lba]
    mov [dap_lba], eax
    mov dword [dap_lba+4], 0
    call lba_read
    jc .stream_fail
    movzx eax, cx
    shl eax, 9
    mov edi, eax
    mov eax, [dest_lin]
    add eax, edi
    mov [dest_lin], eax
    mov eax, [curr_lba]
    movzx edx, cx
    add eax, edx
    mov [curr_lba], eax
    mov eax, [stream_bytes]
    cmp eax, edi
    ja .stream_reduce
    mov dword [stream_bytes], 0
    jmp .stream_loop
.stream_reduce:
    sub eax, edi
    mov [stream_bytes], eax
    jmp .stream_loop
.stream_done:
    clc
    jmp .stream_exit
.stream_fail:
    stc
.stream_exit:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; INT13h AH=42h LBA 讀
lba_read:
    push si
    mov si, DAP
    mov byte [si], 0x10
    mov byte [si+1], 0x00
    mov ax, [dap_count]
    mov [si+2], ax
    mov ax, [dap_off]
    mov [si+4], ax
    mov ax, [dap_seg]
    mov [si+6], ax
    mov eax, [dap_lba]
    mov [si+8], eax
    mov dword [si+12], 0
    ; 使用 BIOS 啟動時提供的 DL（不要覆寫）
    mov dl, [boot_drive]
    mov cx, 3
.lba_retry:
    cmp byte [ext_available], 0
    je .use_chs
    mov ah, 0x42
    int 0x13
    jnc .success
    push ax
    mov ah, 0x00
    int 0x13
    pop ax
    loop .lba_retry
.use_chs:
    pop si
    jmp chs_read
.success:
    pop si
    clc
    ret

chs_read:
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push bp

    mov cx, [dap_count]
    movzx eax, word [dap_seg]
    shl eax, 4
    movzx ebx, word [dap_off]
    add eax, ebx
    mov [tmp_linear], eax
    mov eax, [dap_lba]
    mov [tmp_lba], eax

.chs_loop:
    cmp cx, 0
    jz .chs_done

    movzx ebx, byte [chs_sectors]
    mov eax, [tmp_lba]
    xor edx, edx
    div ebx
    mov [tmp_sector], dx
    mov [tmp_div], eax

    movzx ebx, byte [chs_heads]
    mov eax, [tmp_div]
    xor edx, edx
    div ebx
    mov [tmp_cylinder], ax
    mov [tmp_head], dx

    cmp ax, [chs_cylinders]
    jae .chs_fail

    mov ax, [tmp_cylinder]
    call print_hex16
    movzx ax, byte [tmp_head]
    call print_hex16
    movzx ax, byte [tmp_sector]
    call print_hex16

    mov eax, [tmp_linear]
    mov ebx, eax
    and bx, 0x000F
    shr eax, 4
    mov [tmp_seg], ax
    mov [tmp_off], bx

    mov es, [tmp_seg]
    mov bx, [tmp_off]

    mov dl, [boot_drive]
    mov dh, byte [tmp_head]
    mov ax, [tmp_cylinder]
    mov ch, al
    mov al, ah
    and al, 0x03
    shl al, 6
    mov cl, byte [tmp_sector]
    inc cl
    and cl, 0x3F
    or cl, al
    mov al, 1
    mov ah, 0x02
    int 0x13
    jnc .chs_ok
    push ax
    mov ah, 0x00
    int 0x13
    pop ax
    int 0x13
    jc .chs_fail
.chs_ok:

    mov eax, [tmp_linear]
    add eax, 512
    mov [tmp_linear], eax

    mov eax, [tmp_lba]
    inc eax
    mov [tmp_lba], eax

    dec cx
    jmp .chs_loop

.chs_fail:
    stc
    jmp .chs_exit

.chs_done:
    clc

.chs_exit:
    pop bp
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret

manifest_signature    db 'MFS1'
manifest_name_kernel  db 'KERNEL',0,0
manifest_name_wallpaper db 'WALLPAPR'

kernel_entry_found    db 0
wallpaper_entry_found db 0
wallpaper_available   db 0
align_pad_manifest    db 0

kernel_flags          dd 0
wallpaper_flags       dd 0

kernel_lba            dd 0
kernel_len            dd 0
kernel_dest           dd 0

wallpaper_lba         dd 0
wallpaper_len         dd 0
wallpaper_dest        dd 0
wallpaper_effective_len dd 0

stream_bytes          dd 0
curr_lba              dd 0
dest_lin              dd 0

res_sectors           dw 0
RES_BYTES_LO          dw 0
RES_BYTES_HI          dw 0

dap_count             dw 0
dap_off               dw 0
dap_seg               dw 0
dap_lba               dd 0

boot_drive            db 0
ext_available         db 0
chs_heads             db 0
chs_sectors           db 0
chs_cylinders         dw 0

tmp_sector            dw 0
tmp_cylinder          dw 0
tmp_head              dw 0
tmp_linear            dd 0
tmp_lba               dd 0
tmp_div               dd 0
tmp_seg               dw 0
tmp_off               dw 0

error_msg2 db 'Disk error in stage2',0
;
; 簡單的錯誤處理
disk_error2:
    mov si, error_msg2
    call print_string2
    mov ah, 0x0e
    mov al, ':'
    int 0x10
    ; 輸出 AH 高位
    mov al, ah
    shr al, 4
    and al, 0x0F
    add al, '0'
    cmp al, '9'
    jbe .ok1
    add al, 7
.ok1:
    mov ah, 0x0e
    int 0x10
    ; 輸出 AH 低位
    mov al, ah
    and al, 0x0F
    add al, '0'
    cmp al, '9'
    jbe .ok2
    add al, 7
.ok2:
    mov ah, 0x0e
    int 0x10
    jmp $

print_string2:
    push ax
    push si
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp .loop
.done:
    pop si
    pop ax
    ret

; 輸出16位元十六進制
print_hex16:
    push ax
    push bx
    mov bx, ax

    mov al, bh
    shr al, 4
    call print_hex_digit

    mov al, bh
    and al, 0x0F
    call print_hex_digit

    mov al, bl
    shr al, 4
    call print_hex_digit

    mov al, bl
    and al, 0x0F
    call print_hex_digit

    pop bx
    pop ax
    ret

; 輸出32位元十六進制
; print_hex32 已移除，直接在主程式分別呼叫 print_hex16 輸出高低16位


; 輸出單一十六進制位元
print_hex_digit:
    and al, 0x0F
    cmp al, 9
    jbe .digit
    add al, 7
.digit:
    add al, '0'
    mov ah, 0x0E
    int 0x10
    ret

    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp print_string2
.done:
    ret

; GDT（與 boot.s 相同佈局）
gdt_start:
    dq 0x0000000000000000
gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 2048-($-$$) db 0  ; 4 扇區大小（4*512=2048 bytes）

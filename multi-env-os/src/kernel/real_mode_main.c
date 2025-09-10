// 實模式版本的核心
void _start() {
    // 簡單的 VGA 輸出函數
    volatile unsigned short *vga = (volatile unsigned short*)0xB8000;
    
    // 顯示訊息
    const char* msg1 = "Hello from Multi-Env OS!";
    const char* msg2 = "Kernel loaded successfully!";
    
    int pos = 160; // 第二行開始
    
    // 顯示第一條訊息
    for (int i = 0; msg1[i]; i++) {
        vga[pos++] = (msg1[i] | 0x0700);
    }
    
    pos = 320; // 第四行開始
    
    // 顯示第二條訊息
    for (int i = 0; msg2[i]; i++) {
        vga[pos++] = (msg2[i] | 0x0700);
    }
    
    // 無限循環
    while (1) {
        asm volatile ("hlt");
    }
}

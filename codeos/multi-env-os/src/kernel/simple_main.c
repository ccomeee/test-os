#include "os.h"
#include "stdio.h"
#include "string.h"

// 簡單的 VGA 輸出函數
void simple_print(const char* str) {
    volatile unsigned short *vga = (volatile unsigned short*)0xB8000;
    int i = 0;
    int pos = 0;
    while (str[i]) {
        vga[pos] = (str[i] | 0x0700);
        i++;
        pos++;
    }
}

int main() {
    simple_print("Hello from Multi-Env OS!");
    simple_print("Kernel loaded successfully!");
    
    // 無限循環
    while (1) {
        asm volatile ("hlt");
    }
    
    return 0;
}

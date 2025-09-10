// 圖形化核心 - 實現 VGA 圖形模式
#include "libc.h"
#include "desktop.h"

// VGA 圖形模式設定
#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_MEMORY 0xA0000

// 顏色定義
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GRAY 7
#define COLOR_DARK_GRAY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// 直接設定 VGA 圖形模式 (模式 13h)
void set_vga_mode() {
    // 直接操作 VGA 暫存器來設定模式 13h
    // 設定 CRT 控制器索引暫存器
    asm volatile (
        "mov $0x3D4, %%dx\n"
        "mov $0x00, %%al\n"
        "out %%al, %%dx\n"
        "mov $0x3D5, %%dx\n"
        "mov $0x5F, %%al\n"
        "out %%al, %%dx\n"
        :
        :
        : "ax", "dx"
    );
    
    // 設定序列器暫存器
    asm volatile (
        "mov $0x3C4, %%dx\n"
        "mov $0x01, %%al\n"
        "out %%al, %%dx\n"
        "mov $0x3C5, %%dx\n"
        "mov $0x01, %%al\n"
        "out %%al, %%dx\n"
        :
        :
        : "ax", "dx"
    );
    
    // 設定圖形控制器
    asm volatile (
        "mov $0x3CE, %%dx\n"
        "mov $0x05, %%al\n"
        "out %%al, %%dx\n"
        "mov $0x3CF, %%dx\n"
        "mov $0x40, %%al\n"
        "out %%al, %%dx\n"
        :
        :
        : "ax", "dx"
    );
    
    // 設定調色板模式
    asm volatile (
        "mov $0x3C6, %%dx\n"
        "mov $0xFF, %%al\n"
        "out %%al, %%dx\n"
        :
        :
        : "ax", "dx"
    );
}

// 設定像素
void set_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        volatile unsigned char *vga = (volatile unsigned char*)VGA_MEMORY;
        vga[y * VGA_WIDTH + x] = color;
    }
}

// 繪製矩形
void draw_rect(int x, int y, int width, int height, unsigned char color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            set_pixel(x + j, y + i, color);
        }
    }
}

// 繪製文字（簡單的像素文字）
void draw_text(int x, int y, const char* text, unsigned char color) {
    // 簡單的 8x8 字體實現
    int pos = 0;
    while (text[pos]) {
        // 這裡可以實現更複雜的字體渲染
        // 現在只是簡單地顯示一個方塊
        draw_rect(x + pos * 8, y, 6, 8, color);
        pos++;
    }
}

// 測試圖形模式
void test_graphics_mode() {
    // 繪製彩色條紋來測試圖形模式
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            unsigned char color = (x / 10) % 16; // 每10個像素換一個顏色
            set_pixel(x, y, color);
        }
    }
}

// 主函數
void main() {
    // 直接設定圖形模式
    set_vga_mode();
    
    // 等待一下讓模式切換完成
    for (volatile int i = 0; i < 1000000; i++);
    
    // 初始化桌面檔案系統
    init_desktop_filesystem();
    // 畫出桌面
    draw_desktop();
    
    // 無限循環
    while (1) {
        asm volatile ("hlt");
    }
}

// 圖形化核心 - 實現 VGA 圖形模式
#include "libc.h"
#include "desktop.h"
#include "mouse.h"
#include "video_mode.h"

// VGA 圖形模式設定
// 動態螢幕資訊由 video_mode 提供

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

// 16 色到 16bpp(5-6-5) 的簡單對應表
static unsigned short color16_to_rgb565[16] = {
    0x0000, // 黑
    0x001F, // 藍
    0x07E0, // 綠
    0x07FF, // 青
    0xF800, // 紅
    0xF81F, // 洋紅
    0xB5A0, // 棕(近似)
    0xC618, // 淺灰
    0x7BEF, // 深灰
    0x3DEF, // 淺藍(近似)
    0x7FE0, // 淺綠(近似)
    0x7FFF, // 淺青(近似)
    0xF81F, // 淺紅(近似用洋紅)
    0xFBB7, // 淺洋紅(近似)
    0xFFE0, // 黃
    0xFFFF  // 白
};

// 設定像素（支援 8bpp 與 16bpp RGB565）
void set_pixel(int x, int y, unsigned char color) {
    unsigned short W = get_screen_width();
    unsigned short H = get_screen_height();
    if (x < 0 || x >= (int)W || y < 0 || y >= (int)H) return;

    unsigned char* fb = get_framebuffer_ptr();
    unsigned short pitch = get_screen_pitch();
    unsigned short bpp = get_screen_bpp();

    if (bpp == 8) {
        fb[y * pitch + x] = color;
    } else if (bpp == 16) {
        unsigned short* row = (unsigned short*)(fb + y * pitch);
        unsigned short px = color16_to_rgb565[color & 0x0F];
        row[x] = px;
    } else {
        // 其他 bpp 尚未支援
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

// 將畫面上一塊拷貝到緩衝區（buf 大小需為 w*h*bytesPerPixel）
void fb_read_region(int x, int y, int w, int h, void* buf) {
    unsigned short W = get_screen_width();
    unsigned short H = get_screen_height();
    if (x < 0 || y < 0 || x + w > W || y + h > H) return;
    unsigned char* fb = get_framebuffer_ptr();
    unsigned short pitch = get_screen_pitch();
    unsigned short bpp = get_screen_bpp();
    int bpp_bytes = (bpp == 16) ? 2 : 1;
    unsigned char* out = (unsigned char*)buf;
    for (int row = 0; row < h; ++row) {
        unsigned char* src = fb + (y + row) * pitch + x * bpp_bytes;
        for (int col = 0; col < w * bpp_bytes; ++col) {
            out[row * (w * bpp_bytes) + col] = src[col];
        }
    }
}

// 將緩衝區內容寫回畫面同尺寸區域
void fb_write_region(int x, int y, int w, int h, const void* buf) {
    unsigned short W = get_screen_width();
    unsigned short H = get_screen_height();
    if (x < 0 || y < 0 || x + w > W || y + h > H) return;
    unsigned char* fb = get_framebuffer_ptr();
    unsigned short pitch = get_screen_pitch();
    unsigned short bpp = get_screen_bpp();
    int bpp_bytes = (bpp == 16) ? 2 : 1;
    const unsigned char* in = (const unsigned char*)buf;
    for (int row = 0; row < h; ++row) {
        unsigned char* dst = fb + (y + row) * pitch + x * bpp_bytes;
        for (int col = 0; col < w * bpp_bytes; ++col) {
            dst[col] = in[row * (w * bpp_bytes) + col];
        }
    }
}

// 8x8 基本字型（僅涵蓋 A-Z、0-9、空白與冒號以及部分字元）
static const unsigned char font8x8_upper[36][8] = {
    // 'A'..'Z'
    {0x18,0x24,0x42,0x7E,0x42,0x42,0x42,0x00}, // A
    {0x7C,0x42,0x42,0x7C,0x42,0x42,0x7C,0x00}, // B
    {0x3C,0x42,0x40,0x40,0x40,0x42,0x3C,0x00}, // C
    {0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x00}, // D
    {0x7E,0x40,0x40,0x7C,0x40,0x40,0x7E,0x00}, // E
    {0x7E,0x40,0x40,0x7C,0x40,0x40,0x40,0x00}, // F
    {0x3C,0x42,0x40,0x4E,0x42,0x46,0x3A,0x00}, // G
    {0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x00}, // H
    {0x3E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00}, // I
    {0x02,0x02,0x02,0x02,0x42,0x42,0x3C,0x00}, // J
    {0x42,0x44,0x48,0x70,0x48,0x44,0x42,0x00}, // K
    {0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00}, // L
    {0x42,0x66,0x5A,0x5A,0x42,0x42,0x42,0x00}, // M
    {0x42,0x62,0x52,0x4A,0x46,0x42,0x42,0x00}, // N
    {0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}, // O
    {0x7C,0x42,0x42,0x7C,0x40,0x40,0x40,0x00}, // P
    {0x3C,0x42,0x42,0x42,0x4A,0x44,0x3A,0x00}, // Q
    {0x7C,0x42,0x42,0x7C,0x48,0x44,0x42,0x00}, // R
    {0x3E,0x40,0x40,0x3C,0x02,0x02,0x7C,0x00}, // S
    {0x7F,0x08,0x08,0x08,0x08,0x08,0x08,0x00}, // T
    {0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}, // U
    {0x41,0x41,0x22,0x22,0x14,0x14,0x08,0x00}, // V
    {0x41,0x41,0x49,0x49,0x49,0x2A,0x1C,0x00}, // W
    {0x42,0x24,0x18,0x18,0x18,0x24,0x42,0x00}, // X
    {0x41,0x22,0x14,0x08,0x08,0x08,0x08,0x00}, // Y
    {0x7E,0x02,0x04,0x18,0x20,0x40,0x7E,0x00}, // Z
    // '0'..'9'
    {0x3C,0x46,0x4A,0x52,0x62,0x46,0x3C,0x00}, // 0
    {0x08,0x18,0x28,0x08,0x08,0x08,0x3E,0x00}, // 1
    {0x3C,0x42,0x02,0x0C,0x30,0x40,0x7E,0x00}, // 2
    {0x7E,0x04,0x18,0x06,0x02,0x42,0x3C,0x00}, // 3
    {0x04,0x0C,0x14,0x24,0x44,0x7E,0x04,0x00}, // 4
    {0x7E,0x40,0x7C,0x02,0x02,0x42,0x3C,0x00}, // 5
    {0x1C,0x20,0x40,0x7C,0x42,0x42,0x3C,0x00}, // 6
    {0x7E,0x02,0x04,0x08,0x10,0x10,0x10,0x00}, // 7
    {0x3C,0x42,0x42,0x3C,0x42,0x42,0x3C,0x00}, // 8
    {0x3C,0x42,0x42,0x3E,0x02,0x04,0x38,0x00}  // 9
};

static void draw_char(int x, int y, char ch, unsigned char color) {
    if (ch == ' ') return; // 空白直接不畫，保留背景
    if (ch == ':') {
        // 簡易冒號
        set_pixel(x + 3, y + 2, color);
        set_pixel(x + 3, y + 5, color);
        return;
    }
    // 轉大寫並選擇字形
    if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');
    const unsigned char* glyph = 0;
    if (ch >= 'A' && ch <= 'Z') {
        glyph = font8x8_upper[ch - 'A'];
    } else if (ch >= '0' && ch <= '9') {
        glyph = font8x8_upper[26 + (ch - '0')];
    } else {
        return; // 不支援的字元略過
    }
    for (int row = 0; row < 8; ++row) {
        unsigned char bits = glyph[row];
        for (int col = 0; col < 8; ++col) {
            if (bits & (0x80 >> col)) {
                set_pixel(x + col, y + row, color);
            }
        }
    }
}

// 繪製文字（8x8 位圖字型）
void draw_text(int x, int y, const char* text, unsigned char color) {
    int cx = x;
    for (int i = 0; text[i]; ++i) {
        char ch = text[i];
        if (ch == '\n') {
            y += 8; cx = x; continue;
        }
        draw_char(cx, y, ch, color);
        cx += 8;
    }
}

// 測試圖形模式
void test_graphics_mode() {
    // 繪製彩色條紋來測試圖形模式
    unsigned short W = get_screen_width();
    unsigned short H = get_screen_height();
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            unsigned char color = (x / 10) % 16; // 每10個像素換一個顏色
            set_pixel(x, y, color);
        }
    }
}

// 主函數
void main() {
    // bootloader 已切到 VBE 0x118 並寫入 0x8000，這裡初始化 video_mode 讀取資訊
    video_mode_init();
    
    // 初始化桌面檔案系統
    init_desktop_filesystem();
    // 初始化滑鼠
    mouse_init();
    // 畫桌面
    draw_desktop();
    // 在左上角顯示模式資訊
    {
        unsigned short W = get_screen_width();
        unsigned short H = get_screen_height();
        unsigned short B = get_screen_bpp();
        char info[32];
        // 簡單組字串（避免 sprintf），固定顯示 640x480 16bpp or 8bpp 標示
        info[0]='\0';
        if (B == 16) {
            draw_text(8, 8, "VBE 16bpp", COLOR_WHITE);
        } else if (B == 8) {
            draw_text(8, 8, "VBE 8bpp", COLOR_WHITE);
        } else {
            draw_text(8, 8, "VBE ?", COLOR_WHITE);
        }
    }
    
    // 進入桌面互動主循環
    desktop_main_loop();
}

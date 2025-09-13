// mouse.c - 簡易 PS/2 滑鼠初始化與輪詢（無中斷，純輪詢）
#include "mouse.h"
#include "port_io.h"
#include "video_mode.h"

// PS/2 控制埠
#define PS2_STATUS   0x64
#define PS2_COMMAND  0x64
#define PS2_DATA     0x60

// 狀態位元
#define STAT_OUTPUT  0x01  // Output buffer status (data available)
#define STAT_INPUT   0x02  // Input buffer status (1 = controller busy)

// 簡單等待函數（使用已定義的 outb 以避免內聯組合語言暫存器問題）
static void io_wait() { outb(0x80, 0); }

static void mouse_wait_input_clear() {
    // 等待 Input buffer 清空
    for (int i = 0; i < 100000; ++i) {
        if ((inb(PS2_STATUS) & STAT_INPUT) == 0) return;
    }
}

static void mouse_wait_output_full() {
    // 等待 Output buffer 有資料
    for (int i = 0; i < 100000; ++i) {
        if (inb(PS2_STATUS) & STAT_OUTPUT) return;
    }
}

static void mouse_write(unsigned char val) {
    mouse_wait_input_clear();
    outb(PS2_COMMAND, 0xD4); // 下個資料送到滑鼠
    mouse_wait_input_clear();
    outb(PS2_DATA, val);
}

static unsigned char mouse_read() {
    mouse_wait_output_full();
    return inb(PS2_DATA);
}

static mouse_state_t g_mouse = {160, 100, 0, 0};
static int initialized = 0;

void mouse_init() {
    // 啟用第二通道（滑鼠）
    mouse_wait_input_clear();
    outb(PS2_COMMAND, 0xA8);
    io_wait();

    // 啟用資料回報
    mouse_write(0xF4);
    (void)mouse_read(); // ACK

    initialized = 1;
}

void mouse_poll(mouse_state_t* state) {
    if (!initialized) { mouse_init(); }

    // 嘗試收集三個位元組的封包
    if (!(inb(PS2_STATUS) & STAT_OUTPUT)) {
        *state = g_mouse; // 無資料，維持前狀態
        return;
    }

    unsigned char packet[3];
    packet[0] = inb(PS2_DATA);
    // 確保是同步位元（bit3 應該為 1）
    if ((packet[0] & 0x08) == 0) { *state = g_mouse; return; }
    mouse_wait_output_full(); packet[1] = inb(PS2_DATA);
    mouse_wait_output_full(); packet[2] = inb(PS2_DATA);

    // 按鍵
    int left  = packet[0] & 0x01;
    int right = packet[0] & 0x02;
    // 解析相對位移（帶符號）
    int dx = (packet[0] & 0x10) ? (int)(packet[1] | 0xFFFFFF00) : (int)packet[1];
    int dy = (packet[0] & 0x20) ? (int)(packet[2] | 0xFFFFFF00) : (int)packet[2];
    dy = -dy; // PS/2 向上為負

    g_mouse.x += dx;
    g_mouse.y += dy;
    if (g_mouse.x < 0) g_mouse.x = 0;
    if (g_mouse.y < 0) g_mouse.y = 0;
    unsigned short W = get_screen_width();
    unsigned short H = get_screen_height();
    if (g_mouse.x >= W) g_mouse.x = W - 1;
    if (g_mouse.y >= H) g_mouse.y = H - 1;
    g_mouse.left_button  = left ? 1 : 0;
    g_mouse.right_button = right ? 1 : 0;

    *state = g_mouse;
}

// 桌面圖形界面
#include <string.h>

// 桌面應用啟動表
typedef void (*app_launch_func_t)(void);
typedef struct {
    const char* name;
    app_launch_func_t launch;
} desktop_app_entry_t;

#include <file_manager.h>
#include <terminal.h>
#include <editor.h>
#include <python_env.h>
#include <nodejs_env.h>

static desktop_app_entry_t desktop_apps[] = {
    {"File Manager", open_file_manager_window},
    {"Terminal", launch_terminal},
    {"Text Editor", launch_editor},
    {"Python Environment", launch_python_env},
    {"Node.js Environment", launch_nodejs_env},
    // 可依需求再加入其他 app
};
static const int desktop_apps_count = sizeof(desktop_apps) / sizeof(desktop_app_entry_t);

void desktop_launch_app_by_name(const char* name) {
    for (int i = 0; i < desktop_apps_count; ++i) {
        if (strcmp(desktop_apps[i].name, name) == 0) {
            desktop_apps[i].launch();
            return;
        }
    }
}

#include <file_manager.h>
#include <terminal.h>
#include <editor.h>
#include <python_env.h>
#include <cpp_env.h>
#include <browser.h>
#include <desktop.h>
#include <nodejs_env.h>
// 可依需求再加入其他 app 的 .h

// VGA 圖形模式設定
#include <video_mode.h>
#include <resources.h>
// 壁紙顯示函式（BMP/PNG 將共用介面；目前先有 BMP）
int draw_bmp_onto_screen(const unsigned char* data, unsigned int len, int dst_x, int dst_y);
int draw_bmp_fit_center_screen(const unsigned char* data, unsigned int len, int bottom_reserved);

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


// 桌面圖標位置
#define ICON_SIZE 64
#define ICON_SPACING 80
#define ICONS_PER_ROW 4
#define ICON_START_X 20
#define ICON_START_Y 60

// 任務欄
#define TASKBAR_HEIGHT 30
// 任務欄 Y 由當前螢幕高度決定
#define TASKBAR_Y (get_screen_height() - TASKBAR_HEIGHT)

// 繪製桌面圖標（增加陰影、邊框）
void draw_file_icon(int x, int y, const char* name, file_type_t type) {
    // 陰影
    draw_rect(x + 3, y + 3, ICON_SIZE, ICON_SIZE, COLOR_DARK_GRAY);

    // 圖標主色
    unsigned char icon_color;
    switch (type) {
        case FILE_TYPE_APPLICATION: icon_color = COLOR_LIGHT_BLUE; break;
        case FILE_TYPE_DIRECTORY:   icon_color = COLOR_YELLOW;     break;
        default:                    icon_color = COLOR_LIGHT_GRAY; break;
    }

    // 主體
    draw_rect(x, y, ICON_SIZE, ICON_SIZE, icon_color);

    // 亮邊/暗邊（浮起感）
    draw_rect(x, y, ICON_SIZE, 2, COLOR_WHITE);                     // 上
    draw_rect(x, y, 2, ICON_SIZE, COLOR_WHITE);                     // 左
    draw_rect(x, y + ICON_SIZE - 2, ICON_SIZE, 2, COLOR_DARK_GRAY); // 下
    draw_rect(x + ICON_SIZE - 2, y, 2, ICON_SIZE, COLOR_DARK_GRAY); // 右

    // 內容符號
    if (type == FILE_TYPE_APPLICATION) {
        // 簡單『▶』形狀
        draw_rect(x + 18, y + 16, 4, 32, COLOR_WHITE);
        draw_rect(x + 24, y + 16, 4, 24, COLOR_WHITE);
        draw_rect(x + 30, y + 16, 4, 16, COLOR_WHITE);
        draw_rect(x + 36, y + 16, 4, 8,  COLOR_WHITE);
    } else if (type == FILE_TYPE_DIRECTORY) {
        // 資料夾外框
        draw_rect(x + 8,  y + 14, 48, 4,  COLOR_WHITE);
        draw_rect(x + 8,  y + 18, 4,  28, COLOR_WHITE);
        draw_rect(x + 52, y + 18, 4,  28, COLOR_WHITE);
        draw_rect(x + 8,  y + 46, 48, 4,  COLOR_WHITE);
    }

    // 名稱下劃線（暫代文字）
    draw_rect(x + 5, y + ICON_SIZE + 5, ICON_SIZE - 10, 2, COLOR_WHITE);
    // 顯示圖示名稱（最多 10 字元）
    char buf[11];
    int i = 0;
    while (name[i] && i < 10) { buf[i] = name[i]; ++i; }
    buf[i] = '\0';
    draw_text(x + 2, y + ICON_SIZE + 8, buf, COLOR_WHITE);
}

// 繪製桌面圖標
void draw_desktop_icons() {
    directory_t* desktop_files = get_desktop_files();
    
    int x = ICON_START_X;
    int y = ICON_START_Y;
    int icons_in_row = 0;
    
    for (unsigned int i = 0; i < desktop_files->file_count; i++) {
        draw_file_icon(x, y, desktop_files->files[i].name, desktop_files->files[i].type);
        
        icons_in_row++;
        if (icons_in_row >= ICONS_PER_ROW) {
            icons_in_row = 0;
            x = ICON_START_X;
            y += ICON_SPACING;
        } else {
            x += ICON_SPACING;
        }
    }
}

// 繪製任務欄（加上分隔與立體效果）
void draw_taskbar() {
    // 背景
    unsigned short W = get_screen_width();
    draw_rect(0, TASKBAR_Y, W, TASKBAR_HEIGHT, COLOR_DARK_GRAY);
    // 上方高光線
    draw_rect(0, TASKBAR_Y, W, 1, COLOR_LIGHT_GRAY);

    // 開始按鈕
    int btn_y = TASKBAR_Y + 5;
    draw_rect(5, btn_y, 60, 20, COLOR_BLUE);
    draw_rect(5, btn_y, 60, 1, COLOR_WHITE); // 上亮邊
    draw_rect(5, btn_y, 1, 20, COLOR_WHITE); // 左亮邊
    draw_rect(5, btn_y + 19, 60, 1, COLOR_BLACK); // 下暗邊
    draw_rect(5 + 59, btn_y, 1, 20, COLOR_BLACK); // 右暗邊
    // 顯示 START 文字
    draw_text(12, btn_y + 6, "START", COLOR_WHITE);

    // 檔案管理器按鈕
    draw_rect(75, btn_y, 60, 20, COLOR_GREEN);
    draw_rect(75, btn_y, 60, 1, COLOR_WHITE);
    draw_rect(75, btn_y, 1, 20, COLOR_WHITE);
    draw_rect(75, btn_y + 19, 60, 1, COLOR_BLACK);
    draw_rect(75 + 59, btn_y, 1, 20, COLOR_BLACK);
    draw_rect(80, btn_y + 10, 50, 2, COLOR_WHITE);

    // 右側時間區塊
    W = get_screen_width();
    draw_rect(W - 80, btn_y, 75, 20, COLOR_LIGHT_GRAY);
    draw_rect(W - 80, btn_y, 75, 1, COLOR_WHITE);
    draw_rect(W - 80, btn_y, 1, 20, COLOR_WHITE);
    draw_rect(W - 80, btn_y + 19, 75, 1, COLOR_BLACK);
    draw_rect(W - 6,  btn_y, 1, 20, COLOR_BLACK);
    // 顯示時間（暫時固定），之後可替換為 RTC
    draw_text(W - 72, btn_y + 6, "12:00", COLOR_BLACK);
}

#include "mouse.h"

// Start 選單（簡單面板）狀態與繪製
static int start_menu_open = 0;
static void draw_start_menu() {
    if (!start_menu_open) return;
    int menu_x = 5;
    int menu_y = TASKBAR_Y - 80;
    int menu_w = 140;
    int menu_h = 75;
    // 背景與邊框
    draw_rect(menu_x, menu_y, menu_w, menu_h, COLOR_LIGHT_GRAY);
    draw_rect(menu_x, menu_y, menu_w, 1, COLOR_WHITE);
    draw_rect(menu_x, menu_y, 1, menu_h, COLOR_WHITE);
    draw_rect(menu_x, menu_y + menu_h - 1, menu_w, 1, COLOR_BLACK);
    draw_rect(menu_x + menu_w - 1, menu_y, 1, menu_h, COLOR_BLACK);

    // 列出最多 4 個 App
    directory_t* apps = get_applications();
    int item_y = menu_y + 6;
    for (unsigned i = 0; i < apps->file_count && i < 4; ++i) {
        draw_text(menu_x + 6, item_y, apps->files[i].name, COLOR_BLACK);
        item_y += 16;
    }
}

// 判斷滑鼠點擊是否在圖示上

void desktop_main_loop() {
    static int last_left = 0;
    static int selected_index = -1;
    static int last_click_index = -1;
    static int frames_since_last_click = 1000;

    // 初始化一次完整桌面
    draw_desktop();
    // 游標區塊保存/還原
    static int last_mx = -1, last_my = -1;
    unsigned short bpp = get_screen_bpp();
    int bpp_bytes = (bpp == 16) ? 2 : 1;
    const int CUR_W = 12, CUR_H = 12;
    static unsigned char cursor_backup[12*12*2]; // 足夠 16bpp

    while (1) {
        mouse_state_t mouse;
        mouse_poll(&mouse);

        int mx = mouse.x, my = mouse.y;

        // 還原舊游標區塊
        if (last_mx >= 0 && last_my >= 0) {
            fb_write_region(last_mx, last_my, CUR_W, CUR_H, cursor_backup);
        }

        // 保存新游標底圖
        fb_read_region(mx, my, CUR_W, CUR_H, cursor_backup);

        // 畫滑鼠游標（簡易箭頭）到畫面
        // 白色箭頭
        for (int dy = 0; dy < 10; ++dy) {
            for (int dx = 0; dx <= dy; ++dx) {
                set_pixel(mx + dx, my + dy, COLOR_WHITE);
            }
        }
        // 黑色邊緣
        for (int dy = 0; dy < 10; ++dy) {
            set_pixel(mx + dy, my + dy, COLOR_BLACK);
        }
        last_mx = mx; last_my = my;
        // 顯示 Start 選單（若開啟）
        draw_start_menu();

        int left_edge = mouse.left_button && !last_left; // 按下邊緣
        if (left_edge) {
            // 檢查是否點擊 Start 按鈕
            int btn_y = TASKBAR_Y + 5;
            if (mouse.x >= 5 && mouse.x < 65 && mouse.y >= btn_y && mouse.y < btn_y + 20) {
                start_menu_open = !start_menu_open;
                last_left = mouse.left_button;
                frames_since_last_click = 1000; // reset
                goto loop_end;
            }

            // 如果 Start 開啟，檢查是否點到選單項目
            if (start_menu_open) {
                int menu_x = 5, menu_y = TASKBAR_Y - 80, menu_w = 140, menu_h = 75;
                if (mouse.x >= menu_x && mouse.x < menu_x + menu_w && mouse.y >= menu_y && mouse.y < menu_y + menu_h) {
                    int which = (mouse.y - (menu_y + 6)) / 16; // 粗略命中
                    directory_t* apps = get_applications();
                    if (which >= 0 && (unsigned)which < apps->file_count && which < 4) {
                        desktop_launch_app_by_name(apps->files[which].name);
                        start_menu_open = 0;
                    }
                    last_left = mouse.left_button;
                    goto loop_end;
                } else {
                    // 點在外面就關閉
                    start_menu_open = 0;
                }
            }

            directory_t* desktop_files = get_desktop_files();
            int x = ICON_START_X, y = ICON_START_Y, icons_in_row = 0;
            for (unsigned int i = 0; i < desktop_files->file_count; i++) {
                if (mouse.x >= x && mouse.x < x + ICON_SIZE && mouse.y >= y && mouse.y < y + ICON_SIZE) {
                    // 選取/雙擊判斷
                    if (selected_index != (int)i) {
                        selected_index = (int)i; // 首次點擊 -> 選取
                        frames_since_last_click = 0;
                        last_click_index = (int)i;
                    } else if (frames_since_last_click < 20 && last_click_index == (int)i) {
                        // 簡易雙擊（20 幀內第二次點到同一個）
                        desktop_launch_app_by_name(desktop_files->files[i].name);
                        frames_since_last_click = 1000;
                    } else {
                        frames_since_last_click = 0; // 重置計時
                        last_click_index = (int)i;
                    }
                    // 互動發生：重新繪製整個桌面（狀態改變時才重繪）
                    draw_desktop();
                    break;
                }
                icons_in_row++;
                if (icons_in_row >= ICONS_PER_ROW) {
                    icons_in_row = 0;
                    x = ICON_START_X;
                    y += ICON_SPACING;
                } else {
                    x += ICON_SPACING;
                }
            }
        }
        // 以選取狀態畫出高亮框
        if (selected_index >= 0) {
            int x = ICON_START_X, y = ICON_START_Y, count = 0;
            directory_t* desktop_files = get_desktop_files();
            for (unsigned i = 0; i < desktop_files->file_count; ++i) {
                if ((int)i == selected_index) {
                    // 畫一個淡色外框
                    draw_rect(x - 2, y - 2, ICON_SIZE + 4, 2, COLOR_LIGHT_CYAN);
                    draw_rect(x - 2, y + ICON_SIZE, ICON_SIZE + 4, 2, COLOR_LIGHT_CYAN);
                    draw_rect(x - 2, y - 2, 2, ICON_SIZE + 4, COLOR_LIGHT_CYAN);
                    draw_rect(x + ICON_SIZE, y - 2, 2, ICON_SIZE + 4, COLOR_LIGHT_CYAN);
                    break;
                }
                count++;
                if (count % ICONS_PER_ROW == 0) { x = ICON_START_X; y += ICON_SPACING; } else { x += ICON_SPACING; }
            }
        }

    loop_end:
        last_left = mouse.left_button;
        if (frames_since_last_click < 1000) frames_since_last_click++; // 粗略 frame 計時
        // idle：簡單忙等，避免使用 hlt（尚未啟用中斷）
        for (volatile int d = 0; d < 20000; ++d) { }
    }
}

// 繪製桌面
void draw_desktop() {
    // 優先顯示壁紙（等比置中鋪滿，保留任務欄高度）；若無則使用漸層
    unsigned short W = get_screen_width(); (void)W;
    unsigned short H = get_screen_height();
    const unsigned char* res_ptr = NULL; unsigned int res_len = 0; int drew = -1;
    // 先找 PNG（之後會加入 PNG 解碼），當前先跳過，接著找 BMP
    if (find_resource("wallpaper.bmp", &res_ptr, &res_len) == 0 && res_len > 64) {
        drew = draw_bmp_fit_center_screen(res_ptr, res_len, TASKBAR_HEIGHT);
    }
    if (drew < 0) {
        // 背景漸層（深藍到淺藍）
        for (int y = 0; y < H - TASKBAR_HEIGHT; ++y) {
            unsigned char c = (y % 32) < 16 ? COLOR_BLUE : COLOR_LIGHT_BLUE;
            draw_rect(0, y, W, 1, c);
        }
    }

    // 繪製桌面圖標
    draw_desktop_icons();


    // 繪製任務欄
    draw_taskbar();
}

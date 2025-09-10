// 桌面圖形界面
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


// 桌面圖標位置
#define ICON_SIZE 64
#define ICON_SPACING 80
#define ICONS_PER_ROW 4
#define ICON_START_X 20
#define ICON_START_Y 60

// 任務欄
#define TASKBAR_HEIGHT 30
#define TASKBAR_Y (VGA_HEIGHT - TASKBAR_HEIGHT)

// 繪製桌面圖標
void draw_file_icon(int x, int y, const char* name, file_type_t type) {
    // 繪製圖標背景
    unsigned char icon_color;
    switch (type) {
        case FILE_TYPE_APPLICATION:
            icon_color = COLOR_BLUE;
            break;
        case FILE_TYPE_DIRECTORY:
            icon_color = COLOR_YELLOW;
            break;
        default:
            icon_color = COLOR_LIGHT_GRAY;
            break;
    }
    
    // 繪製圖標方塊
    draw_rect(x, y, ICON_SIZE, ICON_SIZE, icon_color);
    
    // 繪製圖標邊框
    draw_rect(x, y, ICON_SIZE, 2, COLOR_WHITE); // 上邊框
    draw_rect(x, y + ICON_SIZE - 2, ICON_SIZE, 2, COLOR_DARK_GRAY); // 下邊框
    draw_rect(x, y, 2, ICON_SIZE, COLOR_WHITE); // 左邊框
    draw_rect(x + ICON_SIZE - 2, y, 2, ICON_SIZE, COLOR_DARK_GRAY); // 右邊框
    
    // 繪製圖標內容（簡單的符號）
    if (type == FILE_TYPE_APPLICATION) {
        // 應用程式圖標 - 繪製一個簡單的 "A"
        draw_rect(x + 20, y + 10, 24, 4, COLOR_WHITE);
        draw_rect(x + 20, y + 20, 4, 20, COLOR_WHITE);
        draw_rect(x + 40, y + 20, 4, 20, COLOR_WHITE);
        draw_rect(x + 20, y + 30, 24, 4, COLOR_WHITE);
    } else if (type == FILE_TYPE_DIRECTORY) {
        // 目錄圖標 - 繪製一個簡單的資料夾
        draw_rect(x + 10, y + 15, 44, 4, COLOR_WHITE);
        draw_rect(x + 10, y + 19, 44, 30, COLOR_WHITE);
    }
    
    // 繪製檔案名稱（簡化版本）
    // 這裡可以實現更複雜的文字渲染
    // 現在只是繪製一個簡單的線條表示文字
    draw_rect(x + 5, y + ICON_SIZE + 5, ICON_SIZE - 10, 2, COLOR_WHITE);
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

// 繪製任務欄
void draw_taskbar() {
    // 繪製任務欄背景
    draw_rect(0, TASKBAR_Y, VGA_WIDTH, TASKBAR_HEIGHT, COLOR_DARK_GRAY);
    
    // 繪製開始按鈕
    draw_rect(5, TASKBAR_Y + 5, 60, 20, COLOR_BLUE);
    
    // 繪製開始按鈕文字（簡化）
    draw_rect(10, TASKBAR_Y + 10, 50, 2, COLOR_WHITE);
    
    // 繪製時間區域（右側）
    draw_rect(VGA_WIDTH - 80, TASKBAR_Y + 5, 75, 20, COLOR_LIGHT_GRAY);
    
    // 繪製時間文字（簡化）
    draw_rect(VGA_WIDTH - 75, TASKBAR_Y + 10, 65, 2, COLOR_BLACK);
}

// 繪製桌面
void draw_desktop() {
    // 清空螢幕
    draw_rect(0, 0, VGA_WIDTH, VGA_HEIGHT, COLOR_BLACK);
    
    // 繪製桌面背景（漸層效果）
    for (int y = 0; y < VGA_HEIGHT - TASKBAR_HEIGHT; y++) {
        unsigned char color = COLOR_BLACK + (y * 2 / (VGA_HEIGHT - TASKBAR_HEIGHT));
        if (color > COLOR_DARK_GRAY) color = COLOR_DARK_GRAY;
        draw_rect(0, y, VGA_WIDTH, 1, color);
    }
    
    // 繪製桌面圖標
    draw_desktop_icons();
    
    // 繪製任務欄
    draw_taskbar();
    
    // 繪製一些裝飾性元素
    // 繪製桌面標題
    draw_rect(10, 10, 200, 20, COLOR_BLUE);
    draw_rect(15, 15, 190, 2, COLOR_WHITE);
}

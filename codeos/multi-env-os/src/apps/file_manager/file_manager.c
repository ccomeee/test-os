// file_manager.c
#include <file_manager.h>
#include <desktop.h>

#define FM_X 40
#define FM_Y 40
#define FM_WIDTH 240
#define FM_HEIGHT 120

void draw_file_manager_window() {
    // 視窗外框
    draw_rect(FM_X, FM_Y, FM_WIDTH, FM_HEIGHT, COLOR_WHITE);
    // 標題列
    draw_rect(FM_X, FM_Y, FM_WIDTH, 16, COLOR_BLUE);
    // 標題文字（簡化）
    draw_rect(FM_X + 10, FM_Y + 4, 80, 2, COLOR_WHITE);
    // 視窗內容區域
    draw_rect(FM_X + 4, FM_Y + 20, FM_WIDTH - 8, FM_HEIGHT - 24, COLOR_LIGHT_GRAY);
    // TODO: 檔案列表顯示
}

void open_file_manager_window() {
    draw_file_manager_window();
}

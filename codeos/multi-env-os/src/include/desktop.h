// 桌面環境頭檔案
#ifndef DESKTOP_H
#define DESKTOP_H

#include "libc.h"

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

// 檔案類型
typedef enum {
    FILE_TYPE_REGULAR = 1,
    FILE_TYPE_DIRECTORY = 2,
    FILE_TYPE_APPLICATION = 3
} file_type_t;

// 檔案結構
typedef struct {
    char name[32];
    file_type_t type;
    unsigned int size;
    unsigned int data_offset;
    unsigned int permissions;
    unsigned int created_time;
    unsigned int modified_time;
} file_entry_t;

// 目錄結構
typedef struct {
    char name[32];
    unsigned int file_count;
    file_entry_t files[100];
} directory_t;

// 桌面檔案系統
typedef struct {
    directory_t root;
    directory_t desktop;
    directory_t applications;
    directory_t documents;
    directory_t system;
} desktop_filesystem_t;

// 桌面環境函數
void init_desktop_filesystem();
int add_desktop_file(const char* name, file_type_t type, unsigned int size);
int add_application(const char* name, file_type_t type, unsigned int size);
directory_t* get_desktop_files();
directory_t* get_applications();
file_entry_t* find_file(const char* name, directory_t* dir);
desktop_filesystem_t* get_filesystem();

// 桌面繪製函數
void draw_rect(int x, int y, int width, int height, unsigned char color);
void set_pixel(int x, int y, unsigned char color);
void draw_text(int x, int y, const char* text, unsigned char color);
void fb_read_region(int x, int y, int w, int h, void* buf);
void fb_write_region(int x, int y, int w, int h, const void* buf);
void draw_desktop();
void draw_desktop_icons();
void draw_taskbar();
void draw_file_icon(int x, int y, const char* name, file_type_t type);
void desktop_main_loop();

#endif // DESKTOP_H

#ifndef BROWSER_H
#define BROWSER_H

#include <time.h>

#define MAX_URL_LENGTH 2048
#define MAX_COMMAND_LENGTH 4096
#define MAX_TITLE_LENGTH 256
#define MAX_HISTORY_ENTRIES 1000
#define MAX_BOOKMARKS 500

// 定義不同平台的瀏覽器啟動命令
#ifdef _WIN32
    #define BROWSER_OPEN_CMD "start"
#elif defined(__APPLE__)
    #define BROWSER_OPEN_CMD "open"
#else
    #define BROWSER_OPEN_CMD "xdg-open"
#endif

// 啟動瀏覽器，預設或指定網址
int launch_browser(void);
int launch_browser_with_url(const char* url);

// 下載檔案
int download_file(const char* url, const char* output);

// 歷史紀錄操作
int add_history(const char* url);
int list_history(char* buffer, int buf_size);
int clear_history(void);

// 書籤管理
int add_bookmark(const char* url, const char* title);
int list_bookmarks(char* buffer, int buf_size);
int remove_bookmark(const char* url);

// 下載進度回報
int get_download_progress(const char* url);

// URL驗證函數在browser.c內部實現

#endif // BROWSER_H
#include "browser.h"
#include "os.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include <time.h>

#define MAX_URL_LENGTH 2048
#define MAX_COMMAND_LENGTH 4096
#define ALLOWED_PROTOCOLS "http://,https://"

// 定義不同平台的瀏覽器啟動命令
#ifdef _WIN32
    #define BROWSER_OPEN_CMD "start"
#elif defined(__APPLE__)
    #define BROWSER_OPEN_CMD "open"
#else
    #define BROWSER_OPEN_CMD "xdg-open"
#endif

// URL 安全性檢查 - 增強版
static int validate_url(const char* url) {
    if (!url || strlen(url) > MAX_URL_LENGTH) {
        return -1;
    }
    
    // 檢查URL協議
    const char* protocols[] = {"http://", "https://"};
    int valid_protocol = 0;
    for (int i = 0; i < 2; i++) {
        if (strncmp(url, protocols[i], strlen(protocols[i])) == 0) {
            valid_protocol = 1;
            break;
        }
    }
    if (!valid_protocol) return -1;
    
    // 檢查URL中是否包含危險字符
    const char* dangerous_chars = "\\|&;<>()$`\"'*?[]#~=%";
    for (int i = 0; url[i]; i++) {
        if (strchr(dangerous_chars, url[i])) {
            return -1;
        }
    }
    
    // 檢查URL是否包含空格
    if (strchr(url, ' ')) {
        return -1;
    }
    
    return 0;
}

// 預設開啟 Google 首頁，可依需求改傳參數
int launch_browser(void) {
    const char* url = "https://www.google.com";
    char command[MAX_COMMAND_LENGTH];
    
    if (validate_url(url) != 0) {
        fprintf(stderr, "Error: Invalid URL\n");
        return -1;
    }
    
    if (snprintf(command, sizeof(command), "%s \"%s\"", BROWSER_OPEN_CMD, url) >= (int)sizeof(command)) {
        fprintf(stderr, "Error: Command too long\n");
        return -1;
    }
    
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error: Failed to launch browser. Error code: %d\n", result);
        return -1;
    }
    
    return 0;
}

// 支援自訂網址啟動
int launch_browser_with_url(const char* url) {
    char command[MAX_COMMAND_LENGTH];
    if (!url) {
        fprintf(stderr, "Error: URL cannot be NULL\n");
        return -1;
    }
    
    if (validate_url(url) != 0) {
        fprintf(stderr, "Error: Invalid URL: %s\n", url);
        return -1;
    }
    
    // 使用引號包裹URL，防止命令注入
    if (snprintf(command, sizeof(command), "%s \"%s\"", BROWSER_OPEN_CMD, url) >= (int)sizeof(command)) {
        fprintf(stderr, "Error: Command too long\n");
        return -1;
    }
    
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error: Failed to launch browser. Error code: %d\n", result);
        return -1;
    }
    
    // 成功後添加到歷史記錄
    if (add_history(url) != 0) {
        fprintf(stderr, "Warning: Failed to add URL to history\n");
        // 不返回錯誤，因為瀏覽器已經成功啟動
    }
    
    return 0;
}

// 歷史紀錄（簡易以檔案保存）- 增強版
#define HISTORY_FILE "browser_history.txt"
#define MAX_HISTORY_ENTRIES 1000

int add_history(const char* url) {
    if (!url) return -1;
    
    // 驗證URL
    if (validate_url(url) != 0) {
        return -1;
    }
    
    FILE* f = fopen(HISTORY_FILE, "a");
    if (!f) {
        fprintf(stderr, "Error: Could not open history file: %s\n", strerror(errno));
        return -1;
    }
    
    // 添加時間戳
    time_t now = time(NULL);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(f, "[%s] %s\n", timestamp, url);
    fclose(f);
    
    // 檢查歷史記錄是否過長，如果是則保留最新的記錄
    int count = 0;
    char line[MAX_URL_LENGTH + 64]; // 額外空間用於時間戳
    FILE* temp = NULL;
    FILE* orig = NULL;
    
    // 計算歷史記錄數量
    orig = fopen(HISTORY_FILE, "r");
    if (orig) {
        while (fgets(line, sizeof(line), orig) && ++count);
        fclose(orig);
        
        // 如果超過最大數量，則保留最新的記錄
        if (count > MAX_HISTORY_ENTRIES) {
            orig = fopen(HISTORY_FILE, "r");
            temp = fopen("history_temp.txt", "w");
            
            if (!orig || !temp) {
                if (orig) fclose(orig);
                if (temp) fclose(temp);
                return 0; // 不處理錯誤，因為歷史記錄不是關鍵功能
            }
            
            // 跳過舊記錄
            for (int i = 0; i < (count - MAX_HISTORY_ENTRIES); i++) {
                if (!fgets(line, sizeof(line), orig)) break;
            }
            
            // 複製剩餘記錄
            while (fgets(line, sizeof(line), orig)) {
                fputs(line, temp);
            }
            
            fclose(orig);
            fclose(temp);
            
            remove(HISTORY_FILE);
            rename("history_temp.txt", HISTORY_FILE);
        }
    }
    
    return 0;
}

int list_history(char* buffer, int buf_size) {
    if (!buffer || buf_size <= 0) return -1;
    
    FILE* f = fopen(HISTORY_FILE, "r");
    if (!f) {
        buffer[0] = '\0';
        return 0; // 沒有歷史記錄不是錯誤
    }
    
    int total = fread(buffer, 1, buf_size - 1, f);
    buffer[total] = '\0';
    fclose(f);
    return total;
}

int clear_history(void) {
    FILE* f = fopen(HISTORY_FILE, "w");
    if (!f) {
        fprintf(stderr, "Error: Could not clear history: %s\n", strerror(errno));
        return -1;
    }
    fclose(f);
    return 0;
}

// 書籤管理（簡易以檔案保存）- 增強版
#define BOOKMARK_FILE "browser_bookmarks.txt"
#define MAX_TITLE_LENGTH 256
#define MAX_BOOKMARKS 500

int add_bookmark(const char* url, const char* title) {
    if (!url || !title) {
        fprintf(stderr, "Error: URL and title cannot be NULL\n");
        return -1;
    }
    
    // 驗證URL
    if (validate_url(url) != 0) {
        fprintf(stderr, "Error: Invalid URL for bookmark\n");
        return -1;
    }
    
    // 檢查標題長度
    if (strlen(title) > MAX_TITLE_LENGTH) {
        fprintf(stderr, "Error: Bookmark title too long\n");
        return -1;
    }
    
    // 檢查標題中是否包含分隔符
    if (strchr(title, '|')) {
        fprintf(stderr, "Error: Bookmark title cannot contain '|' character\n");
        return -1;
    }
    
    FILE* f = fopen(BOOKMARK_FILE, "a");
    if (!f) {
        fprintf(stderr, "Error: Could not open bookmark file: %s\n", strerror(errno));
        return -1;
    }
    
    // 添加時間戳
    time_t now = time(NULL);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(f, "%s|%s|%s\n", url, title, timestamp);
    fclose(f);
    
    // 檢查書籤數量是否過多
    int count = 0;
    char line[MAX_URL_LENGTH + MAX_TITLE_LENGTH + 64]; // 額外空間用於時間戳
    FILE* temp = NULL;
    FILE* orig = NULL;
    
    // 計算書籤數量
    orig = fopen(BOOKMARK_FILE, "r");
    if (orig) {
        while (fgets(line, sizeof(line), orig) && ++count);
        fclose(orig);
        
        // 如果超過最大數量，提示用戶
        if (count > MAX_BOOKMARKS) {
            fprintf(stderr, "Warning: You have over %d bookmarks. Consider removing some.\n", MAX_BOOKMARKS);
        }
    }
    
    return 0;
}

int list_bookmarks(char* buffer, int buf_size) {
    if (!buffer || buf_size <= 0) {
        return -1;
    }
    
    FILE* f = fopen(BOOKMARK_FILE, "r");
    if (!f) {
        buffer[0] = '\0';
        return 0; // 沒有書籤不是錯誤
    }
    
    int total = fread(buffer, 1, buf_size - 1, f);
    buffer[total] = '\0';
    fclose(f);
    return total;
}

int remove_bookmark(const char* url) {
    if (!url) {
        fprintf(stderr, "Error: URL cannot be NULL\n");
        return -1;
    }
    
    FILE* f = fopen(BOOKMARK_FILE, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open bookmark file: %s\n", strerror(errno));
        return -1;
    }
    
    FILE* temp = fopen("bookmarks_tmp.txt", "w");
    if (!temp) { 
        fprintf(stderr, "Error: Could not create temporary file: %s\n", strerror(errno));
        fclose(f); 
        return -1; 
    }
    
    char line[MAX_URL_LENGTH + MAX_TITLE_LENGTH + 64]; // 額外空間用於時間戳
    int removed = 0;
    
    while (fgets(line, sizeof(line), f)) {
        // 檢查URL是否匹配（只比較到第一個分隔符）
        char* separator = strchr(line, '|');
        if (separator) {
            int url_length = separator - line;
            if (strncmp(line, url, url_length) != 0 || strlen(url) != url_length) {
                fputs(line, temp);
            } else {
                removed = 1;
            }
        } else {
            // 格式不正確的行，保留
            fputs(line, temp);
        }
    }
    
    fclose(f);
    fclose(temp);
    
    if (remove(BOOKMARK_FILE) != 0) {
        fprintf(stderr, "Error: Could not remove original bookmark file: %s\n", strerror(errno));
        return -1;
    }
    
    if (rename("bookmarks_tmp.txt", BOOKMARK_FILE) != 0) {
        fprintf(stderr, "Error: Could not rename temporary file: %s\n", strerror(errno));
        return -1;
    }
    
    return removed ? 0 : -1;
}

// 下載進度回報（改進版，提供詳細進度信息）
int get_download_progress(const char* url) {
    if (!url) {
        fprintf(stderr, "Error: Invalid URL\n");
        return -1;
    }
    
    // 假設下載時會產生 output.tmp 檔案
    char tmp_file[256];
    char url_hash[65] = {0};
    
    // 簡單的URL哈希，用於區分不同URL的下載
    unsigned int hash = 0;
    for (int i = 0; url[i]; i++) {
        hash = hash * 31 + url[i];
    }
    snprintf(url_hash, sizeof(url_hash), "%u", hash);
    
    // 使用URL哈希作為臨時文件名的一部分
    snprintf(tmp_file, sizeof(tmp_file), "download_%s.tmp", url_hash);
    
    FILE* f = fopen(tmp_file, "r");
    if (!f) {
        return 0; // 無進度
    }
    
    // 讀取文件大小信息
    char info_line[256];
    long total_size = 0;
    long current_size = 0;
    
    if (fgets(info_line, sizeof(info_line), f)) {
        sscanf(info_line, "%ld/%ld", &current_size, &total_size);
    }
    
    fclose(f);
    
    // 如果有總大小信息，計算百分比進度
    if (total_size > 0) {
        int progress = (int)((current_size * 100) / total_size);
        return progress;
    }
    
    return 1; // 有進度但無法計算百分比
}

// 安全的下載檔案功能 - 增強版
int download_file(const char* url, const char* output) {
    if (!url || !output) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return -1;
    }
    
    // 驗證URL
    if (validate_url(url) != 0) {
        fprintf(stderr, "Error: Invalid URL\n");
        return -1;
    }
    
    // 驗證輸出文件名
    if (strlen(output) == 0) {
        fprintf(stderr, "Error: Output filename cannot be empty\n");
        return -1;
    }
    
    // 檢查輸出文件名是否包含危險字符
    const char* dangerous_chars = "\\|&;<>()$`\"'*?[]#~=%";
    for (int i = 0; output[i]; i++) {
        if (strchr(dangerous_chars, output[i])) {
            fprintf(stderr, "Error: Output filename contains invalid characters\n");
            return -1;
        }
    }
    
    // 創建URL哈希，用於臨時文件名
    unsigned int hash = 0;
    for (int i = 0; url[i]; i++) {
        hash = hash * 31 + url[i];
    }
    
    char url_hash[65];
    snprintf(url_hash, sizeof(url_hash), "%u", hash);
    
    // 創建臨時文件名，使用系統臨時目錄
    char tmp_dir[256];
#ifdef _WIN32
    const char* temp_env = getenv("TEMP");
    if (!temp_env) temp_env = "."; // 如果無法獲取臨時目錄，使用當前目錄
    snprintf(tmp_dir, sizeof(tmp_dir), "%s", temp_env);
#else
    snprintf(tmp_dir, sizeof(tmp_dir), "/tmp");
#endif
    
    char tmp_file[512];
    snprintf(tmp_file, sizeof(tmp_file), "%s/download_%s.tmp", tmp_dir, url_hash);
    char progress_file[512];
    snprintf(progress_file, sizeof(progress_file), "%s/progress_%s.tmp", tmp_dir, url_hash);
    
    // 使用curl的寫入函數選項來更新進度文件
    char curl_config[MAX_COMMAND_LENGTH];
    
    // 根據不同平台選擇不同的命令格式
#ifdef _WIN32
    snprintf(curl_config, sizeof(curl_config),
        "curl -L --progress-bar --connect-timeout 30 --max-time 3600 "
        "-o \"%s\" "
        "-w \"%%{size_download}/%%{content_length_download}\" "
        "-o \"%s\" \"%s\"", 
        progress_file, output, url);
    
    char command[MAX_COMMAND_LENGTH * 2];
    if (snprintf(command, sizeof(command),
        "echo \"0/0\" > \"%s\" && %s && "
        "type \"%s\" > \"%s\" && del \"%s\"", 
        progress_file, curl_config, progress_file, tmp_file, progress_file) >= (int)sizeof(command)) {
        fprintf(stderr, "Error: Command too long\n");
        return -1;
    }
#else
    snprintf(curl_config, sizeof(curl_config),
        "curl -L --progress-bar --connect-timeout 30 --max-time 3600 "
        "-o \"%s\" "
        "-w '%%{size_download}/%%{content_length_download}' "
        "-o \"%s\" '%s'", 
        progress_file, output, url);
    
    char command[MAX_COMMAND_LENGTH * 2];
    if (snprintf(command, sizeof(command),
        "echo \"0/0\" > \"%s\" && %s && "
        "cat \"%s\" > \"%s\" && rm \"%s\"", 
        progress_file, curl_config, progress_file, tmp_file, progress_file) >= (int)sizeof(command)) {
        fprintf(stderr, "Error: Command too long\n");
        return -1;
    }
#endif
    
    printf("Downloading %s...\n", url);
    printf("Output file: %s\n", output);
    printf("Press Ctrl+C to cancel\n");
    
    // 執行下載命令
    int result = system(command);
    
    if (result != 0) {
        fprintf(stderr, "Error: Download failed. Error code: %d\n", result);
        // 清理臨時文件
        remove(tmp_file);
        remove(progress_file);
        return -1;
    }
    
    printf("Download completed: %s\n", output);
    
    // 下載完成後刪除臨時文件
    remove(tmp_file);
    remove(progress_file);
    
    // 添加到歷史記錄
    char history_entry[MAX_URL_LENGTH + 256];
    snprintf(history_entry, sizeof(history_entry), "download:%s", url);
    add_history(history_entry);
    
    return 0;
}

// 取消下載功能
int cancel_download(const char* url) {
    if (!url) {
        fprintf(stderr, "Error: Invalid URL\n");
        return -1;
    }
    
    // 創建URL哈希，用於臨時文件名
    unsigned int hash = 0;
    for (int i = 0; url[i]; i++) {
        hash = hash * 31 + url[i];
    }
    
    char url_hash[65];
    snprintf(url_hash, sizeof(url_hash), "%u", hash);
    
    // 創建臨時文件名，使用系統臨時目錄
    char tmp_dir[256];
#ifdef _WIN32
    const char* temp_env = getenv("TEMP");
    if (!temp_env) temp_env = "."; // 如果無法獲取臨時目錄，使用當前目錄
    snprintf(tmp_dir, sizeof(tmp_dir), "%s", temp_env);
#else
    snprintf(tmp_dir, sizeof(tmp_dir), "/tmp");
#endif
    
    char tmp_file[512];
    snprintf(tmp_file, sizeof(tmp_file), "%s/download_%s.tmp", tmp_dir, url_hash);
    char progress_file[512];
    snprintf(progress_file, sizeof(progress_file), "%s/progress_%s.tmp", tmp_dir, url_hash);
    
    // 嘗試刪除臨時文件
    remove(tmp_file);
    remove(progress_file);
    
    // 嘗試終止下載進程
#ifdef _WIN32
    system("taskkill /F /IM curl.exe 2>NUL");
#else
    system("pkill -f curl 2>/dev/null");
#endif
    
    printf("Download cancelled: %s\n", url);
    return 0;
}
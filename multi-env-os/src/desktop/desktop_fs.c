// 桌面檔案系統
#include "libc.h"
#include "string.h"

// 檔案系統結構
#define MAX_FILES 100
#define MAX_FILENAME 32
#define MAX_DIRS 20
#define MAX_DIRNAME 32

// 檔案類型
typedef enum {
    FILE_TYPE_REGULAR = 1,
    FILE_TYPE_DIRECTORY = 2,
    FILE_TYPE_APPLICATION = 3
} file_type_t;

// 檔案結構
typedef struct {
    char name[MAX_FILENAME];
    file_type_t type;
    unsigned int size;
    unsigned int data_offset;
    unsigned int permissions;
    unsigned int created_time;
    unsigned int modified_time;
} file_entry_t;

// 目錄結構
typedef struct {
    char name[MAX_DIRNAME];
    unsigned int file_count;
    file_entry_t files[MAX_FILES];
} directory_t;

// 桌面檔案系統
typedef struct {
    directory_t root;
    directory_t desktop;
    directory_t applications;
    directory_t documents;
    directory_t system;
} desktop_filesystem_t;

// 全域檔案系統實例
static desktop_filesystem_t g_fs;

// 初始化桌面檔案系統
void init_desktop_filesystem() {
    // 初始化根目錄
    strcpy(g_fs.root.name, "/");
    g_fs.root.file_count = 0;
    
    // 初始化桌面目錄
    strcpy(g_fs.desktop.name, "Desktop");
    g_fs.desktop.file_count = 0;
    
    // 初始化應用程式目錄
    strcpy(g_fs.applications.name, "Applications");
    g_fs.applications.file_count = 0;
    
    // 初始化文件目錄
    strcpy(g_fs.documents.name, "Documents");
    g_fs.documents.file_count = 0;
    
    // 初始化系統目錄
    strcpy(g_fs.system.name, "System");
    g_fs.system.file_count = 0;
    
    // 添加預設桌面檔案
    add_desktop_file("Terminal", FILE_TYPE_APPLICATION, 0);
    add_desktop_file("Text Editor", FILE_TYPE_APPLICATION, 0);
    add_desktop_file("Browser", FILE_TYPE_APPLICATION, 0);
    add_desktop_file("File Manager", FILE_TYPE_APPLICATION, 0);
    
    // 添加預設應用程式
    add_application("Python Environment", FILE_TYPE_APPLICATION, 0);
    add_application("C++ Environment", FILE_TYPE_APPLICATION, 0);
    add_application("Node.js Environment", FILE_TYPE_APPLICATION, 0);
}

// 添加桌面檔案
int add_desktop_file(const char* name, file_type_t type, unsigned int size) {
    if (g_fs.desktop.file_count >= MAX_FILES) {
        return -1; // 目錄已滿
    }
    
    file_entry_t* file = &g_fs.desktop.files[g_fs.desktop.file_count];
    strcpy(file->name, name);
    file->type = type;
    file->size = size;
    file->data_offset = 0;
    file->permissions = 0755;
    file->created_time = 0; // 簡化，不實現時間
    file->modified_time = 0;
    
    g_fs.desktop.file_count++;
    return 0;
}

// 添加應用程式
int add_application(const char* name, file_type_t type, unsigned int size) {
    if (g_fs.applications.file_count >= MAX_FILES) {
        return -1; // 目錄已滿
    }
    
    file_entry_t* file = &g_fs.applications.files[g_fs.applications.file_count];
    strcpy(file->name, name);
    file->type = type;
    file->size = size;
    file->data_offset = 0;
    file->permissions = 0755;
    file->created_time = 0;
    file->modified_time = 0;
    
    g_fs.applications.file_count++;
    return 0;
}

// 獲取桌面檔案列表
directory_t* get_desktop_files() {
    return &g_fs.desktop;
}

// 獲取應用程式列表
directory_t* get_applications() {
    return &g_fs.applications;
}

// 查找檔案
file_entry_t* find_file(const char* name, directory_t* dir) {
    for (unsigned int i = 0; i < dir->file_count; i++) {
        if (strcmp(dir->files[i].name, name) == 0) {
            return &dir->files[i];
        }
    }
    return NULL;
}

// 獲取檔案系統
desktop_filesystem_t* get_filesystem() {
    return &g_fs;
}

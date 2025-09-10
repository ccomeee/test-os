#include "process.h"
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// 進程表，用於追蹤系統中的進程
#define MAX_PROCESSES 1024
static struct {
    int pid;
    int status;
    int exit_code;
    int is_active;
    void *thread_data;
} process_table[MAX_PROCESSES];

// 初始化進程表
static int process_table_initialized = 0;
static void init_process_table(void) {
    if (process_table_initialized) return;
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid = i;
        process_table[i].status = 0;
        process_table[i].exit_code = 0;
        process_table[i].is_active = 0;
        process_table[i].thread_data = NULL;
    }
    process_table_initialized = 1;
}

// 分配一個新的進程ID
static int allocate_pid(void) {
    if (!process_table_initialized) init_process_table();
    
    for (int i = 1; i < MAX_PROCESSES; i++) { // 從1開始，0保留給kernel
        if (!process_table[i].is_active) {
            process_table[i].is_active = 1;
            process_table[i].status = 1; // 運行中
            process_table[i].exit_code = 0;
            return i;
        }
    }
    return -1; // 沒有可用的PID
}

// 創建一個新進程
int fork(void) {
    int pid = allocate_pid();
    if (pid < 0) {
        errno = EAGAIN;
        return -1;
    }
    
    // 在實際系統中，這裡應該複製當前進程的內存空間和狀態
    // 但在這個簡化版本中，我們只是分配一個新的PID
    
    return pid;
}

// 執行一個新程序
int exec(const char *path, char *const argv[]) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }
    
    // 在實際系統中，這裡應該加載並執行指定的程序
    // 但在這個簡化版本中，我們只是檢查路徑是否有效
    
    FILE *file = fopen(path, "r");
    if (!file) {
        errno = ENOENT;
        return -1;
    }
    fclose(file);
    
    // 假設執行成功
    return 0;
}

// 等待子進程結束
int wait(int *status) {
    if (!process_table_initialized) init_process_table();
    
    // 在實際系統中，這裡應該暫停當前進程直到有子進程結束
    // 但在這個簡化版本中，我們只是檢查是否有已結束的子進程
    
    for (int i = 1; i < MAX_PROCESSES; i++) {
        if (process_table[i].is_active && process_table[i].status == 0) { // 已結束
            int exit_code = process_table[i].exit_code;
            process_table[i].is_active = 0; // 釋放PID
            
            if (status) *status = exit_code;
            return i; // 返回結束的進程ID
        }
    }
    
    errno = ECHILD;
    return -1; // 沒有子進程
}

// 創建一個新線程
int pthread_create(void **thread, void *attr, void *(*start_routine)(void*), void *arg) {
    if (!start_routine) {
        errno = EINVAL;
        return -1;
    }
    
    int pid = allocate_pid();
    if (pid < 0) {
        errno = EAGAIN;
        return -1;
    }
    
    // 在實際系統中，這裡應該創建一個新線程並執行start_routine
    // 但在這個簡化版本中，我們只是分配一個新的PID
    
    if (thread) *thread = (void*)(intptr_t)pid;
    
    // 假設創建成功
    return 0;
}

// 結束當前線程
void pthread_exit(void *retval) {
    // 在實際系統中，這裡應該結束當前線程並設置返回值
    // 但在這個簡化版本中，我們不做任何事情
}
#include "cpp_env.h"
#include "stdio.h"   // 你自己的 stub
#include "stdlib.h"  // 你自己的 stub

int is_cpp_installed(void) {
    // system() stub：直接 return -1 或你自定義的行為
    return -1;
}
void install_cpp(void) {
    printf("正在下載並安裝 MinGW-w64 (g++)...\n");
    // system() stub：不能實際執行
}
void setup_cpp_env(void) {
    if (!is_cpp_installed()) {
        install_cpp();
    } else {
        printf("C++ 編譯器已安裝。\n");
    }
}
void launch_cpp_env(void) {
    printf("啟動 C++ 編譯環境...\n");
    // system() stub：不能實際執行
}
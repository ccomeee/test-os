#ifndef PNG_H
#define PNG_H
#include <libc.h>

// 畫出 PNG（非交錯 8-bit RGB/RGBA）等比置中鋪滿，bottom_reserved 保留底部像素高度
// 成功回傳 0，失敗回傳負值
int draw_png_fit_center_screen(const unsigned char* data, unsigned int len, int bottom_reserved);

#endif // PNG_H

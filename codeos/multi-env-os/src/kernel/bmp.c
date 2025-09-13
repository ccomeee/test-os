#include <libc.h>
#include <video_mode.h>

// 解析簡化 BMP：支援 24-bit BGR 以及 16-bit RGB565；不支援壓縮
// data 指向整個 BMP 檔，len 為長度
// 會將圖像左上角貼到 (dst_x,dst_y)，若超出畫面會裁切

#pragma pack(push,1)
typedef struct {
    unsigned short bfType;      // 'BM'
    unsigned int   bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int   bfOffBits;   // 位圖資料偏移
} BMPFileHeader;

typedef struct {
    unsigned int   biSize;      // 40
    int            biWidth;
    int            biHeight;    // 正數：自下而上；負數：自上而下
    unsigned short biPlanes;    // 1
    unsigned short biBitCount;  // 24 或 16
    unsigned int   biCompression; // 0 = BI_RGB(無壓縮)
    unsigned int   biSizeImage;
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)

static inline void put_px(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    unsigned short bpp = get_screen_bpp();
    unsigned short pitch = get_screen_pitch();
    unsigned short W = get_screen_width();
    unsigned short H = get_screen_height();
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    unsigned char* fb = get_framebuffer_ptr();
    if (bpp == 16) {
        // 5-6-5
        unsigned short R = (r >> 3) & 0x1F;
        unsigned short G = (g >> 2) & 0x3F;
        unsigned short B = (b >> 3) & 0x1F;
        unsigned short px = (R << 11) | (G << 5) | B;
        ((unsigned short*)(fb + y * pitch))[x] = px;
    } else if (bpp == 8) {
        // 簡化：轉灰度
        unsigned char gray = (unsigned char)((r*30 + g*59 + b*11)/100);
        fb[y * pitch + x] = gray;
    }

}

// 將 BMP 等比縮放並置中貼到畫面；bottom_reserved 用於保留底部（例如 taskbar）
int draw_bmp_fit_center_screen(const unsigned char* data, unsigned int len, int bottom_reserved) {
    if (!data || len < sizeof(BMPFileHeader) + sizeof(BMPInfoHeader)) return -1;
    const BMPFileHeader* fh = (const BMPFileHeader*)data;
    if (fh->bfType != 0x4D42) return -2; // 'BM'
    const BMPInfoHeader* ih = (const BMPInfoHeader*)(data + sizeof(BMPFileHeader));
    if (ih->biCompression != 0 || (ih->biBitCount != 24 && ih->biBitCount != 16)) return -3;
    int srcW = ih->biWidth;
    int srcH = ih->biHeight;
    int topDown = 0;
    if (srcH < 0) { srcH = -srcH; topDown = 1; }
    const unsigned char* pixels = data + fh->bfOffBits;
    if (len < fh->bfOffBits) return -4;

    unsigned short dstW = get_screen_width();
    unsigned short dstH = get_screen_height();
    if (bottom_reserved > 0 && bottom_reserved < dstH) dstH -= (unsigned short)bottom_reserved;

    // 計算等比縮放
    double scaleX = (double)dstW / (double)srcW;
    double scaleY = (double)dstH / (double)srcH;
    double scale = (scaleX < scaleY) ? scaleX : scaleY;
    int outW = (int)(srcW * scale);
    int outH = (int)(srcH * scale);
    if (outW <= 0 || outH <= 0) return -5;
    int offX = ((int)dstW - outW) / 2;
    int offY = ((int)dstH - outH) / 2;

    if (ih->biBitCount == 24) {
        unsigned int rowSize = ((srcW * 3 + 3) & ~3);
        for (int y = 0; y < outH; ++y) {
            // 最近鄰：對應到來源列
            int srcY = (int)(y / scale);
            int srcRow = topDown ? srcY : (srcH - 1 - srcY);
            const unsigned char* line = pixels + srcRow * rowSize;
            for (int x = 0; x < outW; ++x) {
                int srcX = (int)(x / scale);
                const unsigned char* p = &line[srcX * 3];
                unsigned char b = p[0], g = p[1], r = p[2];
                put_px(offX + x, offY + y, r, g, b);
            }
        }
    } else if (ih->biBitCount == 16) {
        unsigned int rowSize = ((srcW * 2 + 3) & ~3);
        for (int y = 0; y < outH; ++y) {
            int srcY = (int)(y / scale);
            int srcRow = topDown ? srcY : (srcH - 1 - srcY);
            const unsigned short* line = (const unsigned short*)(pixels + srcRow * rowSize);
            for (int x = 0; x < outW; ++x) {
                int srcX = (int)(x / scale);
                unsigned short px = line[srcX];
                unsigned char r = (unsigned char)(((px >> 11) & 0x1F) << 3);
                unsigned char g = (unsigned char)(((px >> 5) & 0x3F) << 2);
                unsigned char b = (unsigned char)((px & 0x1F) << 3);
                put_px(offX + x, offY + y, r, g, b);
            }
        }
    }
    return 0;
}

int draw_bmp_onto_screen(const unsigned char* data, unsigned int len, int dst_x, int dst_y) {
    if (!data || len < sizeof(BMPFileHeader) + sizeof(BMPInfoHeader)) return -1;
    const BMPFileHeader* fh = (const BMPFileHeader*)data;
    if (fh->bfType != 0x4D42) return -2; // 'BM'
    const BMPInfoHeader* ih = (const BMPInfoHeader*)(data + sizeof(BMPFileHeader));
    if (ih->biCompression != 0 || (ih->biBitCount != 24 && ih->biBitCount != 16)) return -3;
    int width = ih->biWidth;
    int height = ih->biHeight;
    int topDown = 0;
    if (height < 0) { height = -height; topDown = 1; }
    const unsigned char* pixels = data + fh->bfOffBits;
    if (len < fh->bfOffBits) return -4;

    if (ih->biBitCount == 24) {
        // 每列對齊到 4 bytes
        unsigned int rowSize = ((width * 3 + 3) & ~3);
        for (int row = 0; row < height; ++row) {
            int srcRow = topDown ? row : (height - 1 - row);
            const unsigned char* line = pixels + srcRow * rowSize;
            for (int col = 0; col < width; ++col) {
                unsigned char b = line[col*3 + 0];
                unsigned char g = line[col*3 + 1];
                unsigned char r = line[col*3 + 2];
                put_px(dst_x + col, dst_y + row, r, g, b);
            }
        }
    } else if (ih->biBitCount == 16) {
        // 假設來源為 565
        unsigned int rowSize = ((width * 2 + 3) & ~3);
        for (int row = 0; row < height; ++row) {
            int srcRow = topDown ? row : (height - 1 - row);
            const unsigned short* line = (const unsigned short*)(pixels + srcRow * rowSize);
            for (int col = 0; col < width; ++col) {
                unsigned short px = line[col];
                unsigned char r = (unsigned char)(((px >> 11) & 0x1F) << 3);
                unsigned char g = (unsigned char)(((px >> 5) & 0x3F) << 2);
                unsigned char b = (unsigned char)((px & 0x1F) << 3);
                put_px(dst_x + col, dst_y + row, r, g, b);
            }
        }
    }
    return 0;
}

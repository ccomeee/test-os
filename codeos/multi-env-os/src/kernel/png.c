// 最小 PNG 解碼器（里程碑 B 第一版）
// 限制：
// - 僅支援 8-bit、非交錯（interlace=0）
// - 色彩類型 2(RGB) 與 6(RGBA)
// - zlib 僅支援 stored blocks (BTYPE=00)（請用無壓縮 PNG：compression-level=0）
// - 濾波僅支援 Filter 0(None)（請將 filter 設為 0）

#include <libc.h>
#include <video_mode.h>
#include <png.h>

static inline void put_px(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    unsigned short bpp = get_screen_bpp();
    unsigned short pitch = get_screen_pitch();
    unsigned short W = get_screen_width();
    unsigned short H = get_screen_height();
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    unsigned char* fb = get_framebuffer_ptr();
    if (bpp == 16) {
        unsigned short R = (r >> 3) & 0x1F;
        unsigned short G = (g >> 2) & 0x3F;
        unsigned short B = (b >> 3) & 0x1F;
        unsigned short px = (R << 11) | (G << 5) | B;
        ((unsigned short*)(fb + y * pitch))[x] = px;
    } else if (bpp == 8) {
        unsigned char gray = (unsigned char)((r*30 + g*59 + b*11)/100);
        fb[y * pitch + x] = gray;
    }
}

static unsigned int read_be32(const unsigned char* p){
    return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
}

// 簡化 zlib inflate：僅支援 stored blocks (BTYPE=00)
// 將輸入 zlib stream 解到 out 緩衝（容量 out_cap），回傳解出位元組數，或負值錯誤
static int zlib_inflate_stored_only(const unsigned char* in, unsigned int in_len, unsigned char* out, unsigned int out_cap) {
    if (in_len < 2) return -1; // zlib header 2 bytes（略過）
    unsigned int i = 2;
    unsigned int o = 0;
    while (i < in_len) {
        if (i+1 >= in_len) return -2;
        unsigned char bfinal = in[i] & 1;
        unsigned char btype  = (in[i] >> 1) & 3;
        i++;
        if (btype == 0) {
            // stored: 對齊到下一個位元組邊界已在 zlib 中處理，這裡直接讀 LEN/NLEN
            if (i + 4 > in_len) return -3;
            unsigned int LEN  = in[i] | (in[i+1]<<8);
            unsigned int NLEN = in[i+2] | (in[i+3]<<8);
            i += 4;
            if ((LEN ^ 0xFFFF) != NLEN) return -4;
            if (i + LEN > in_len) return -5;
            if (o + LEN > out_cap) return -6;
            for (unsigned int k=0;k<LEN;k++) out[o++] = in[i++];
            if (bfinal) break;
        } else {
            // 不支援壓縮/動態/固定霍夫曼
            return -7;
        }
    }
    return (int)o;
}

int draw_png_fit_center_screen(const unsigned char* data, unsigned int len, int bottom_reserved) {
    if (!data || len < 8) return -1;
    // PNG signature
    static const unsigned char sig[8] = {137,80,78,71,13,10,26,10};
    for (int i=0;i<8;i++) if (data[i]!=sig[i]) return -2;
    unsigned int off = 8;
    unsigned int width=0, height=0; unsigned char bit_depth=0, color_type=0, interlace=0;
    // 收集所有 IDAT 連續拼接
    const unsigned char* idat_start = 0; unsigned int idat_total = 0;
    // 粗略：第一次遇到 IDAT 記起來起點；遇到後續 IDAT 只累加長度（要求 IDAT 彼此相鄰）
    //（常見編碼會相鄰；若不相鄰則本簡化版不支援）
    while (off + 8 <= len) {
        unsigned int clen = read_be32(data+off); off+=4;
        if (off + 4 > len) return -3;
        unsigned int ctype = read_be32(data+off); off+=4;
        if (off + clen + 4 > len) return -4; // 含 CRC
        if (ctype == 0x49484452) { // IHDR
            if (clen < 13) return -5;
            width  = read_be32(data+off);
            height = read_be32(data+off+4);
            bit_depth = data[off+8];
            color_type = data[off+9];
            interlace = data[off+12];
            if (bit_depth != 8) return -6;
            if (!(color_type==2 || color_type==6)) return -7;
            if (interlace != 0) return -8;
        } else if (ctype == 0x49444154) { // IDAT
            if (!idat_start) idat_start = data+off;
            idat_total += clen;
        } else if (ctype == 0x49454E44) { // IEND
            break;
        }
        off += clen + 4; // 跳過 data + CRC
    }
    if (!idat_start || idat_total==0) return -9;

    // 解 zlib -> raw scanlines（每行前 1 byte filter）
    // 目標緩衝大小：height*(1 + bytes_per_pixel*width)
    int bpp = (color_type==2)?3:4;
    unsigned int stride = 1 + (unsigned int)bpp * width;
    unsigned int out_cap = stride * height;
    // 注意：在 freestanding 環境下避免大堆疊，使用靜態區域（限制上限）
    static unsigned char inflated[1024*768*4 + 1024]; // 保障夠用（大概）
    if (out_cap > sizeof(inflated)) return -10;
    int out_len = zlib_inflate_stored_only(idat_start, idat_total, inflated, sizeof(inflated));
    if (out_len < 0 || (unsigned int)out_len < out_cap) return -11; // 不足

    // 僅支援 Filter 0(None)
    for (unsigned int y=0; y<height; ++y) {
        unsigned char* row = inflated + y*stride;
        if (row[0] != 0) return -12; // 非 None
    }

    // Fit-Center 繪製
    unsigned short dstW = get_screen_width();
    unsigned short dstH = get_screen_height();
    if (bottom_reserved > 0 && bottom_reserved < dstH) dstH -= (unsigned short)bottom_reserved;
    double scaleX = (double)dstW / (double)width;
    double scaleY = (double)dstH / (double)height;
    double scale = (scaleX < scaleY) ? scaleX : scaleY;
    int outW = (int)(width * scale);
    int outH = (int)(height * scale);
    if (outW <= 0 || outH <= 0) return -13;
    int offX = ((int)dstW - outW) / 2;
    int offY = ((int)dstH - outH) / 2;

    for (int y=0; y<outH; ++y) {
        int srcY = (int)(y / scale);
        const unsigned char* row = inflated + srcY*stride + 1; // 跳過 filter byte
        for (int x=0; x<outW; ++x) {
            int srcX = (int)(x / scale);
            const unsigned char* px = row + srcX*bpp;
            unsigned char r = px[0], g = px[1], b = px[2];
            // 忽略 alpha（色彩類型 6）
            put_px(offX + x, offY + y, r, g, b);
        }
    }
    return 0;
}

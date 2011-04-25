#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <memory.h>

#include "yappy.h"

typedef unsigned char ui8;
typedef unsigned short ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;
 
static ui8 maps[32][16];
static size_t infos[256];
 
void Yappy_FillTables() {
 
    memset(&maps[0][0], 0, sizeof(maps));
    ui64 step = 1 << 16;
 
    for (size_t i = 0; i < 16; ++i) {
        ui64 value = 65535;
        step = (step * 67537) >> 16;
        while(value < (29UL << 16)) {
           maps[value >> 16][i] = 1;
           value = (value * step) >> 16;
        }
    }
 
    int cntr = 0;
    for (size_t i = 0; i < 29; ++i) {
        for (size_t j = 0; j < 16; ++j) {
            if (maps[i][j]) {
                infos[32 + cntr] = i + 4 + (j << 8);
                maps[i][j] = 32 + cntr;
                ++cntr;
            } else {
                if (i == 0)
                   throw("i == 0");
                maps[i][j] = maps[i - 1][j];
            }
        }
    }
    if (cntr != 256 - 32) {
       throw("init error");
    }
}
 
void inline Copy(const ui8 *data, ui8 *to) {
    _mm_storeu_si128((__m128i *)(to), _mm_loadu_si128((const __m128i *)(data)));
}
 
ui8 *Yappy_UnCompress(const ui8 *data, const ui8 *end, ui8 *to) {
    const ui8 *start = to;
    while(data < end) {
        size_t index = data[0];
        if (index < 32) {
            Copy(data + 1, to);
            if (index > 15) {
                Copy(data + 17, to + 16);
            }
            to += index + 1;
            data += index + 2;
        } else {
            size_t info = infos[index];
            size_t length = info & 0x00ff;
            size_t offset = (info & 0xff00) + size_t(data[1]);
 
            Copy(to - offset, to);
            if (length > 16) {
                Copy(to - offset + 16, to + 16);
            }
            to += length;
            data += 2;
        }
   }
   return to;
}
 
int inline Match(const ui8 *data, size_t i, size_t j, size_t size) {
    if (*(ui32 *)(data + i) != *(ui32 *)(data + j))
        return 0;
    size_t k = 4;
    size_t bound = i - j;
    bound = bound > size ? size : bound;
    bound = bound > 32 ? 32 : bound;
    for (;k < bound && data[i + k] == data[j + k]; ++k);
    return k < bound ? k : bound;
}
 
 
ui64 inline Hash(ui64 value) {
    return ((value * 912367421UL) >> 24) & 4095;
}
 
 
void inline Link(size_t *hashes, size_t *nodes, size_t i, const ui8 *data) {
   size_t &hashValue = hashes[Hash(*(const ui32 *)(data + i))];
   nodes[i & 4095] = hashValue;
   hashValue = i;
}
 
 
ui8 *Yappy_Compress(const ui8 *data, ui8 *to, size_t len, int level) {
 
    size_t hashes[4096];
    size_t nodes[4096];
    ui8 end = 0xff;
    ui8 *optr = &end;
 
    for (size_t i = 0; i < 4096; ++i) {
        hashes[i] = size_t(-1);
    }
 
    for (size_t i = 0; i < len;) {
        ui8 coded = data[i];
        Link(hashes, nodes, i, data);
 
        size_t bestMatch = 3;
        ui16 bestCode = 0;
 
        size_t ptr = i;
        int tries = 0;
 
        while(1) {
            size_t newPtr = nodes[ptr & 4095];
            if (newPtr >= ptr || i - newPtr >= 4095 || tries > level) {
                break;
            }
            ptr = newPtr;
            size_t match = Match(data, i, ptr, len - i);
 
            if (bestMatch < match) {
                ui8 code = maps[match - 4][(i - ptr) >> 8];
                match = infos[code] & 0xff;
 
                if (bestMatch < match) {
                    bestMatch = match;
                    bestCode = code + (((i - ptr) & 0xff) << 8);
                    if (bestMatch == 32)
                        break;
                }
            }
 
            tries += match > 3;
        }
 
        if (optr[0] > 30) {
            optr = &end;
        }
 
        if (bestMatch > 3) {
            *(ui16 *)to = bestCode;
 
            for (size_t k = 1; k < bestMatch; ++k)
               Link(hashes, nodes, i + k, data);
 
            i += bestMatch;
            to += 2;
            optr = &end;
        } else {
            if (optr[0] == 0xff) {
               optr = to;
               optr[0] = 0xff;
               ++to;
            }
            ++optr[0];
            to[0] = coded;
            ++to;
            ++i;
        }
    }
    return to;
}

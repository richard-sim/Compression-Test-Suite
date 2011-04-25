#ifndef __YAPPY_H__
#define __YAPPY_H__
 
void Yappy_FillTables();

unsigned char *Yappy_UnCompress(const unsigned char *data, const unsigned char *end, unsigned char *to);
unsigned char *Yappy_Compress(const unsigned char *data, unsigned char *to, size_t len, int level = 10);

#endif // __YAPPY_H__
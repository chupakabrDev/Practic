#pragma once

#define N1 1
#define M1 16
#define N2 4
#define M2 4

typedef char SmallBlock[N1][M1];
typedef SmallBlock LargeBlock[N2][M2];

#define LARGE_BLOCK_SIZE sizeof(LargeBlock)

LargeBlock** readFile(const char *filename);

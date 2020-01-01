
#ifndef MBEAN_H
#define MBEAN_H

#define MBEAN_GRID_W 10
#define MBEAN_GRID_H 15

#define MBEAN_FLAG_SETTLED  0x01

#include <stdint.h>

#define mbean_drop( g, b, col ) \
   (g)->grid[col][0] = b;

/***
 * grid[X][Y]
 *
 * |0 1 2 3 4|X
 * |1        |
 * |2        |
 * |3        |
 * |4        |
 * |---------|
 *  Y
 */

struct mbean_grid {
   uint8_t flags;
   int8_t grid[MBEAN_GRID_W][MBEAN_GRID_H];
};

void mbean_iter( struct mbean_grid* g );

#endif /* MBEAN_H */


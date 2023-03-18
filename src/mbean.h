
#ifndef MBEAN_H
#define MBEAN_H

#ifndef MBEAN_BEAN_W
#  define MBEAN_BEAN_W 8
#endif /* !MBEAN_BEAN_W */

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

struct MBEAN_DATA {
   uint8_t flags;
   struct RETROCON con;
   int8_t grid[MBEAN_GRID_W][MBEAN_GRID_H];
};

void mbean_iter( struct MBEAN_DATA* g );

#endif /* MBEAN_H */


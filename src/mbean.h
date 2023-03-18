
#ifndef MBEAN_H
#define MBEAN_H

#ifndef MBEAN_BEAN_W
#  define MBEAN_BEAN_W 8
#endif /* !MBEAN_BEAN_W */

#ifndef MBEAN_TICK_WAIT
#  define MBEAN_TICK_WAIT 10
#endif /* !MBEAN_TICK_WAIT */

#define MBEAN_GRID_W 10
#define MBEAN_GRID_H 15

#define MBEAN_GRID_W_PX ((MBEAN_GRID_W) * (MBEAN_BEAN_W))
#define MBEAN_GRID_H_PX ((MBEAN_GRID_H) * (MBEAN_BEAN_W))

#define MBEAN_GRID_X_PX ((retroflat_screen_w() / 2) - ((MBEAN_GRID_W_PX) / 2))
#define MBEAN_GRID_Y_PX ((retroflat_screen_h() / 2) - ((MBEAN_GRID_H_PX) / 2))

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
   size_t wait;
};

void mbean_iter( struct MBEAN_DATA* g );

#endif /* MBEAN_H */


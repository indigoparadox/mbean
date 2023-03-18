
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
   int8_t drops[2];
   size_t drops_sz;
   int8_t drops_rot;
   int8_t drops_x;
   int8_t drops_y;
   size_t wait;
};

void mbean_iter( struct MBEAN_DATA* g );

void mbean_drop( struct MBEAN_DATA* g, int8_t x, int8_t y );

void mbean_plop_drops( struct MBEAN_DATA* g );

#ifdef MBEAN_C
int8_t MAUG_CONST g_mbean_drop_rot_x[4] = { 1, 0, -1, 0 };
int8_t MAUG_CONST g_mbean_drop_rot_y[4] = { 0, 1, 0, -1 };
#else
extern MAUG_CONST int8_t g_mbean_drop_rot_x[4];
extern MAUG_CONST int8_t g_mbean_drop_rot_y[4];
#endif /* MBEAN_C */

#endif /* MBEAN_H */


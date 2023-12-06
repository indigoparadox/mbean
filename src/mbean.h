
#ifndef MBEAN_H
#define MBEAN_H

#ifndef MBEAN_BEAN_W
#  define MBEAN_BEAN_W 8
#endif /* !MBEAN_BEAN_W */

#ifndef MBEAN_TICK_WAIT
#  define MBEAN_TICK_WAIT 10
#endif /* !MBEAN_TICK_WAIT */

#ifndef MBEAN_GC_NODES_SZ_MAX
#  define MBEAN_GC_NODES_SZ_MAX 20
#endif /* !MBEAN_GC_NODES_SZ_MAX */

#define MBEAN_GRID_W 10
#define MBEAN_GRID_H 15

#define MBEAN_GRID_W_PX ((MBEAN_GRID_W) * (MBEAN_BEAN_W))
#define MBEAN_GRID_H_PX ((MBEAN_GRID_H) * (MBEAN_BEAN_W))

#define MBEAN_GRID_X_PX ((retroflat_screen_w() / 2) - ((MBEAN_GRID_W_PX) / 2))
#define MBEAN_GRID_Y_PX ((retroflat_screen_h() / 2) - ((MBEAN_GRID_H_PX) / 2))

#define MBEAN_FLAG_SETTLED  0x01

#define MBEAN_FLAG_INIT_DONE 0x02

/* Flag for data->flags indicating rotate key was hit last frame. */
#define MBEAN_FLAG_ROT_LAST 0x04

/* Flag for data->flags indicating a drop was just placed last iter. */
#define MBEAN_FLAG_PLACED_LAST 0x05

#define MBEAN_TILE_PURGE -1

#define MBEAN_WAIT_SKIP -1

#define MBEAN_SND_CHANNEL 0

#define MBEAN_SND_CYCLES 10

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
   int8_t probed[MBEAN_GRID_W][MBEAN_GRID_H];
   int8_t grid[MBEAN_GRID_W][MBEAN_GRID_H];
   int8_t drops[2];
   size_t drops_sz;
   /* Current dropping bean pair rotation. */
   int8_t drops_rot;
   int8_t drops_x;
   int8_t drops_y;
   int16_t wait;
   uint16_t snd_cycles_left;
};

struct MBEAN_GC_NODE {
   int8_t x;
   int8_t y;
   int8_t bean;
   struct MBEAN_GC_NODE* neighbors[4];
};

void mbean_iter( struct MBEAN_DATA* g );

void mbean_drop( struct MBEAN_DATA* g, int8_t x, int8_t y );

void mbean_place_drop_tiles( struct MBEAN_DATA* g );

void mbean_rotate_drops( struct MBEAN_DATA* g );

void mbean_move_drops_x( struct MBEAN_DATA* g, int8_t x_m );

struct MBEAN_GC_NODE* mbean_gc_probe(
   struct MBEAN_DATA* g, int8_t x, int8_t y,
   struct MBEAN_GC_NODE* nodes, size_t* nodes_sz );

void mbean_gc( struct MBEAN_DATA* g );

#ifdef MBEAN_C
int8_t MAUG_CONST gc_mbean_drop_rot_x[4] = { 1, 0, -1, 0 };
int8_t MAUG_CONST gc_mbean_drop_rot_y[4] = { 0, 1, 0, -1 };
RETROFLAT_COLOR g_mbean_colors[5];
#else
extern MAUG_CONST int8_t gc_mbean_drop_rot_x[4];
extern MAUG_CONST int8_t gc_mbean_drop_rot_y[4];
extern RETROFLAT_COLOR g_mbean_colors[5];
#endif /* MBEAN_C */

#endif /* MBEAN_H */


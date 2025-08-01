
#ifndef MBEAN_H
#define MBEAN_H

#include <maug.h>
#include <retrofnt.h>
#include <retrogui.h>
#include <retrowin.h>
#include <retrocon.h>
#include <retrosnd.h>

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
#define MBEAN_FLAG_PLACED_LAST 0x08

#define MBEAN_TILE_PURGE -1

#define MBEAN_WAIT_SKIP -1

#define MBEAN_SND_CHANNEL 0

#define MBEAN_SND_CYCLES 1

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
   retroflat_dir4_t drops_rot;
   int8_t drops_x;
   int8_t drops_y;
   int16_t score;
   int16_t wait;
   uint16_t snd_cycles_left;
   MAUG_MHANDLE font_h;
};

struct MBEAN_GC_NODE {
   int8_t x;
   int8_t y;
   int8_t bean;
   struct MBEAN_GC_NODE* neighbors[4];
};

/**
 * \brief Mark spaces that have beans but did not previously have beans as due
 *        to be redrawn.
 */
void mbean_erase_bean_drops( struct MBEAN_DATA* g );

/**
 * \return MERROR_USR if bean rotation would collide with another bean, or
 *         MERROR_OK otherwise.
 */
MERROR_RETVAL mbean_check_collision(
   struct MBEAN_DATA* g,
   int8_t pivot_x, int8_t pivot_y, retroflat_dir4_t pivot_r );

void mbean_iter( struct MBEAN_DATA* g );

void mbean_drop( struct MBEAN_DATA* g, int8_t x, int8_t y );

void mbean_place_drop_tiles( struct MBEAN_DATA* g );

/**
 * \brief Rotate the current beans drop (if it is not blocked).
 */
void mbean_rotate_drops( struct MBEAN_DATA* g );

void mbean_move_drops_x( struct MBEAN_DATA* g, int8_t x_m );

struct MBEAN_GC_NODE* mbean_gc_probe(
   struct MBEAN_DATA* g, int8_t x, int8_t y,
   struct MBEAN_GC_NODE* nodes, size_t* nodes_sz );

void mbean_gc( struct MBEAN_DATA* g );

#ifdef MBEAN_C
RETROFLAT_COLOR g_mbean_colors[5];
#else
extern RETROFLAT_COLOR g_mbean_colors[5];
#endif /* MBEAN_C */

#endif /* MBEAN_H */


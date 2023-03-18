
#include <maug.h>
#include <retrocon.h>

#define MBEAN_C
#include "mbean.h"

void mbean_iter_gravity( struct MBEAN_DATA* g, int8_t bx, int8_t by ) {

   /* Never do anything for the bottom row. */
   if( by >= MBEAN_GRID_H - 1 ) {
      return;
   }

   if( g->grid[bx][by] && !g->grid[bx][by + 1] ) {
      /* Move the bean down by one. */
      g->grid[bx][by + 1] = g->grid[bx][by];
      g->grid[bx][by] = 0;

      /* A fall happened, so the grid is unsettled. */
      g->flags &= ~MBEAN_FLAG_SETTLED;
   }
}

void mbean_iter( struct MBEAN_DATA* g ) {
   int8_t x = 0;
   int8_t y = 0;
   size_t i = 0;

   /* Drop the drops first so they can unsettle the grid if they drop. */
   for( i = 0 ; g->drops_sz > i ; i++ ) {
      x = g->drops_x + (i * g_mbean_drop_rot_x[g->drops_rot]);
      y = g->drops_y + (i * g_mbean_drop_rot_y[g->drops_rot]);
      if( y >= MBEAN_GRID_H - 1 || g->grid[x][y + 1] ) {
         /* Place the dropping beans on the grid. */
         mbean_plop_drops( g );
         goto settle_grid;
      }
   }

   /* Beans still dropping. */
   g->drops_y++;

settle_grid:

   /* Assuming settled by default. */
   g->flags |= MBEAN_FLAG_SETTLED;

   /* Start from the bottom up to avoid collisions with falling beans. */
   for( y = MBEAN_GRID_H - 1 ; 0 <= y ; y-- ) {
      for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
         mbean_iter_gravity( g, x, y );
      }
   }

}

void mbean_drop( struct MBEAN_DATA* g, int8_t x, int8_t y ) {
   g->drops_sz = 2;
   g->drops[0] = 3;
   g->drops[1] = 4;
   g->drops_x = x;
   g->drops_y = 0;
   g->drops_rot = 0;
}

void mbean_plop_drops( struct MBEAN_DATA* g ) {
   size_t i = 0;
   int8_t bx = 0,
      by = 0;

   for( i = 0 ; g->drops_sz > i ; i++ ) {
      bx = g->drops_x + (i * g_mbean_drop_rot_x[g->drops_rot]);
      by = g->drops_y + (i * g_mbean_drop_rot_y[g->drops_rot]);
      g->grid[bx][by] = g->drops[i];
   }

   g->drops_sz = 0;
}


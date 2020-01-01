
#include "mbean.h"

void mbean_iter_gravity( struct mbean_grid* g, int8_t bx, int8_t by ) {

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

void mbean_iter( struct mbean_grid* g ) {
   int8_t x = 0;
   int8_t y = 0;

   /* Assuming settled by default. */
   g->flags |= MBEAN_FLAG_SETTLED;

   /* Start from the bottom up to avoid collisions with falling beans. */
   for( y = MBEAN_GRID_H - 1 ; 0 <= y ; y-- ) {
      for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
         mbean_iter_gravity( g, x, y );
      }
   }

}



#include <maug.h>

#define MBEAN_C
#include "mbean.h"

void mbean_erase_bean_drops( struct MBEAN_DATA* g ) {
   size_t i = 0;
   int8_t dirty_x = 0;
   int8_t dirty_y = 0;

   for( i = 0 ; g->drops_sz > i ; i++ ) {

      /* Multiply by index of current drop, as the drop is i beans out from the
       * center/pivot bean of the drop.
       */
      dirty_x = g->drops_x + (i * gc_retroflat_offsets4_x[g->drops_rot]);
      dirty_y = g->drops_y + (i * gc_retroflat_offsets4_y[g->drops_rot]);

      /* These should be enforced by the collision check. */
      assert( 0 <= dirty_x && 0 <= dirty_y &&
         MBEAN_GRID_W > dirty_x && MBEAN_GRID_H > dirty_y );
      assert( 0 >= g->grid[dirty_x][dirty_y] );

      debug_printf( 0, "marking %d, %d (%d) dirty...",
         dirty_x, dirty_y, g->grid[dirty_x][dirty_y] );
      g->grid[dirty_x][dirty_y] = MBEAN_TILE_PURGE;
   }
}

MERROR_RETVAL mbean_check_collision(
   struct MBEAN_DATA* g, int8_t pivot_x, int8_t pivot_y, int8_t pivot_r
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   int8_t new_x = 0;
   int8_t new_y = 0;

   for( i = 0 ; g->drops_sz > i ; i++ ) {
      new_x = pivot_x + (i * gc_retroflat_offsets4_x[pivot_r]);
      new_y = pivot_y + (i * gc_retroflat_offsets4_y[pivot_r]);

      /* Bounds check. */
      if(
         0 > new_x || 0 > new_y ||
         MBEAN_GRID_W <= new_x || MBEAN_GRID_H <= new_y
      ) {
         retval = MERROR_USR;
         goto cleanup;
      }

      /* Make sure the space we're marking as purge starts empty. (If it's not
       * empty, that means we tried to move a bean *through* another bean,
       * somehow!
       */
      if( 0 < g->grid[new_x][new_y] ) {
         retval = MERROR_USR;
         goto cleanup;
      }
   }

cleanup:

   return retval;
}

void mbean_iter_gravity( struct MBEAN_DATA* g, int8_t bx, int8_t by ) {

   /* Never do anything for the bottom row. */
   if( by >= MBEAN_GRID_H - 1 ) {
      return;
   }

   /* If this tile is not empty but the tile below it is, fall! */
   if( 0 < g->grid[bx][by] && 0 >= g->grid[bx][by + 1] ) {
      /* Move the bean down by one. */
      g->grid[bx][by + 1] = g->grid[bx][by];
      g->grid[bx][by] = MBEAN_TILE_PURGE;

      /* A fall happened, so the grid is unsettled. */
      g->flags &= ~MBEAN_FLAG_SETTLED;
   }
}

void mbean_iter( struct MBEAN_DATA* g ) {
   int8_t x = 0;
   int8_t y = 0;
   size_t i = 0;

   g->flags &= ~MBEAN_FLAG_PLACED_LAST;

   /* Drop the drops first so they can unsettle the grid if they drop. */
   if( g->wait > 0 ) {
      g->wait--;
   } else {

      debug_printf( 0, "setting wait to %d...", MBEAN_TICK_WAIT );
      g->wait = MBEAN_TICK_WAIT;

      mbean_erase_bean_drops( g );

      /* Do actual drop. */
      for( i = 0 ; g->drops_sz > i ; i++ ) {
         /* Draw the current dropping pair according to their rotation. */
         x = g->drops_x + (i * gc_retroflat_offsets4_x[g->drops_rot]);
         y = g->drops_y + (i * gc_retroflat_offsets4_y[g->drops_rot]);
         if( y >= MBEAN_GRID_H - 1 || 0 < g->grid[x][y + 1] ) {
            /* Bean has hit the ground. Place the dropped beans on the grid. */
            mbean_place_drop_tiles( g );
            goto settle_grid;
         }
      }

      /* Beans still dropping. */
      g->drops_y++;
   }

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

   /* Create a new bean drop. */

   g->drops_sz = 2;
   g->drops[0] = 1 + retroflat_get_rand() % 4;
   g->drops[1] = 1 + retroflat_get_rand() % 4;
   g->drops_x = x;
   g->drops_y = 0;
   g->drops_rot = RETROFLAT_DIR4_EAST;
   debug_printf( 0, "setting wait to %d...", MBEAN_TICK_WAIT );
   g->wait = MBEAN_TICK_WAIT;
}

void mbean_place_drop_tiles( struct MBEAN_DATA* g ) {
   size_t i = 0;
   int8_t bx = 0,
      by = 0;

   /* Iterate through the beans in the drop and place them according to the
    * x/y-offset lookup table indexed by rotation key.
    */
   for( i = 0 ; g->drops_sz > i ; i++ ) {
      bx = g->drops_x + (i * gc_retroflat_offsets4_x[g->drops_rot]);
      by = g->drops_y + (i * gc_retroflat_offsets4_y[g->drops_rot]);
      g->grid[bx][by] = g->drops[i];
   }

   g->flags |= MBEAN_FLAG_PLACED_LAST;

   g->drops_sz = 0;
}

void mbean_rotate_drops( struct MBEAN_DATA* g ) {

   /* Collision check. */
   if( MERROR_OK != mbean_check_collision(
      g, g->drops_x, g->drops_y, retroflat_dir4_rotate_cw( g->drops_rot ) )
   ) {
      /* Collision! Cancel rotation! */
      return;
   }

   debug_printf( 1, "rotating beans..." );

   mbean_erase_bean_drops( g );

   /* Perform the rotation. */
   g->drops_rot = retroflat_dir4_rotate_cw( g->drops_rot );

   /* Notify the rest of the engine that the rotate has happened. */
   g->flags |= MBEAN_FLAG_ROT_LAST;

}

void mbean_move_drops_x( struct MBEAN_DATA* g, int8_t x_move ) {
   int8_t x_sz = gc_retroflat_offsets4_x[g->drops_rot];

   x_sz *= g->drops_sz;

   /* Collision check. */
   if( MERROR_OK != mbean_check_collision(
      g, g->drops_x + x_move, g->drops_y, g->drops_rot )
   ) {
      /* Collision! Cancel rotation! */
      return;
   }

   mbean_erase_bean_drops( g );

   g->drops_x += x_move;
}

struct MBEAN_GC_NODE* mbean_gc_probe(
   struct MBEAN_DATA* g, int8_t x, int8_t y,
   struct MBEAN_GC_NODE* nodes, size_t* nodes_sz
) {
   size_t i = 0;
   struct MBEAN_GC_NODE* self;

   debug_printf( 1, "probe #" SIZE_T_FMT ": %d, %d", *nodes_sz, x, y );

   g->probed[x][y] = 1;

   /* If we've been called on a node, it matches the previous (or is the
    * first in a probe!) */
   self = &(nodes[*nodes_sz]);
   self->x = x;
   self->y = y;
   self->bean = g->grid[x][y];
   (*nodes_sz)++;

   for( i = 0 ; 4 > i ; i++ ) {
      if(
         MBEAN_GRID_W <= x + gc_retroflat_offsets4_x[i] ||
         0 > x + gc_retroflat_offsets4_x[i] ||
         MBEAN_GRID_H <= y + gc_retroflat_offsets4_y[i] ||
         0 > y + gc_retroflat_offsets4_y[i]
      ) {
         /* Don't go off the grid! */
         continue;
      }

      if(
         g->probed[x + gc_retroflat_offsets4_x[i]][y + gc_retroflat_offsets4_y[i]]
      ) {
         /* Skip probed grid cells. */
         continue;
      }

      if(
         g->grid[x + gc_retroflat_offsets4_x[i]][y + gc_retroflat_offsets4_y[i]]
         == g->grid[x][y]
      ) {
         self->neighbors[i] = mbean_gc_probe(
            g, x + gc_retroflat_offsets4_x[i], y + gc_retroflat_offsets4_y[i],
            nodes, nodes_sz );
      }
   }

   return self;
}

void mbean_gc( struct MBEAN_DATA* g ) {
   struct MBEAN_GC_NODE nodes[MBEAN_GC_NODES_SZ_MAX];
   size_t nodes_sz = 0,
      i = 0;
   int8_t x = 0,
      y = 0;

   /* Reset probe/purge grids. */
   memset( g->probed, '\0', MBEAN_GRID_W * MBEAN_GRID_H );

   for( y = 0 ; MBEAN_GRID_H > y ; y++ ) {
      for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
         nodes_sz = 0;

         /* Skip probed or empty beans. */
         if( g->probed[x][y] || 0 == g->grid[x][y] ) {
            continue;
         }

         mbean_gc_probe( g, x, y, nodes, &nodes_sz );
         
         /* Make sure we found enough contiguous beans to vanish! */
         if( 4 > nodes_sz ) {
            continue;
         }

         /* We found enough, so descend and add to purge grid! */
         for( i = 0 ; nodes_sz > i ; i++ ) {
            g->grid[nodes[i].x][nodes[i].y] = MBEAN_TILE_PURGE;
         }
      }
   }

   for( y = 0 ; MBEAN_GRID_H > y ; y++ ) {
      for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
         if( MBEAN_TILE_PURGE == g->grid[x][y] ) {
            /* g->grid[x][y] = 0; */

            g->score += 10;

            /* Unsettle grid. */
            g->flags &= !MBEAN_FLAG_SETTLED;
         }
      }
   }
}


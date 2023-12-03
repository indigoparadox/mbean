
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
   if( g->wait > 0 ) {
      g->wait--;
   } else {
      /* Do actual drop. */
      g->wait = MBEAN_TICK_WAIT;
      for( i = 0 ; g->drops_sz > i ; i++ ) {
         x = g->drops_x + (i * gc_mbean_drop_rot_x[g->drops_rot]);
         y = g->drops_y + (i * gc_mbean_drop_rot_y[g->drops_rot]);
         if( y >= MBEAN_GRID_H - 1 || g->grid[x][y + 1] ) {
            /* Place the dropping beans on the grid. */
            mbean_plop_drops( g );
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
   g->drops_sz = 2;
   g->drops[0] = 1 + retroflat_get_rand() % 4;
   g->drops[1] = 1 + retroflat_get_rand() % 4;
   g->drops_x = x;
   g->drops_y = 0;
   g->drops_rot = 0;
}

void mbean_plop_drops( struct MBEAN_DATA* g ) {
   size_t i = 0;
   int8_t bx = 0,
      by = 0;

   for( i = 0 ; g->drops_sz > i ; i++ ) {
      bx = g->drops_x + (i * gc_mbean_drop_rot_x[g->drops_rot]);
      by = g->drops_y + (i * gc_mbean_drop_rot_y[g->drops_rot]);
      g->grid[bx][by] = g->drops[i];
   }

   g->drops_sz = 0;
}

void mbean_move_drops( struct MBEAN_DATA* g, int8_t x_m ) {
   int8_t x_sz = gc_mbean_drop_rot_x[g->drops_rot];

   x_sz *= g->drops_sz;

   /* Check if drops_x + size_t * offset is out of bounds, depending on
    * rotation.
    */
   if( (
      (0 == g->drops_rot && g->drops_x + x_m + x_sz <= MBEAN_GRID_W) ||
      (1 == g->drops_rot && g->drops_x + x_m < MBEAN_GRID_W) ||
      (2 == g->drops_rot && g->drops_x + x_m < MBEAN_GRID_W) ||
      (3 == g->drops_rot && g->drops_x + x_m < MBEAN_GRID_W)
   ) && (
      (0 == g->drops_rot && g->drops_x + x_m >= 0) ||
      (1 == g->drops_rot && g->drops_x + x_m >= 0) ||
      (2 == g->drops_rot && g->drops_x + x_m + x_sz >= 0) ||
      (3 == g->drops_rot && g->drops_x + x_m >= 0)
   ) ) {
      g->drops_x += x_m;
   }
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
         MBEAN_GRID_W <= x + gc_mbean_drop_rot_x[i] ||
         0 > x + gc_mbean_drop_rot_x[i] ||
         MBEAN_GRID_H <= y + gc_mbean_drop_rot_y[i] ||
         0 > y + gc_mbean_drop_rot_y[i]
      ) {
         /* Don't go off the grid! */
         continue;
      }

      if(
         g->probed[x + gc_mbean_drop_rot_x[i]][y + gc_mbean_drop_rot_y[i]]
      ) {
         /* Skip probed grid cells. */
         continue;
      }

      if(
         g->grid[x + gc_mbean_drop_rot_x[i]][y + gc_mbean_drop_rot_y[i]]
         == g->grid[x][y]
      ) {
         self->neighbors[i] = mbean_gc_probe(
            g, x + gc_mbean_drop_rot_x[i], y + gc_mbean_drop_rot_y[i],
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
   memset( g->purge, '\0', MBEAN_GRID_W * MBEAN_GRID_H );

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
            g->purge[nodes[i].x][nodes[i].y] = 1;
         }
      }
   }

   for( y = 0 ; MBEAN_GRID_H > y ; y++ ) {
      for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
         if( g->purge[x][y] ) {
            g->grid[x][y] = 0;
            g->purge[x][y] = 0;

            /* Unsettle grid. */
            g->flags &= !MBEAN_FLAG_SETTLED;
         }
      }
   }
}


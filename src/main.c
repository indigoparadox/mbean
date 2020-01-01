
#include "mbean.h"

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

int main( int argc, char** argv ) {
   struct mbean_grid g = { 0 };
   bool running = true;
   int x = 0, y = 0;

   while( running ) {
      /* Clear the screen and iterate the bean grid. */
      printf( "\e[1;1H\e[2J" );
      mbean_iter( &g );

      /* Drop a bean set if we can. */
      if( MBEAN_FLAG_SETTLED & g.flags ) {
         mbean_drop( &g, 1, 3 );
         mbean_drop( &g, 2, 4 );
      }

      /* Roughly display the bean grid. */
      for( y = 0 ; MBEAN_GRID_H > y ; y++ ) {
         printf( "|" );
         for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
            if( !g.grid[x][y] ) {
               printf( " " );
            } else {
               printf( "%c", (65 + g.grid[x][y]) );
            }
         }
         printf( "|\n" );
      }
      printf( "|" );
      for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
         printf( "-" );
      }
      printf( "|\n\n" );

      /* Wait a moment. */
      sleep( 1 );
   }

   return 0;
}


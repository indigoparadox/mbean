
#define MAUG_C
#include <maug.h>

#define RETROFLT_C
#include <retroflt.h>

#define RETROCON_C
#include <retrocon.h>

#define RETROSND_C
#include <retrosnd.h>

#include "mbean.h"

void mbean_loop( MAUG_MHANDLE data_h ) {
   struct MBEAN_DATA* data = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t x = 0,
      y = 0,
      i = 0;
   int input = 0;
   struct RETROFLAT_INPUT input_evt;

   maug_mlock( data_h, data );
   maug_cleanup_if_null_alloc( struct MBEAN_DATA*, data );

   /* See if we're done playing a sound. */
   if( 1 == data->snd_cycles_left ) {
      debug_printf( 2, "finished playing sound" );
      retrosnd_midi_note_off( MBEAN_SND_CHANNEL, 60, 127 );
      data->snd_cycles_left--;
   } else if( 0 < data->snd_cycles_left ) {
      data->snd_cycles_left--;
   }

   if( RETROCON_FLAG_ACTIVE != (RETROCON_FLAG_ACTIVE & data->con.flags) ) {
      /* Clear the screen and iterate the bean grid. */
      mbean_iter( data );

      /* Play a sound if just dropped and none is currently playing. */
      if(
         MBEAN_FLAG_PLACED_LAST == (MBEAN_FLAG_PLACED_LAST & data->flags) &&
         0 == data->snd_cycles_left
      ) {
         debug_printf( 2, "playing sound..." );
         retrosnd_midi_note_on( MBEAN_SND_CHANNEL, 60, 127 );
         data->snd_cycles_left = MBEAN_SND_CYCLES;
      }

      /* Drop a bean set if we can. */
      if( (MBEAN_FLAG_SETTLED & data->flags) && 0 == data->drops_sz ) {
         debug_printf( 1, "attempting gc..." );
         mbean_gc( data );
         if( (MBEAN_FLAG_SETTLED & data->flags) && 0 == data->drops_sz ) {
            debug_printf( 1, "dropping..." );
            mbean_drop( data, 3, 0 );
         }
         /*
         mbean_drop( data, 1, 3 );
         mbean_drop( data, 2, 4 );
         */
      }
   }

   /* === Input === */

   input = retroflat_poll_input( &input_evt );

   retrocon_input( &(data->con), &input, &input_evt );

   if( RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & data->con.flags) ) {
      goto display;
   }

   switch( input ) {
   case RETROFLAT_KEY_RIGHT:
      mbean_move_drops_x( data, 1 );
      break;

   case RETROFLAT_KEY_LEFT:
      mbean_move_drops_x( data, -1 );
      break;

   case RETROFLAT_KEY_DOWN:
      debug_printf( 0, "setting wait to %d...", MBEAN_WAIT_SKIP );
      data->wait = MBEAN_WAIT_SKIP;
      break;

   case RETROFLAT_KEY_SPACE:
      if( MBEAN_FLAG_ROT_LAST != (MBEAN_FLAG_ROT_LAST & data->flags) ) {
         mbean_rotate_drops( data );
      }
      break;

   case RETROFLAT_KEY_ESC:
      error_printf( "quitting due to keypress..." );
      retroflat_quit( 0 );
      break;
   }

   /* Reset rotate flag if rotate not pressed. */
   if( RETROFLAT_KEY_SPACE != input ) {
      /* TODO: Have a timer to suppress rotation flag. */
      data->flags &= ~MBEAN_FLAG_ROT_LAST;
   }
 
   /*  === Drawing === */

display:

   retroflat_draw_lock( NULL );

   /*
   retroflat_rect(
      NULL, RETROFLAT_COLOR_BLACK, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );
   */

   if( MBEAN_FLAG_INIT_DONE != (MBEAN_FLAG_INIT_DONE & data->flags) ) {
      retroflat_rect(
         NULL, RETROFLAT_COLOR_WHITE, 
         MBEAN_GRID_X_PX, MBEAN_GRID_Y_PX,
         MBEAN_GRID_W_PX, MBEAN_GRID_H_PX,
         0 );
   }

   /* Roughly display the bean grid. */
   for( y = 0 ; MBEAN_GRID_H > y ; y++ ) {
      for( x = 0 ; MBEAN_GRID_W > x ; x++ ) {
         if( 0 < data->grid[x][y] ) {
            /* Draw colorful bean tiles. */
            retroflat_ellipse( NULL, g_mbean_colors[data->grid[x][y]],
               MBEAN_GRID_X_PX + (x * MBEAN_BEAN_W),
               MBEAN_GRID_Y_PX + (y * MBEAN_BEAN_W),
               MBEAN_BEAN_W,
               MBEAN_BEAN_W,
               0 );
         } else if( MBEAN_TILE_PURGE == data->grid[x][y] ) {
            /* Overdraw dirty bean tiles with background. */
            retroflat_ellipse( NULL, RETROFLAT_COLOR_BLACK,
               MBEAN_GRID_X_PX + (x * MBEAN_BEAN_W),
               MBEAN_GRID_Y_PX + (y * MBEAN_BEAN_W),
               MBEAN_BEAN_W,
               MBEAN_BEAN_W,
               0 );
            data->grid[x][y] = 0;
         }
      }
   }

   /* Draw drops. */
   for( i = 0 ; data->drops_sz > i ; i++ ) {
      x = data->drops_x + (i * gc_mbean_drop_rot_x[data->drops_rot]);
      y = data->drops_y + (i * gc_mbean_drop_rot_y[data->drops_rot]);
      /* debug_printf( 3, "drop: %d, %d", x, y ); */
      retroflat_ellipse( NULL, g_mbean_colors[data->drops[i]],
         MBEAN_GRID_X_PX + (x * MBEAN_BEAN_W),
         MBEAN_GRID_Y_PX + (y * MBEAN_BEAN_W),
         MBEAN_BEAN_W,
         MBEAN_BEAN_W,
         0 );
   }

   retrocon_display( &(data->con), NULL );

   retroflat_draw_release( NULL );

cleanup:

   if( NULL != data ) {
      maug_munlock( data_h, data );
   }

   if( MERROR_OK != retval ) {
      error_printf( "quitting due to error..." );
      retroflat_quit( retval );
   }
}

int main( int argc, char* argv[] ) {
   int retval = 0;
   MAUG_MHANDLE data_h = NULL;
   struct RETROFLAT_ARGS args;
   struct MBEAN_DATA* data = NULL;

   /* === Setup === */

   logging_init();

   maug_mzero( &args, sizeof( struct RETROFLAT_ARGS ) );

   args.screen_w = 320;
   args.screen_h = 200;
   args.title = "mbean";
   args.flags |= RETROFLAT_FLAGS_KEY_REPEAT;

   retval = retroflat_init( argc, argv, &args );
   maug_cleanup_if_not_ok();

   retval = retrosnd_init( &args );
   maug_cleanup_if_not_ok();

   retrosnd_midi_set_voice( MBEAN_SND_CHANNEL, 127 );
   /* retrosnd_midi_set_control( MBEAN_SND_CHANNEL, 7, 127 );
   retrosnd_midi_set_control( MBEAN_SND_CHANNEL, 39, 0x3fff ); */

   debug_printf( 3, "allocating data struct (" SIZE_T_FMT " bytes)...",
      sizeof( struct MBEAN_DATA ) );

   data_h = maug_malloc( 1, sizeof( struct MBEAN_DATA ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, data_h );

   maug_mlock( data_h, data );
   maug_cleanup_if_null_alloc( struct MBEAN_DATA*, data );

   maug_mzero( data, sizeof( struct MBEAN_DATA ) );

   retval = retrocon_init( &(data->con) );
   maug_cleanup_if_not_ok();

   data->con.lbuffer_color = RETROFLAT_COLOR_WHITE;
   data->con.sbuffer_color = RETROFLAT_COLOR_GRAY;
   data->con.bg_color = RETROFLAT_COLOR_BLACK;

   maug_munlock( data_h, data );

   g_mbean_colors[1] = RETROFLAT_COLOR_GRAY;
   g_mbean_colors[2] = RETROFLAT_COLOR_RED;
   g_mbean_colors[3] = RETROFLAT_COLOR_GREEN;
   g_mbean_colors[4] = RETROFLAT_COLOR_BLUE;

   retroflat_message( RETROFLAT_MSG_FLAG_WARNING, "Hello", "Beans" );

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)mbean_loop, data_h );

cleanup:

   retrosnd_shutdown();

#ifndef RETROFLAT_OS_WASM

   if( NULL != data_h ) {
      maug_mfree( data_h );
   }

   retroflat_shutdown( retval );

#endif /* !RETROFLAT_OS_WASM */

   logging_shutdown();

   return retval;
}
END_OF_MAIN()


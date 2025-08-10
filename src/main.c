
#define MAUG_C

#include "mbean.h"

#define MBEAN_SCORE_STR_SZ_MAX 16

void mbean_loop( MAUG_MHANDLE data_h ) {
   struct MBEAN_DATA* data = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t x = 0,
      y = 0,
      i = 0;
   RETROFLAT_IN_KEY input = 0;
   struct RETROFLAT_INPUT input_evt;
   char score_str[MBEAN_SCORE_STR_SZ_MAX + 1] = { 0 };
   retrogui_idc_t idc_con = RETROGUI_IDC_NONE;
   char hash_str[] = "Commit\n" MBEAN_COMMIT_HASH;

   maug_mlock( data_h, data );
   maug_cleanup_if_null_alloc( struct MBEAN_DATA*, data );

#ifdef MBEAN_SOUND
   /* See if we're done playing a sound. */
   if( 1 == data->snd_cycles_left ) {
      debug_printf( 2, MS_FMT ": finished playing sound",
         retroflat_get_ms() );
      retrosnd_midi_note_off( MBEAN_SND_CHANNEL, 60, 127 );
      data->snd_cycles_left--;
   } else if( 0 < data->snd_cycles_left ) {
      data->snd_cycles_left--;
   }
#endif /* MBEAN_SOUND */

   if( RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & data->con.flags) ) {
      /* Skip bean logic. */
      goto check_input;
   }

   /* Clear the screen and iterate the bean grid. */
   mbean_iter( data );

   /* Drop a bean set if we can. */
   if(
      MBEAN_FLAG_SETTLED == (MBEAN_FLAG_SETTLED & data->flags) &&
      0 == data->drops_sz
   ) {
#ifdef MBEAN_SOUND
      if( 0 == data->snd_cycles_left ) {
         debug_printf( 2, MS_FMT ": playing sound...",
            retroflat_get_ms() );
         retrosnd_midi_note_on( MBEAN_SND_CHANNEL, 60, 127 );
         data->snd_cycles_left = MBEAN_SND_CYCLES;
      }
#endif /* MBEAN_SOUND */

      debug_printf( 1, MS_FMT ": attempting gc...",
         retroflat_get_ms() );
      mbean_gc( data );
      if( (MBEAN_FLAG_SETTLED & data->flags) && 0 == data->drops_sz ) {
         debug_printf( 1, MS_FMT ": dropping...",
            retroflat_get_ms() );
         mbean_drop( data, 3, 0 );
      }
      /*
      mbean_drop( data, 1, 3 );
      mbean_drop( data, 2, 4 );
      */
   } else {
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 10, 10,
         40, 20,
         RETROFLAT_FLAGS_FILL );
   }

check_input:

   /* === Input === */

   input = retroflat_poll_input( &input_evt );

   retrocon_input( &(data->con), &input, &input_evt, &idc_con, NULL );
   if( RETROCON_IDC_CLOSE == idc_con ) {
      /* Redraw the screen. */
      data->flags &= ~MBEAN_FLAG_INIT_DONE;
   }

   if( RETROCON_FLAG_ACTIVE == (RETROCON_FLAG_ACTIVE & data->con.flags) ) {
      goto display;
   }

   switch( input ) {
   retroflat_case_key( RETROFLAT_KEY_RIGHT, RETROFLAT_PAD_RIGHT )
      mbean_move_drops_x( data, 1 );
      /* Reset rotate flag if rotate not pressed. */
      data->flags &= ~MBEAN_FLAG_ROT_LAST;
      break;

   retroflat_case_key( RETROFLAT_KEY_LEFT, RETROFLAT_PAD_LEFT )
      mbean_move_drops_x( data, -1 );
      /* Reset rotate flag if rotate not pressed. */
      data->flags &= ~MBEAN_FLAG_ROT_LAST;
      break;

   retroflat_case_key( RETROFLAT_KEY_DOWN, RETROFLAT_PAD_DOWN )
      debug_printf( 0, MS_FMT ": setting wait to %d...",
         retroflat_get_ms(), MBEAN_WAIT_SKIP );
      data->wait = MBEAN_WAIT_SKIP;
      /* Reset rotate flag if rotate not pressed. */
      data->flags &= ~MBEAN_FLAG_ROT_LAST;
      break;

   retroflat_case_key( RETROFLAT_KEY_SPACE, RETROFLAT_PAD_A )
      if( MBEAN_FLAG_ROT_LAST != (MBEAN_FLAG_ROT_LAST & data->flags) ) {
         mbean_rotate_drops( data );
      }
      break;

#ifndef RETROFLAT_NO_KEYBOARD
   case RETROFLAT_KEY_ESC:
      error_printf( "quitting due to keypress..." );
      retroflat_quit( 0 );
      break;
#endif /* !RETROFLAT_NO_KEYBOARD */
   }

   /* TODO: Have a timer to toggle rotation-suppression flag. */
   /* data->flags &= ~MBEAN_FLAG_ROT_LAST; */
 
   /*  === Drawing === */

display:

   retroflat_draw_lock( NULL );

   if( MBEAN_FLAG_INIT_DONE != (MBEAN_FLAG_INIT_DONE & data->flags) ) {
      /* Black out background (Windows and some platforms don't zero). */
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 0, 0,
         retroflat_screen_w(), retroflat_screen_h(),
         RETROFLAT_FLAGS_FILL );
      
      /* Draw rectangle border. */
      retroflat_rect(
         NULL, RETROFLAT_COLOR_WHITE, 
         MBEAN_GRID_X_PX, MBEAN_GRID_Y_PX,
         MBEAN_GRID_W_PX, MBEAN_GRID_H_PX,
         0 );

      hash_str[11] = '\0';
      retrofont_string(
         NULL, RETROFLAT_COLOR_RED, hash_str, 0,
         data->font_h,
         MBEAN_GRID_X_PX + MBEAN_GRID_W_PX + 10,
         MBEAN_GRID_Y_PX + 60, 0, 0, 0 );

      data->flags |= MBEAN_FLAG_INIT_DONE;
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
      x = data->drops_x + (i * gc_retroflat_offsets4_x[data->drops_rot]);
      y = data->drops_y + (i * gc_retroflat_offsets4_y[data->drops_rot]);
      /* debug_printf( 3, "drop: %d, %d", x, y ); */
      retroflat_ellipse( NULL, g_mbean_colors[data->drops[i]],
         MBEAN_GRID_X_PX + (x * MBEAN_BEAN_W),
         MBEAN_GRID_Y_PX + (y * MBEAN_BEAN_W),
         MBEAN_BEAN_W,
         MBEAN_BEAN_W,
         0 );
   }

   if( data->score_prev_draw != data->score ) {
      /* Redraw the score. */
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLACK, 10, 10,
         40, 20,
         RETROFLAT_FLAGS_FILL );
      maug_snprintf( score_str, MBEAN_SCORE_STR_SZ_MAX,
         "Score\n%04d", data->score );
      retrofont_string(
         NULL, RETROFLAT_COLOR_RED, score_str, 0,
         data->font_h,
         MBEAN_GRID_X_PX + MBEAN_GRID_W_PX + 10,
         MBEAN_GRID_Y_PX + 10, 0, 0, 0 );
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

#if 0
void dump_palette() {
   size_t i = 0;
   uint32_t rgb = 0;

   for( i = 0 ; 16 > i ; i++ ) {
      retroflat_get_palette( i, &rgb );
      debug_printf( 3, "color " SIZE_T_FMT ": " X32_FMT,
         i, rgb );
   }
}
#endif

int main( int argc, char* argv[] ) {
   int retval = 0;
   MAUG_MHANDLE data_h = NULL;
   struct RETROFLAT_ARGS args;
   struct MBEAN_DATA* data = NULL;

   /* === Setup === */

   logging_init();

   maug_mzero( &args, sizeof( struct RETROFLAT_ARGS ) );

   args.title = "mbean";
   args.flags |= RETROFLAT_FLAGS_KEY_REPEAT;

   retval = retroflat_init( argc, argv, &args );
   maug_cleanup_if_not_ok();

#ifdef MBEAN_SOUND
   retval = retrosnd_init( &args );
   if( MERROR_SND == retval ) {
      retroflat_message( RETROFLAT_MSG_FLAG_WARNING, "Sound Error",
         "Could not setup sound device!" );
      retval = MERROR_OK;
   } else if( MERROR_OK != retval ) {
      goto cleanup;
   }

#ifdef RETROSND_API_PC_BIOS
   retrosnd_midi_set_sf_bank( "dmx_dmx.op2" );
#endif /* RETROSND_API_PC_BIOS */
   retrosnd_midi_set_voice( MBEAN_SND_CHANNEL, 0 );
   /* retrosnd_midi_set_control( MBEAN_SND_CHANNEL, 7, 127 );
   retrosnd_midi_set_control( MBEAN_SND_CHANNEL, 39, 0x3fff ); */

#endif /* MBEAN_SOUND */

   debug_printf( 3, MS_FMT ": allocating data struct ("
      SIZE_T_FMT " bytes)...",
      retroflat_get_ms(), sizeof( struct MBEAN_DATA ) );

   data_h = maug_malloc( 1, sizeof( struct MBEAN_DATA ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, data_h );

   maug_mlock( data_h, data );
   maug_cleanup_if_null_alloc( struct MBEAN_DATA*, data );

   maug_mzero( data, sizeof( struct MBEAN_DATA ) );

   /* TODO: Font support under WASM! */
   retval = retrofont_load( "unscii_8.hex", &(data->font_h), 8, 33, 93 );
   if( MERROR_OK != retval ) {
      retroflat_message(
         RETROFLAT_MSG_FLAG_ERROR, "Error", "Could not load font!" );
   }
   retval = MERROR_OK; /* XXX */
   maug_cleanup_if_not_ok();

   retrocon_init( &(data->con), "unscii_8.hex",
      (retroflat_screen_w() >> 1) - 100,
      (retroflat_screen_h() >> 1) - 50,
      200, 100 );

   data->con.lbuffer_color = RETROFLAT_COLOR_WHITE;
   data->con.sbuffer_color = RETROFLAT_COLOR_GRAY;
   data->con.bg_color = RETROFLAT_COLOR_BLACK;

   data->score_prev_draw = -1; /* Force initial score draw. */

   maug_munlock( data_h, data );

   g_mbean_colors[1] = RETROFLAT_COLOR_GRAY;
   g_mbean_colors[2] = RETROFLAT_COLOR_RED;
   g_mbean_colors[3] = RETROFLAT_COLOR_GREEN;
   g_mbean_colors[4] = RETROFLAT_COLOR_BLUE;

   /*
   dump_palette();

   retroflat_set_palette( 1, 0xffffffff );
   retroflat_set_palette( 2, 0xffffffff );
   retroflat_set_palette( 3, 0xffffffff );
   retroflat_set_palette( 4, 0xffffffff );
   */

   /* retroflat_message( RETROFLAT_MSG_FLAG_WARNING, "Hello", "Beans" ); */

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)mbean_loop, NULL, data_h );

cleanup:

#ifndef RETROFLAT_OS_WASM

   if( (MAUG_MHANDLE)NULL != data_h ) {
      maug_mlock( data_h, data );
      retrocon_shutdown( &(data->con) );
      maug_munlock( data_h, data );
   }

#ifdef MBEAN_SOUND
   retrosnd_shutdown();
#endif /* MBEAN_SOUND */

   if( (MAUG_MHANDLE)NULL != data_h ) {
      maug_mlock( data_h, data );
      maug_mfree( data->font_h );
      maug_munlock( data_h, data );
      maug_mfree( data_h );
   }

   retroflat_shutdown( retval );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()


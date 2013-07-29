#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x07, 0x7E, 0xC9, 0xB3, 0x93, 0xD3, 0x49, 0x04, 0x82, 0x93, 0xC6, 0xC0, 0x6D, 0x89, 0xFF, 0x8C }
PBL_APP_INFO(MY_UUID,
             "Count Down WatchFace", "Tommy.K",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer time_layer;
TextLayer text_layer;
TextLayer week_layer;

// Can be used to cancel timer via `app_timer_cancel_event()`
AppTimerHandle timer_handle;

PblTm getTimeDifference(PblTm nextTimePoint, PblTm currentTime) 
{
  PblTm returnTime;

  if (nextTimePoint.tm_min >= currentTime.tm_min) {
    returnTime.tm_hour = nextTimePoint.tm_hour - currentTime.tm_hour;
    returnTime.tm_min = nextTimePoint.tm_min - currentTime.tm_min;
    return returnTime;
  }

  returnTime.tm_hour = nextTimePoint.tm_hour - currentTime.tm_hour - 1;
  returnTime.tm_min = nextTimePoint.tm_min + 60 - currentTime.tm_min;

  return returnTime;
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.
  static char weekText[] = "00";

  PblTm currentTime;
  PblTm nextTimePoint;
  PblTm timeLeft;
  PblTm weeksLeft;

  get_time(&currentTime);

  if (currentTime.tm_hour < 8) 
  {
     nextTimePoint.tm_hour = 8;
     nextTimePoint.tm_min = 30;

     timeLeft = getTimeDifference(nextTimePoint, currentTime);
  } 
  else if (currentTime.tm_hour == 8 && currentTime.tm_min <= 30)
  {
     nextTimePoint.tm_hour = 8;
     nextTimePoint.tm_min = 30;

     timeLeft = getTimeDifference(nextTimePoint, currentTime);
  }
  else if (currentTime.tm_hour < 12)
  {
     nextTimePoint.tm_hour = 12;
     nextTimePoint.tm_min = 0;

     timeLeft = getTimeDifference(nextTimePoint, currentTime);
  }
  else if (currentTime.tm_hour < 13)
  {
     nextTimePoint.tm_hour = 13;
     nextTimePoint.tm_min = 0;

     timeLeft = getTimeDifference(nextTimePoint, currentTime);
  }
  else if (currentTime.tm_hour < 17)
  {
     nextTimePoint.tm_hour = 17;
     nextTimePoint.tm_min = 0;

     timeLeft = getTimeDifference(nextTimePoint, currentTime);
  }
  else
  {
     nextTimePoint.tm_hour = 24;
     nextTimePoint.tm_min = 0;

     timeLeft = getTimeDifference(nextTimePoint, currentTime);
  }
  string_format_time(timeText, sizeof(timeText), "%H:%M", &timeLeft);

  text_layer_set_text(&time_layer, timeText);

  weeksLeft.tm_min = (365 - currentTime.tm_yday) / 7; // minをweeksの代用
  string_format_time(weekText, sizeof(2), "%M", &weeksLeft);
  strcat(weekText, " WKS LFT");

  text_layer_set_text(&week_layer, weekText);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  // week_layer
  text_layer_init(&week_layer, GRect(0, 20, 144, 30));
  text_layer_set_background_color(&week_layer, GColorBlack);
  text_layer_set_text_alignment(&week_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&week_layer, GColorWhite);
  text_layer_set_font(&week_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  // time_layer
  text_layer_init(&time_layer, GRect(0, 55, 144, 55));
  text_layer_set_background_color(&time_layer, GColorBlack);
  text_layer_set_text_alignment(&time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&time_layer, GColorWhite);
  text_layer_set_font(&time_layer, fonts_get_system_font(FONT_KEY_GOTHAM_42_MEDIUM_NUMBERS));

  // text_layer
  text_layer_init(&text_layer, GRect(0, 110, 144, 30));
  text_layer_set_background_color(&text_layer, GColorBlack);
  text_layer_set_text_alignment(&text_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&text_layer, GColorWhite);
  text_layer_set_font(&text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(&text_layer, "TL NXT TM PT");


  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(ctx, NULL);


  layer_add_child(&window.layer, &week_layer.layer);
  layer_add_child(&window.layer, &text_layer.layer);
  layer_add_child(&window.layer, &time_layer.layer);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    // Handle app start
    .init_handler = &handle_init,
    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}

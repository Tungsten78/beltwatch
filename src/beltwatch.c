#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x46, 0xC7, 0x43, 0x97, 0xE3, 0x87, 0x4A, 0x44, 0x99, 0x29, 0x96, 0xA6, 0xAE, 0xB2, 0x77, 0x83 }
PBL_APP_INFO(MY_UUID,
             "BeltWatch", "",
             1, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
Layer layer;
TextLayer layerH1, layerH2, layerM1, layerM2, layerS1, layerS2;
PropertyAnimation prop_aniH1, prop_aniH2, prop_aniM1, prop_aniM2, prop_aniS1, prop_aniS2;
BmpContainer bitmap_container;
bool first = true;
int digits[] = {0,0,0,0,0,0};

#define COUNT 57
const char* numberstr = "0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9";

#define WIDTH 144
#define HEIGHT 168

#define BLOCK_HEIGHT 28
#define BLOCK_WIDTH 14

GRect textrect = {{0,0},{BLOCK_WIDTH,COUNT*BLOCK_HEIGHT}};

void create_layer(TextLayer *layer, GRect rect, int offset, const char *font)
{
    rect.origin.x += offset;
    
    text_layer_init(layer, rect);
    text_layer_set_text_color(layer, GColorBlack);
    text_layer_set_background_color(layer, GColorClear);
    text_layer_set_overflow_mode(layer, GTextOverflowModeWordWrap);
    text_layer_set_font(layer, fonts_get_system_font(font));
    text_layer_set_text_alignment(layer,GTextAlignmentLeft);
    text_layer_set_text(layer, numberstr);
    
    layer_add_child(&window.layer, &layer->layer);
    
    layer->layer.frame.origin.y = (-10 * BLOCK_HEIGHT) + ((HEIGHT-BLOCK_HEIGHT-10)/2);

}

void handle_init(AppContextRef ctx)
{
    (void)ctx;

    window_init(&window, "BeltWatch");
    window_stack_push(&window, true /* Animated */);

    create_layer(&layerH1, textrect, 21, FONT_KEY_GOTHIC_28_BOLD);
    create_layer(&layerH2, textrect, 39, FONT_KEY_GOTHIC_28_BOLD);
    create_layer(&layerM1, textrect, 58, FONT_KEY_GOTHIC_28_BOLD);
    create_layer(&layerM2, textrect, 76, FONT_KEY_GOTHIC_28_BOLD);
    create_layer(&layerS1, textrect, 94, FONT_KEY_GOTHIC_28_BOLD);
    create_layer(&layerS2, textrect, 112, FONT_KEY_GOTHIC_28_BOLD);
    
    resource_init_current_app(&APP_RESOURCES);
    bmp_init_container(RESOURCE_ID_IMAGE_OVERLAY, &bitmap_container);
    bitmap_layer_set_compositing_mode(&bitmap_container.layer, GCompOpAnd);

    layer_add_child(&window.layer, &bitmap_container.layer.layer);
}

void handle_deinit(AppContextRef ctx)
{
    (void)ctx;
    
    bmp_deinit_container(&bitmap_container);
}

void animate(TextLayer *layer, PropertyAnimation *ani, int t)
{
    GRect rect = layer->layer.frame;
    int oldY = rect.origin.y;
    rect.origin.y= (-10 - t)* BLOCK_HEIGHT + ((HEIGHT-BLOCK_HEIGHT-10)/2);
    if (rect.origin.y != oldY)
    {
        property_animation_init_layer_frame(ani, &layer->layer, NULL, &rect);
        if (!first && layer != &layerS2) animation_set_duration(&ani->animation, 2000);
        animation_schedule(&ani->animation);
    }
}

void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t)
{
    (void)ctx;

    int tt;
    if (!clock_is_24h_style()) t->tick_time->tm_hour %= 12;
    if (digits[0]!=(tt=t->tick_time->tm_hour/10)) animate(&layerH1, &prop_aniH1, digits[0]=tt);
    if (digits[1]!=(tt=t->tick_time->tm_hour%10)) animate(&layerH2, &prop_aniH2, digits[1]=tt);
    if (digits[2]!=(tt=t->tick_time->tm_min/10)) animate(&layerM1, &prop_aniM1, digits[2]=tt);
    if (digits[3]!=(tt=t->tick_time->tm_min%10)) animate(&layerM2, &prop_aniM2, digits[3]=tt);
    if (digits[4]!=(tt=t->tick_time->tm_sec/10)) animate(&layerS1, &prop_aniS1, digits[4]=tt);
    if (digits[5]!=(tt=t->tick_time->tm_sec%10)) animate(&layerS2, &prop_aniS2, digits[5]=tt);
    
    first = false;
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
      .init_handler = &handle_init,
      .deinit_handler = &handle_deinit,
      .tick_info = {
          .tick_handler = &handle_second_tick,
          .tick_units = SECOND_UNIT
      }
  };
  app_event_loop(params, &handlers);
}

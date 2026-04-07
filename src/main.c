#include <pebble.h>

// ============================================================
// Pixel Sampler — main.c
// v1.0: Fonts, Colors, Platform browser
// ============================================================

// ============================================================
// FONT TABLE
// ============================================================
typedef struct {
  const char *name;    // display name, e.g. "Gothic 14"
  const char *key;     // FONT_KEY_* string
} SamplerFont;

static const SamplerFont s_fonts[] = {
  { "Gothic 14",              FONT_KEY_GOTHIC_14              },
  { "Gothic 14 Bold",         FONT_KEY_GOTHIC_14_BOLD         },
  { "Gothic 18",              FONT_KEY_GOTHIC_18              },
  { "Gothic 18 Bold",         FONT_KEY_GOTHIC_18_BOLD         },
  { "Gothic 24",              FONT_KEY_GOTHIC_24              },
  { "Gothic 24 Bold",         FONT_KEY_GOTHIC_24_BOLD         },
  { "Gothic 28",              FONT_KEY_GOTHIC_28              },
  { "Gothic 28 Bold",         FONT_KEY_GOTHIC_28_BOLD         },
  { "Bitham 30 Black",        FONT_KEY_BITHAM_30_BLACK        },
  { "Bitham 42 Bold",         FONT_KEY_BITHAM_42_BOLD         },
  { "Bitham 42 Light",        FONT_KEY_BITHAM_42_LIGHT        },
  { "Bitham 34 Med Nums",     FONT_KEY_BITHAM_34_MEDIUM_NUMBERS },
  { "Bitham 42 Med Nums",     FONT_KEY_BITHAM_42_MEDIUM_NUMBERS },
  { "Roboto Cond 21",         FONT_KEY_ROBOTO_CONDENSED_21    },
  { "Roboto Bold 49",         FONT_KEY_ROBOTO_BOLD_SUBSET_49  },
  { "Droid Serif 28 Bold",    FONT_KEY_DROID_SERIF_28_BOLD    },
  { "LECO 36 Bold",           FONT_KEY_LECO_36_BOLD_NUMBERS   },
  { "LECO 38 Bold",           FONT_KEY_LECO_38_BOLD_NUMBERS   },
  { "Gothic 24 Num",          FONT_KEY_GOTHIC_24_BOLD         },
};
#define NUM_FONTS ((int)(sizeof(s_fonts) / sizeof(s_fonts[0])))

// ============================================================
// COLOR TABLE  (64 Pebble palette colors)
// ============================================================
typedef struct {
  const char *name;   // GColor name, e.g. "GColorMintGreen"
  const char *hex;    // e.g. "#aaffaa"
  uint8_t r, g, b;   // 0..3 each (Pebble 6-bit)
  GColor   color;
} SamplerColor;

// Macro: convert 2-bit channel (0..3) to 8-bit (0,85,170,255)
#define CH(v) ((uint8_t)((v)*85))
#define PC(nm, hx, r2, g2, b2) \
  { nm, hx, r2, g2, b2, {.argb = (uint8_t)(0b11000000 | ((r2)<<4) | ((g2)<<2) | (b2))} }

static const SamplerColor s_colors[] = {
  PC("GColorBlack",               "#000000", 0,0,0),
  PC("GColorOxfordBlue",          "#000055", 0,0,1),
  PC("GColorDukeBlue",            "#0000aa", 0,0,2),
  PC("GColorBlue",                "#0000ff", 0,0,3),
  PC("GColorDarkGreen",           "#005500", 0,1,0),
  PC("GColorMidnightGreen",       "#005555", 0,1,1),
  PC("GColorCobaltBlue",          "#0055aa", 0,1,2),
  PC("GColorBlueMoon",            "#0055ff", 0,1,3),
  PC("GColorIslamicGreen",        "#00aa00", 0,2,0),
  PC("GColorJaegerGreen",         "#00aa55", 0,2,1),
  PC("GColorTiffanyBlue",         "#00aaaa", 0,2,2),
  PC("GColorVividCerulean",       "#00aaff", 0,2,3),
  PC("GColorGreen",               "#00ff00", 0,3,0),
  PC("GColorMalachite",           "#00ff55", 0,3,1),
  PC("GColorMediumSpringGreen",   "#00ffaa", 0,3,2),
  PC("GColorCyan",                "#00ffff", 0,3,3),
  PC("GColorBulgarianRose",       "#550000", 1,0,0),
  PC("GColorImperialPurple",      "#550055", 1,0,1),
  PC("GColorIndigo",              "#5500aa", 1,0,2),
  PC("GColorElectricUltramarine", "#5500ff", 1,0,3),
  PC("GColorArmyGreen",           "#555500", 1,1,0),
  PC("GColorDarkGray",            "#555555", 1,1,1),
  PC("GColorLiberty",             "#5555aa", 1,1,2),
  PC("GColorVeryLightBlue",       "#5555ff", 1,1,3),
  PC("GColorKellyGreen",          "#55aa00", 1,2,0),
  PC("GColorMayGreen",            "#55aa55", 1,2,1),
  PC("GColorCadetBlue",           "#55aaaa", 1,2,2),
  PC("GColorPictonBlue",          "#55aaff", 1,2,3),
  PC("GColorBrightGreen",         "#55ff00", 1,3,0),
  PC("GColorScreaminGreen",       "#55ff55", 1,3,1),
  PC("GColorMediumAquamarine",    "#55ffaa", 1,3,2),
  PC("GColorElectricBlue",        "#55ffff", 1,3,3),
  PC("GColorDarkCandyAppleRed",   "#aa0000", 2,0,0),
  PC("GColorJazzberryJam",        "#aa0055", 2,0,1),
  PC("GColorPurple",              "#aa00aa", 2,0,2),
  PC("GColorVividViolet",         "#aa00ff", 2,0,3),
  PC("GColorWindsorTan",          "#aa5500", 2,1,0),
  PC("GColorRoseVale",            "#aa5555", 2,1,1),
  PC("GColorPurpureus",           "#aa55aa", 2,1,2),
  PC("GColorLavenderIndigo",      "#aa55ff", 2,1,3),
  PC("GColorLimerick",            "#aaaa00", 2,2,0),
  PC("GColorBrass",               "#aaaa55", 2,2,1),
  PC("GColorLightGray",           "#aaaaaa", 2,2,2),
  PC("GColorBabyBlueEyes",        "#aaaaff", 2,2,3),
  PC("GColorSpringBud",           "#aaff00", 2,3,0),
  PC("GColorInchworm",            "#aaff55", 2,3,1),
  PC("GColorMintGreen",           "#aaffaa", 2,3,2),
  PC("GColorCeleste",             "#aaffff", 2,3,3),
  PC("GColorRed",                 "#ff0000", 3,0,0),
  PC("GColorFolly",               "#ff0055", 3,0,1),
  PC("GColorFashionMagenta",      "#ff00aa", 3,0,2),
  PC("GColorMagenta",             "#ff00ff", 3,0,3),
  PC("GColorOrange",              "#ff5500", 3,1,0),
  PC("GColorSunsetOrange",        "#ff5555", 3,1,1),
  PC("GColorBrilliantRose",       "#ff55aa", 3,1,2),
  PC("GColorShockingPink",        "#ff55ff", 3,1,3),
  PC("GColorChromeYellow",        "#ffaa00", 3,2,0),
  PC("GColorRajah",               "#ffaa55", 3,2,1),
  PC("GColorMelon",               "#ffaaaa", 3,2,2),
  PC("GColorRichBrilliantLavender","#ffaaff",3,2,3),
  PC("GColorYellow",              "#ffff00", 3,3,0),
  PC("GColorIcterine",            "#ffff55", 3,3,1),
  PC("GColorPastelYellow",        "#ffffaa", 3,3,2),
  PC("GColorWhite",               "#ffffff", 3,3,3),
};
#define NUM_COLORS ((int)(sizeof(s_colors) / sizeof(s_colors[0])))

// ============================================================
// SAMPLE STRINGS (cycled in font detail screen)
// ============================================================
static const char *s_samples[] = {
  "09:42",
  "AaBbCcDd",
  "0123456789",
  "The quick fox",
};
#define NUM_SAMPLES 4

// ============================================================
// STATE
// ============================================================
static Window *s_main_window;
static MenuLayer *s_main_menu;

// Font detail
static Window *s_font_window;
static MenuLayer *s_font_menu;
static Window *s_font_detail_window;
static TextLayer *s_font_sample_layer;
static TextLayer *s_font_name_layer;
static int s_current_font = 0;
static int s_current_sample = 0;

// Color detail
static Window *s_color_window;
static MenuLayer *s_color_menu;
static Window *s_color_detail_window;
static Layer *s_color_fill_layer;
static TextLayer *s_color_info_layer;
static int s_current_color = 0;

// Platform
static Window *s_platform_window;
static TextLayer *s_platform_text_layer;

// ============================================================
// HELPERS
// ============================================================

// Returns true if a color is "light" enough that black text reads better
static bool color_is_light(const SamplerColor *c) {
  // Simple luminance check on 0..3 channels
  // Weights: R*2 + G*5 + B*1, threshold at mid (dark = sum < 9)
  return (c->r * 2 + c->g * 5 + c->b * 1) >= 9;
}

// ============================================================
// FONT DETAIL WINDOW
// ============================================================

static void font_detail_update(void) {
  const SamplerFont *f = &s_fonts[s_current_font];
  GFont font = fonts_get_system_font(f->key);
  text_layer_set_font(s_font_sample_layer, font);
  text_layer_set_text(s_font_sample_layer, s_samples[s_current_sample]);
  text_layer_set_text(s_font_name_layer, f->name);
}

static void font_detail_click(ClickRecognizerRef r, void *ctx) {
  s_current_sample = (s_current_sample + 1) % NUM_SAMPLES;
  font_detail_update();
}

static void font_detail_click_config(void *ctx) {
  window_single_click_subscribe(BUTTON_ID_SELECT, font_detail_click);
}

static void font_detail_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  // Sample text takes most of the screen, name bar at bottom
  int name_h = 20;
  GRect sample_rect = GRect(4, 4, bounds.size.w - 8, bounds.size.h - name_h - 8);
  GRect name_rect   = GRect(0, bounds.size.h - name_h, bounds.size.w, name_h);

  s_font_sample_layer = text_layer_create(sample_rect);
  text_layer_set_overflow_mode(s_font_sample_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(s_font_sample_layer, GTextAlignmentCenter);
  layer_add_child(root, text_layer_get_layer(s_font_sample_layer));

  s_font_name_layer = text_layer_create(name_rect);
  text_layer_set_font(s_font_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_font_name_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_font_name_layer, GColorBlack);
  text_layer_set_text_color(s_font_name_layer, GColorWhite);
  layer_add_child(root, text_layer_get_layer(s_font_name_layer));

  window_set_click_config_provider(window, font_detail_click_config);
  font_detail_update();
}

static void font_detail_unload(Window *window) {
  text_layer_destroy(s_font_sample_layer);
  text_layer_destroy(s_font_name_layer);
}

// ============================================================
// FONT LIST WINDOW
// ============================================================

static uint16_t font_menu_num_rows(MenuLayer *ml, uint16_t sec, void *ctx) {
  return NUM_FONTS;
}

static int16_t font_menu_cell_height(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  // Give each row enough room for the font to breathe — tallest fonts need ~52px
  return 52;
}

static void font_menu_draw_row(GContext *ctx, const Layer *cell, MenuIndex *idx, void *cbctx) {
  int row = idx->row;
  const SamplerFont *f = &s_fonts[row];
  GRect bounds = layer_get_bounds(cell);

  // Draw the font name in its own font, vertically centered
  GFont font = fonts_get_system_font(f->key);
  int text_h = 36; // conservative max height
  int y = (bounds.size.h - text_h) / 2;
  if (y < 0) y = 0;
  graphics_draw_text(ctx, f->name, font,
    GRect(6, y, bounds.size.w - 12, bounds.size.h - y),
    GTextOverflowModeFill, GTextAlignmentLeft, NULL);
}

static void font_menu_select(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  s_current_font   = idx->row;
  s_current_sample = 0;
  window_stack_push(s_font_detail_window, true);
}

static void font_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  s_font_menu = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_font_menu, NULL, (MenuLayerCallbacks){
    .get_num_rows   = font_menu_num_rows,
    .get_cell_height = font_menu_cell_height,
    .draw_row       = font_menu_draw_row,
    .select_click   = font_menu_select,
  });
  menu_layer_set_click_config_onto_window(s_font_menu, window);
  layer_add_child(root, menu_layer_get_layer(s_font_menu));
}

static void font_window_unload(Window *window) {
  menu_layer_destroy(s_font_menu);
}

// ============================================================
// COLOR DETAIL WINDOW
// ============================================================

static void color_fill_draw(Layer *layer, GContext *ctx) {
  const SamplerColor *c = &s_colors[s_current_color];
#if defined(PBL_COLOR)
  graphics_context_set_fill_color(ctx, c->color);
#else
  graphics_context_set_fill_color(ctx, GColorWhite);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void color_detail_update(void) {
  const SamplerColor *c = &s_colors[s_current_color];
  static char buf[64];
  snprintf(buf, sizeof(buf), "%s\n%s\nR:%d G:%d B:%d",
           c->name, c->hex, (int)c->r, (int)c->g, (int)c->b);
  text_layer_set_text(s_color_info_layer, buf);

#if defined(PBL_COLOR)
  bool light = color_is_light(c);
  text_layer_set_text_color(s_color_info_layer, light ? GColorBlack : GColorWhite);
  text_layer_set_background_color(s_color_info_layer, GColorClear);
#else
  text_layer_set_text_color(s_color_info_layer, GColorBlack);
#endif
  layer_mark_dirty(s_color_fill_layer);
}

static void color_detail_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  s_color_fill_layer = layer_create(bounds);
  layer_set_update_proc(s_color_fill_layer, color_fill_draw);
  layer_add_child(root, s_color_fill_layer);

  // Centered info text
  int info_h = 72;
  GRect info_rect = GRect(8, (bounds.size.h - info_h) / 2, bounds.size.w - 16, info_h);
  s_color_info_layer = text_layer_create(info_rect);
  text_layer_set_font(s_color_info_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_color_info_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_color_info_layer, GTextOverflowModeWordWrap);
  layer_add_child(root, text_layer_get_layer(s_color_info_layer));

  color_detail_update();
}

static void color_detail_unload(Window *window) {
  layer_destroy(s_color_fill_layer);
  text_layer_destroy(s_color_info_layer);
}

// ============================================================
// COLOR LIST WINDOW
// ============================================================

static uint16_t color_menu_num_rows(MenuLayer *ml, uint16_t sec, void *ctx) {
  return NUM_COLORS;
}

static int16_t color_menu_cell_height(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  return 28;
}

static void color_menu_draw_row(GContext *ctx, const Layer *cell, MenuIndex *idx, void *cbctx) {
  int row = idx->row;
  const SamplerColor *c = &s_colors[row];
  GRect bounds = layer_get_bounds(cell);

  // Swatch: 20x20 filled rect, 4px from left, vertically centered
  int sw = 20, sh = 20;
  int sy = (bounds.size.h - sh) / 2;
  GRect swatch = GRect(4, sy, sw, sh);

#if defined(PBL_COLOR)
  graphics_context_set_fill_color(ctx, c->color);
  graphics_fill_rect(ctx, swatch, 2, GCornersAll);
  // thin border so white/near-white swatches are visible
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_round_rect(ctx, swatch, 2);
#else
  // B&W: just draw a bordered empty rect
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, swatch);
#endif

  // Name + hex  
  GFont small = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  // Short name: drop "GColor" prefix
  const char *short_name = c->name + 6; // skip "GColor"
  char label[32];
  snprintf(label, sizeof(label), "%s  %s", short_name, c->hex);
  int tx = sw + 8;
  graphics_draw_text(ctx, label, small,
    GRect(tx, sy - 1, bounds.size.w - tx - 4, bounds.size.h),
    GTextOverflowModeFill, GTextAlignmentLeft, NULL);
}

static void color_menu_select(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  s_current_color = idx->row;
  window_stack_push(s_color_detail_window, true);
}

static void color_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  s_color_menu = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_color_menu, NULL, (MenuLayerCallbacks){
    .get_num_rows    = color_menu_num_rows,
    .get_cell_height = color_menu_cell_height,
    .draw_row        = color_menu_draw_row,
    .select_click    = color_menu_select,
  });
  menu_layer_set_click_config_onto_window(s_color_menu, window);
  layer_add_child(root, menu_layer_get_layer(s_color_menu));
}

static void color_window_unload(Window *window) {
  menu_layer_destroy(s_color_menu);
}

// ============================================================
// PLATFORM WINDOW
// ============================================================

static void platform_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  s_platform_text_layer = text_layer_create(
    GRect(6, 8, bounds.size.w - 12, bounds.size.h - 16));
  text_layer_set_font(s_platform_text_layer,
    fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_overflow_mode(s_platform_text_layer, GTextOverflowModeWordWrap);

  // Build platform info string at compile time
  static char info[256];

#if defined(PBL_PLATFORM_EMERY)
  snprintf(info, sizeof(info),
    "Platform\nEmery\n\n200 x 228\nRect\n64-color\nHealth: Yes");
#elif defined(PBL_PLATFORM_BASALT)
  snprintf(info, sizeof(info),
    "Platform\nBasalt\n\n144 x 168\nRect\n64-color\nHealth: Yes");
#elif defined(PBL_PLATFORM_CHALK)
  snprintf(info, sizeof(info),
    "Platform\nChalk\n\n180 x 180\nRound\n64-color\nHealth: Yes");
#elif defined(PBL_PLATFORM_DIORITE)
  snprintf(info, sizeof(info),
    "Platform\nDiorite\n\n144 x 168\nRect\nB&W\nHealth: Yes");
#elif defined(PBL_PLATFORM_APLITE)
  snprintf(info, sizeof(info),
    "Platform\nAplite\n\n144 x 168\nRect\nB&W\nHealth: No");
#elif defined(PBL_PLATFORM_FLINT)
  snprintf(info, sizeof(info),
    "Platform\nFlint\n\n144 x 168\nRect\nB&W\nHealth: No");
#elif defined(PBL_PLATFORM_GABBRO)
  snprintf(info, sizeof(info),
    "Platform\nGabbro\n\n260 x 260\nRound\n64-color\nHealth: Yes");
#else
  snprintf(info, sizeof(info), "Platform\nUnknown");
#endif

  text_layer_set_text(s_platform_text_layer, info);
  layer_add_child(root, text_layer_get_layer(s_platform_text_layer));
}

static void platform_window_unload(Window *window) {
  text_layer_destroy(s_platform_text_layer);
}

// ============================================================
// MAIN MENU
// ============================================================

static uint16_t main_menu_num_rows(MenuLayer *ml, uint16_t sec, void *ctx) {
  return 3;
}

static void main_menu_draw_row(GContext *ctx, const Layer *cell, MenuIndex *idx, void *cbctx) {
  static const char *titles[] = { "Fonts", "Colors", "Platform" };
  static const char *subs[]   = {
    "System font specimen",
    "64-color palette",
    "Device info"
  };
  menu_cell_basic_draw(ctx, cell, titles[idx->row], subs[idx->row], NULL);
}

static void main_menu_select(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  switch (idx->row) {
    case 0: window_stack_push(s_font_window,     true); break;
    case 1: window_stack_push(s_color_window,    true); break;
    case 2: window_stack_push(s_platform_window, true); break;
  }
}

static void main_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  s_main_menu = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_main_menu, NULL, (MenuLayerCallbacks){
    .get_num_rows = main_menu_num_rows,
    .draw_row     = main_menu_draw_row,
    .select_click = main_menu_select,
  });
  menu_layer_set_click_config_onto_window(s_main_menu, window);
  layer_add_child(root, menu_layer_get_layer(s_main_menu));
}

static void main_window_unload(Window *window) {
  menu_layer_destroy(s_main_menu);
}

// ============================================================
// APP LIFECYCLE
// ============================================================

static Window *make_window(WindowHandler load, WindowHandler unload) {
  Window *w = window_create();
  window_set_window_handlers(w, (WindowHandlers){ .load = load, .unload = unload });
  return w;
}

static void init(void) {
  s_main_window         = make_window(main_window_load,         main_window_unload);
  s_font_window         = make_window(font_window_load,         font_window_unload);
  s_font_detail_window  = make_window(font_detail_load,         font_detail_unload);
  s_color_window        = make_window(color_window_load,        color_window_unload);
  s_color_detail_window = make_window(color_detail_load,        color_detail_unload);
  s_platform_window     = make_window(platform_window_load,     platform_window_unload);

  window_stack_push(s_main_window, true);
}

static void deinit(void) {
  window_destroy(s_main_window);
  window_destroy(s_font_window);
  window_destroy(s_font_detail_window);
  window_destroy(s_color_window);
  window_destroy(s_color_detail_window);
  window_destroy(s_platform_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

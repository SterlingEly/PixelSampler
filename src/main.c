#include <pebble.h>

// ============================================================
// Pixel Sampler — main.c
// v1.1
// ============================================================

// ============================================================
// FONT TABLE  (alphabetical by family, then size)
// ============================================================
typedef struct {
  const char *name;   // display name shown in list
  const char *key;    // FONT_KEY_* string
  const char *tag;    // short tag shown as subtitle, e.g. "FONT_KEY_GOTHIC_14"
} SamplerFont;

static const SamplerFont s_fonts[] = {
  // Bitham
  { "Bitham 30 Black",          FONT_KEY_BITHAM_30_BLACK,           "BITHAM_30_BLACK"           },
  { "Bitham 34 Med Numbers",    FONT_KEY_BITHAM_34_MEDIUM_NUMBERS,  "BITHAM_34_MEDIUM_NUMBERS"  },
  { "Bitham 42 Bold",           FONT_KEY_BITHAM_42_BOLD,            "BITHAM_42_BOLD"            },
  { "Bitham 42 Light",          FONT_KEY_BITHAM_42_LIGHT,           "BITHAM_42_LIGHT"           },
  { "Bitham 42 Med Numbers",    FONT_KEY_BITHAM_42_MEDIUM_NUMBERS,  "BITHAM_42_MEDIUM_NUMBERS"  },
  // Droid
  { "Droid Serif 28 Bold",      FONT_KEY_DROID_SERIF_28_BOLD,       "DROID_SERIF_28_BOLD"       },
  // Gothic
  { "Gothic 14",                FONT_KEY_GOTHIC_14,                 "GOTHIC_14"                 },
  { "Gothic 14 Bold",           FONT_KEY_GOTHIC_14_BOLD,            "GOTHIC_14_BOLD"            },
  { "Gothic 18",                FONT_KEY_GOTHIC_18,                 "GOTHIC_18"                 },
  { "Gothic 18 Bold",           FONT_KEY_GOTHIC_18_BOLD,            "GOTHIC_18_BOLD"            },
  { "Gothic 24",                FONT_KEY_GOTHIC_24,                 "GOTHIC_24"                 },
  { "Gothic 24 Bold",           FONT_KEY_GOTHIC_24_BOLD,            "GOTHIC_24_BOLD"            },
  { "Gothic 28",                FONT_KEY_GOTHIC_28,                 "GOTHIC_28"                 },
  { "Gothic 28 Bold",           FONT_KEY_GOTHIC_28_BOLD,            "GOTHIC_28_BOLD"            },
  // LECO
  { "LECO 20 Bold",             FONT_KEY_LECO_20_BOLD_NUMBERS,      "LECO_20_BOLD_NUMBERS"      },
  { "LECO 26 Bold AM/PM",       FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM,"LECO_26_BOLD_NUMBERS_AM_PM"},
  { "LECO 28 Light",            FONT_KEY_LECO_28_LIGHT_NUMBERS,     "LECO_28_LIGHT_NUMBERS"     },
  { "LECO 32 Bold",             FONT_KEY_LECO_32_BOLD_NUMBERS,      "LECO_32_BOLD_NUMBERS"      },
  { "LECO 36 Bold",             FONT_KEY_LECO_36_BOLD_NUMBERS,      "LECO_36_BOLD_NUMBERS"      },
  { "LECO 38 Bold",             FONT_KEY_LECO_38_BOLD_NUMBERS,      "LECO_38_BOLD_NUMBERS"      },
  { "LECO 42",                  FONT_KEY_LECO_42_NUMBERS,           "LECO_42_NUMBERS"           },
  { "LECO 60 AM/PM *",          FONT_KEY_LECO_60_NUMBERS_AM_PM,     "LECO_60_NUMBERS_AM_PM"     },
  { "LECO 60 Bold AM/PM *",     FONT_KEY_LECO_60_BOLD_NUMBERS_AM_PM,"LECO_60_BOLD_NUMBERS_AM_PM"},
  // Roboto
  { "Roboto Bold 49",           FONT_KEY_ROBOTO_BOLD_SUBSET_49,     "ROBOTO_BOLD_SUBSET_49"     },
  { "Roboto Condensed 21",      FONT_KEY_ROBOTO_CONDENSED_21,       "ROBOTO_CONDENSED_21"       },
};
#define NUM_FONTS ((int)(sizeof(s_fonts) / sizeof(s_fonts[0])))

// Sample strings shown on the font detail screen (all visible at once)
static const char *s_samples[] = {
  "09:42",
  "0123456789",
  "AaBbCcDd",
  "The quick fox",
  "!@#$%^&*()",
};
#define NUM_SAMPLES 5

// ============================================================
// COLOR TABLE  (64 Pebble palette, spectrum order)
// Hue sweep: black → violet/blue → cyan/green → yellow/red → white
// Within each hue column: dark → mid → bright
// ============================================================
typedef struct {
  const char *name;   // full GColor name
  const char *hex;
  uint8_t r, g, b;   // 2-bit channels (0..3)
  GColor   color;
} SamplerColor;

#define PC(nm, hx, r2, g2, b2) \
  { nm, hx, r2, g2, b2, {.argb = (uint8_t)(0b11000000 | ((r2)<<4) | ((g2)<<2) | (b2))} }

static const SamplerColor s_colors[] = {
  // ---- Achromatic: black → grays → white ----
  PC("GColorBlack",                "#000000", 0,0,0),
  PC("GColorDarkGray",             "#555555", 1,1,1),
  PC("GColorLightGray",            "#aaaaaa", 2,2,2),
  PC("GColorWhite",                "#ffffff", 3,3,3),

  // ---- Blues (R=0, G=0, B++) ----
  PC("GColorOxfordBlue",           "#000055", 0,0,1),
  PC("GColorDukeBlue",             "#0000aa", 0,0,2),
  PC("GColorBlue",                 "#0000ff", 0,0,3),

  // ---- Blue-purples (R=0, G=0→low, B=high) ----
  PC("GColorElectricUltramarine",  "#5500ff", 1,0,3),
  PC("GColorIndigo",               "#5500aa", 1,0,2),
  PC("GColorImperialPurple",       "#550055", 1,0,1),

  // ---- Purples & violets (R++, B=high) ----
  PC("GColorVividViolet",          "#aa00ff", 2,0,3),
  PC("GColorPurple",               "#aa00aa", 2,0,2),
  PC("GColorJazzberryJam",         "#aa0055", 2,0,1),
  PC("GColorBulgarianRose",        "#550000", 1,0,0),
  PC("GColorDarkCandyAppleRed",    "#aa0000", 2,0,0),

  // ---- Reds ----
  PC("GColorRed",                  "#ff0000", 3,0,0),
  PC("GColorFolly",                "#ff0055", 3,0,1),
  PC("GColorFashionMagenta",       "#ff00aa", 3,0,2),
  PC("GColorMagenta",              "#ff00ff", 3,0,3),

  // ---- Pink / magentas ----
  PC("GColorShockingPink",         "#ff55ff", 3,1,3),
  PC("GColorBrilliantRose",        "#ff55aa", 3,1,2),
  PC("GColorSunsetOrange",         "#ff5555", 3,1,1),
  PC("GColorOrange",               "#ff5500", 3,1,0),

  // ---- Orange → yellow ----
  PC("GColorChromeYellow",         "#ffaa00", 3,2,0),
  PC("GColorRajah",                "#ffaa55", 3,2,1),
  PC("GColorMelon",                "#ffaaaa", 3,2,2),
  PC("GColorRichBrilliantLavender","#ffaaff", 3,2,3),
  PC("GColorYellow",               "#ffff00", 3,3,0),
  PC("GColorIcterine",             "#ffff55", 3,3,1),
  PC("GColorPastelYellow",         "#ffffaa", 3,3,2),

  // ---- Yellow-greens ----
  PC("GColorArmyGreen",            "#555500", 1,1,0),
  PC("GColorLimerick",             "#aaaa00", 2,2,0),
  PC("GColorBrass",                "#aaaa55", 2,2,1),
  PC("GColorBabyBlueEyes",         "#aaaaff", 2,2,3),
  PC("GColorSpringBud",            "#aaff00", 2,3,0),
  PC("GColorInchworm",             "#aaff55", 2,3,1),
  PC("GColorKellyGreen",           "#55aa00", 1,2,0),
  PC("GColorBrightGreen",          "#55ff00", 1,3,0),

  // ---- Greens ----
  PC("GColorDarkGreen",            "#005500", 0,1,0),
  PC("GColorIslamicGreen",         "#00aa00", 0,2,0),
  PC("GColorGreen",                "#00ff00", 0,3,0),
  PC("GColorScreaminGreen",        "#55ff55", 1,3,1),
  PC("GColorMayGreen",             "#55aa55", 1,2,1),

  // ---- Blue-greens / teals ----
  PC("GColorMalachite",            "#00ff55", 0,3,1),
  PC("GColorMediumAquamarine",     "#55ffaa", 1,3,2),
  PC("GColorMintGreen",            "#aaffaa", 2,3,2),
  PC("GColorJaegerGreen",          "#00aa55", 0,2,1),
  PC("GColorMidnightGreen",        "#005555", 0,1,1),
  PC("GColorMediumSpringGreen",    "#00ffaa", 0,3,2),
  PC("GColorElectricBlue",         "#55ffff", 1,3,3),
  PC("GColorCeleste",              "#aaffff", 2,3,3),

  // ---- Cyans ----
  PC("GColorCyan",                 "#00ffff", 0,3,3),
  PC("GColorTiffanyBlue",          "#00aaaa", 0,2,2),
  PC("GColorCadetBlue",            "#55aaaa", 1,2,2),

  // ---- Cerulean / sky blues ----
  PC("GColorVividCerulean",        "#00aaff", 0,2,3),
  PC("GColorPictonBlue",           "#55aaff", 1,2,3),
  PC("GColorBlueMoon",             "#0055ff", 0,1,3),
  PC("GColorCobaltBlue",           "#0055aa", 0,1,2),

  // ---- Periwinkles / blue-violets ----
  PC("GColorLiberty",              "#5555aa", 1,1,2),
  PC("GColorVeryLightBlue",        "#5555ff", 1,1,3),
  PC("GColorLavenderIndigo",       "#aa55ff", 2,1,3),
  PC("GColorPurpureus",            "#aa55aa", 2,1,2),
  PC("GColorRoseVale",             "#aa5555", 2,1,1),
  PC("GColorWindsorTan",           "#aa5500", 2,1,0),
};
#define NUM_COLORS ((int)(sizeof(s_colors) / sizeof(s_colors[0])))

// B&W platforms: show just the 4 representable levels
// (All 64 colors map to one of these when dithering is ignored)
static const SamplerColor s_bw_colors[] = {
  PC("GColorBlack",     "#000000", 0,0,0),
  PC("GColorDarkGray",  "#555555", 1,1,1),
  PC("GColorLightGray", "#aaaaaa", 2,2,2),
  PC("GColorWhite",     "#ffffff", 3,3,3),
};
#define NUM_BW_COLORS 4

// ============================================================
// STATE
// ============================================================
static Window    *s_main_window;
static MenuLayer *s_main_menu;

// Fonts
static Window      *s_font_window;
static MenuLayer   *s_font_menu;
static Window      *s_font_detail_window;
static ScrollLayer *s_font_scroll_layer;
static Layer       *s_font_canvas_layer;
static int          s_current_font = 0;

// Colors
static Window    *s_color_window;
static MenuLayer *s_color_menu;
static Window    *s_color_detail_window;
static Layer     *s_color_fill_layer;
static TextLayer *s_color_info_layer;
static int        s_current_color = 0;

// Platform
static Window    *s_platform_window;
static ScrollLayer *s_platform_scroll;
static TextLayer *s_platform_text_layer;

// ============================================================
// HELPERS
// ============================================================
static bool color_is_light(const SamplerColor *c) {
  return (c->r * 2 + c->g * 5 + c->b) >= 9;
}

static const SamplerColor *get_color(int idx) {
#if defined(PBL_COLOR)
  return &s_colors[idx];
#else
  return &s_bw_colors[idx];
#endif
}

static int num_colors(void) {
#if defined(PBL_COLOR)
  return NUM_COLORS;
#else
  return NUM_BW_COLORS;
#endif
}

// ============================================================
// FONT DETAIL WINDOW
// Font detail: ScrollLayer containing all sample strings
// stacked vertically, each rendered in the chosen font.
// ============================================================

#define FONT_DETAIL_PADDING  6
#define FONT_DETAIL_LINE_GAP 4

// We draw all samples onto a single canvas layer inside the scroll layer.
// Canvas height is computed once and stored.
static int16_t s_font_canvas_h = 0;

static void font_canvas_draw(Layer *layer, GContext *ctx) {
  const SamplerFont *f = &s_fonts[s_current_font];
  GFont specimen = fonts_get_system_font(f->key);
  GFont label    = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  GRect bounds   = layer_get_bounds(layer);
  int w = bounds.size.w;
  int y = FONT_DETAIL_PADDING;

  // Header: font name in Gothic 14 Bold, dark bar
  GFont hdr = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  int hdr_h = 18;
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, w, hdr_h + 2), 0, GCornerNone);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, f->name, hdr,
    GRect(4, 1, w - 8, hdr_h),
    GTextOverflowModeFill, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, f->tag, label,
    GRect(4, hdr_h + 2, w - 8, 14),
    GTextOverflowModeFill, GTextAlignmentLeft, NULL);
  y = hdr_h + 16 + FONT_DETAIL_PADDING;

  // Divider
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_draw_line(ctx, GPoint(0, y - 4), GPoint(w, y - 4));

  // Each sample string rendered in the specimen font
  graphics_context_set_text_color(ctx, GColorBlack);
  for (int i = 0; i < NUM_SAMPLES; i++) {
    // Measure the text height so we can advance y correctly
    GSize sz = graphics_text_layout_get_content_size(
      s_samples[i], specimen,
      GRect(0, 0, w - 2 * FONT_DETAIL_PADDING, 200),
      GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int line_h = sz.h + FONT_DETAIL_LINE_GAP;
    if (line_h < 14) line_h = 14;

    graphics_draw_text(ctx, s_samples[i], specimen,
      GRect(FONT_DETAIL_PADDING, y, w - 2 * FONT_DETAIL_PADDING, line_h + 4),
      GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    y += line_h + FONT_DETAIL_LINE_GAP;

    // Thin rule between samples
    if (i < NUM_SAMPLES - 1) {
      graphics_context_set_stroke_color(ctx, GColorLightGray);
      graphics_draw_line(ctx, GPoint(FONT_DETAIL_PADDING, y), GPoint(w - FONT_DETAIL_PADDING, y));
      y += 3;
    }
  }
}

// Compute the total canvas height needed for the current font
static int16_t compute_font_canvas_height(int w) {
  const SamplerFont *f = &s_fonts[s_current_font];
  GFont specimen = fonts_get_system_font(f->key);
  int hdr_h = 18 + 16 + FONT_DETAIL_PADDING * 2 + 4;
  int y = hdr_h;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    GSize sz = graphics_text_layout_get_content_size(
      s_samples[i], specimen,
      GRect(0, 0, w - 2 * FONT_DETAIL_PADDING, 200),
      GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int line_h = sz.h + FONT_DETAIL_LINE_GAP;
    if (line_h < 14) line_h = 14;
    y += line_h + FONT_DETAIL_LINE_GAP;
    if (i < NUM_SAMPLES - 1) y += 3;
  }
  return (int16_t)(y + FONT_DETAIL_PADDING);
}

static void font_detail_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);

  s_font_canvas_h = compute_font_canvas_height(bounds.size.w);
  if (s_font_canvas_h < bounds.size.h) s_font_canvas_h = bounds.size.h;

  s_font_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_content_size(s_font_scroll_layer,
    GSize(bounds.size.w, s_font_canvas_h));
  scroll_layer_set_click_config_onto_window(s_font_scroll_layer, window);
  layer_add_child(root, scroll_layer_get_layer(s_font_scroll_layer));

  s_font_canvas_layer = layer_create(
    GRect(0, 0, bounds.size.w, s_font_canvas_h));
  layer_set_update_proc(s_font_canvas_layer, font_canvas_draw);
  scroll_layer_add_child(s_font_scroll_layer, s_font_canvas_layer);
}

static void font_detail_unload(Window *window) {
  layer_destroy(s_font_canvas_layer);
  scroll_layer_destroy(s_font_scroll_layer);
}

// ============================================================
// FONT LIST WINDOW
// ============================================================
static uint16_t font_menu_num_rows(MenuLayer *ml, uint16_t sec, void *ctx) {
  return NUM_FONTS;
}

static int16_t font_menu_cell_height(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  return 36;
}

static void font_menu_draw_row(GContext *ctx, const Layer *cell,
                               MenuIndex *idx, void *cbctx) {
  const SamplerFont *f = &s_fonts[idx->row];
  // All rows use Gothic 24 Bold for the name — consistent, legible
  menu_cell_basic_draw(ctx, cell, f->name, f->tag, NULL);
}

static void font_menu_select(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  s_current_font = idx->row;
  window_stack_push(s_font_detail_window, true);
}

static void font_window_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);
  s_font_menu   = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_font_menu, NULL, (MenuLayerCallbacks){
    .get_num_rows    = font_menu_num_rows,
    .get_cell_height = font_menu_cell_height,
    .draw_row        = font_menu_draw_row,
    .select_click    = font_menu_select,
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
  const SamplerColor *c = get_color(s_current_color);
#if defined(PBL_COLOR)
  graphics_context_set_fill_color(ctx, c->color);
#else
  graphics_context_set_fill_color(ctx,
    gcolor_equal(c->color, GColorBlack)     ? GColorBlack :
    gcolor_equal(c->color, GColorDarkGray)  ? GColorDarkGray :
    gcolor_equal(c->color, GColorLightGray) ? GColorLightGray :
                                              GColorWhite);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void color_detail_update(void) {
  const SamplerColor *c = get_color(s_current_color);
  static char buf[80];
  snprintf(buf, sizeof(buf), "%s\n%s\nR:%d  G:%d  B:%d",
           c->name, c->hex, (int)c->r, (int)c->g, (int)c->b);
  text_layer_set_text(s_color_info_layer, buf);
#if defined(PBL_COLOR)
  bool light = color_is_light(c);
  text_layer_set_text_color(s_color_info_layer,
    light ? GColorBlack : GColorWhite);
  text_layer_set_background_color(s_color_info_layer, GColorClear);
#else
  // On B&W, use inverted text so it's readable on any fill
  bool is_black = gcolor_equal(c->color, GColorBlack) ||
                  gcolor_equal(c->color, GColorDarkGray);
  text_layer_set_text_color(s_color_info_layer,
    is_black ? GColorWhite : GColorBlack);
  text_layer_set_background_color(s_color_info_layer, GColorClear);
#endif
  layer_mark_dirty(s_color_fill_layer);
}

static void color_detail_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);

  s_color_fill_layer = layer_create(bounds);
  layer_set_update_proc(s_color_fill_layer, color_fill_draw);
  layer_add_child(root, s_color_fill_layer);

  int info_h = 72;
  GRect info_rect = GRect(8, (bounds.size.h - info_h) / 2,
                          bounds.size.w - 16, info_h);
  s_color_info_layer = text_layer_create(info_rect);
  text_layer_set_font(s_color_info_layer,
    fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
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
  return num_colors();
}

static int16_t color_menu_cell_height(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  return 28;
}

static void color_menu_draw_row(GContext *ctx, const Layer *cell,
                                MenuIndex *idx, void *cbctx) {
  const SamplerColor *c = get_color(idx->row);
  GRect bounds = layer_get_bounds(cell);

  int sw = 20, sh = 20;
  int sy = (bounds.size.h - sh) / 2;
  GRect swatch = GRect(4, sy, sw, sh);

#if defined(PBL_COLOR)
  graphics_context_set_fill_color(ctx, c->color);
  graphics_fill_rect(ctx, swatch, 2, GCornersAll);
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_round_rect(ctx, swatch, 2);
#else
  // B&W: fill with the actual gray level, border in black
  GColor fill = gcolor_equal(c->color, GColorBlack)     ? GColorBlack :
                gcolor_equal(c->color, GColorDarkGray)  ? GColorDarkGray :
                gcolor_equal(c->color, GColorLightGray) ? GColorLightGray :
                                                          GColorWhite;
  graphics_context_set_fill_color(ctx, fill);
  graphics_fill_rect(ctx, swatch, 0, GCornerNone);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, swatch);
#endif

  // Name only (drop "GColor" prefix for brevity)
  GFont small = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  const char *short_name = c->name + 6;  // skip "GColor"
  int tx = sw + 8;
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, short_name, small,
    GRect(tx, sy - 1, bounds.size.w - tx - 4, bounds.size.h),
    GTextOverflowModeFill, GTextAlignmentLeft, NULL);
}

static void color_menu_select(MenuLayer *ml, MenuIndex *idx, void *ctx) {
  s_current_color = idx->row;
  window_stack_push(s_color_detail_window, true);
}

static void color_window_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);
  s_color_menu  = menu_layer_create(bounds);
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
// PLATFORM WINDOW  (scrollable text)
// ============================================================
static void platform_window_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);

  static char info[512];

#if defined(PBL_PLATFORM_EMERY)
  snprintf(info, sizeof(info),
    "Emery\n"
    "Pebble Time 2\n"
    "\n"
    "Screen\n200 x 228 px\n"
    "\n"
    "Shape\nRectangle\n"
    "\n"
    "Color\n64-color\n"
    "\n"
    "Health\nSteps, Sleep,\nHR, Calories\n"
    "\n"
    "* LECO 60 sizes\n  available here");
#elif defined(PBL_PLATFORM_BASALT)
  snprintf(info, sizeof(info),
    "Basalt\n"
    "Pebble Time\n"
    "\n"
    "Screen\n144 x 168 px\n"
    "\n"
    "Shape\nRectangle\n"
    "\n"
    "Color\n64-color\n"
    "\n"
    "Health\nSteps, Sleep,\nHR, Calories");
#elif defined(PBL_PLATFORM_CHALK)
  snprintf(info, sizeof(info),
    "Chalk\n"
    "Pebble Time Round\n"
    "\n"
    "Screen\n180 x 180 px\n"
    "\n"
    "Shape\nRound\n"
    "\n"
    "Color\n64-color\n"
    "\n"
    "Health\nSteps, Sleep,\nHR, Calories");
#elif defined(PBL_PLATFORM_DIORITE)
  snprintf(info, sizeof(info),
    "Diorite\n"
    "Pebble 2\n"
    "\n"
    "Screen\n144 x 168 px\n"
    "\n"
    "Shape\nRectangle\n"
    "\n"
    "Color\nBlack & White\n"
    "\n"
    "Health\nSteps, Sleep,\nHR, Calories");
#elif defined(PBL_PLATFORM_APLITE)
  snprintf(info, sizeof(info),
    "Aplite\n"
    "Pebble OG / Steel\n"
    "\n"
    "Screen\n144 x 168 px\n"
    "\n"
    "Shape\nRectangle\n"
    "\n"
    "Color\nBlack & White\n"
    "\n"
    "Health\nNone");
#elif defined(PBL_PLATFORM_FLINT)
  snprintf(info, sizeof(info),
    "Flint\n"
    "Pebble Time Round 14\n"
    "\n"
    "Screen\n144 x 168 px\n"
    "\n"
    "Shape\nRectangle\n"
    "\n"
    "Color\nBlack & White\n"
    "\n"
    "Health\nSteps, Sleep,\nCalories\n(no HR)");
#elif defined(PBL_PLATFORM_GABBRO)
  snprintf(info, sizeof(info),
    "Gabbro\n"
    "Pebble Round 2\n"
    "\n"
    "Screen\n260 x 260 px\n"
    "\n"
    "Shape\nRound\n"
    "\n"
    "Color\n64-color\n"
    "\n"
    "Health\nSteps, Sleep,\nHR, Calories\n"
    "\n"
    "* LECO 60 sizes\n  available here");
#else
  snprintf(info, sizeof(info), "Unknown platform");
#endif

  // Measure text height to set scroll content size
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GSize text_size = graphics_text_layout_get_content_size(
    info, font,
    GRect(0, 0, bounds.size.w - 12, 2000),
    GTextOverflowModeWordWrap, GTextAlignmentLeft);
  int content_h = text_size.h + 24;
  if (content_h < bounds.size.h) content_h = bounds.size.h;

  s_platform_scroll = scroll_layer_create(bounds);
  scroll_layer_set_content_size(s_platform_scroll,
    GSize(bounds.size.w, content_h));
  scroll_layer_set_click_config_onto_window(s_platform_scroll, window);
  layer_add_child(root, scroll_layer_get_layer(s_platform_scroll));

  s_platform_text_layer = text_layer_create(
    GRect(6, 8, bounds.size.w - 12, content_h - 8));
  text_layer_set_font(s_platform_text_layer, font);
  text_layer_set_overflow_mode(s_platform_text_layer,
    GTextOverflowModeWordWrap);
  text_layer_set_text(s_platform_text_layer, info);
  scroll_layer_add_child(s_platform_scroll,
    text_layer_get_layer(s_platform_text_layer));
}

static void platform_window_unload(Window *window) {
  text_layer_destroy(s_platform_text_layer);
  scroll_layer_destroy(s_platform_scroll);
}

// ============================================================
// MAIN MENU
// ============================================================
static uint16_t main_menu_num_rows(MenuLayer *ml, uint16_t sec, void *ctx) {
  return 3;
}

static void main_menu_draw_row(GContext *ctx, const Layer *cell,
                               MenuIndex *idx, void *cbctx) {
  static const char *titles[] = { "Fonts", "Colors", "Platform" };
  static const char *subs[] = {
    "25 system fonts",
#if defined(PBL_COLOR)
    "64-color palette",
#else
    "4 B&W levels",
#endif
    "Device info",
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
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);
  s_main_menu   = menu_layer_create(bounds);
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
  window_set_window_handlers(w, (WindowHandlers){
    .load = load, .unload = unload
  });
  return w;
}

static void init(void) {
  s_main_window         = make_window(main_window_load,     main_window_unload);
  s_font_window         = make_window(font_window_load,     font_window_unload);
  s_font_detail_window  = make_window(font_detail_load,     font_detail_unload);
  s_color_window        = make_window(color_window_load,    color_window_unload);
  s_color_detail_window = make_window(color_detail_load,    color_detail_unload);
  s_platform_window     = make_window(platform_window_load, platform_window_unload);
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

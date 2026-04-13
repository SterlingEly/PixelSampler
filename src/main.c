#include <pebble.h>

// ============================================================
// Pixel Sampler — main.c  v1.0
// A developer reference app: browse all system fonts, the full
// 64-color palette, and hardware platform info on your wrist.
// Supports all 7 Pebble platforms (aplite/basalt/chalk/diorite/
// emery/flint/gabbro).
// ============================================================

// ------------------------------------------------------------
// Round-screen geometry
// ROUND_INSET  : horizontal safe zone, equal to the narrowest
//               chord width near the top/bottom of the circle.
//               chalk (180px dia) -> 30px, gabbro (260px) -> 40px.
// ROUND_TOP_PAD: vertical offset so the first text row clears
//               the circular boundary at the top of the screen.
// ------------------------------------------------------------
#if defined(PBL_PLATFORM_GABBRO)
#  define ROUND_INSET    40
#  define ROUND_TOP_PAD  36
#elif defined(PBL_ROUND)
#  define ROUND_INSET    30
#  define ROUND_TOP_PAD  26
#else
#  define ROUND_INSET     0
#  define ROUND_TOP_PAD   0
#endif

// ============================================================
// FONT TABLE
// Alphabetical by family then size.
// LECO 60 variants only exist in emery/gabbro SDK headers.
// ============================================================
typedef struct { const char *name; const char *key; } SamplerFont;

static const SamplerFont s_fonts[] = {
  { "Bitham 30 Black",        FONT_KEY_BITHAM_30_BLACK            },
  { "Bitham 34 Med Numbers",  FONT_KEY_BITHAM_34_MEDIUM_NUMBERS   },
  { "Bitham 42 Bold",         FONT_KEY_BITHAM_42_BOLD             },
  { "Bitham 42 Light",        FONT_KEY_BITHAM_42_LIGHT            },
  { "Bitham 42 Med Numbers",  FONT_KEY_BITHAM_42_MEDIUM_NUMBERS   },
  { "Droid Serif 28 Bold",    FONT_KEY_DROID_SERIF_28_BOLD        },
  { "Gothic 14",              FONT_KEY_GOTHIC_14                  },
  { "Gothic 14 Bold",         FONT_KEY_GOTHIC_14_BOLD             },
  { "Gothic 18",              FONT_KEY_GOTHIC_18                  },
  { "Gothic 18 Bold",         FONT_KEY_GOTHIC_18_BOLD             },
  { "Gothic 24",              FONT_KEY_GOTHIC_24                  },
  { "Gothic 24 Bold",         FONT_KEY_GOTHIC_24_BOLD             },
  { "Gothic 28",              FONT_KEY_GOTHIC_28                  },
  { "Gothic 28 Bold",         FONT_KEY_GOTHIC_28_BOLD             },
  { "LECO 20 Bold",           FONT_KEY_LECO_20_BOLD_NUMBERS       },
  { "LECO 26 Bold AM/PM",     FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM },
  { "LECO 28 Light",          FONT_KEY_LECO_28_LIGHT_NUMBERS      },
  { "LECO 32 Bold",           FONT_KEY_LECO_32_BOLD_NUMBERS       },
  { "LECO 36 Bold",           FONT_KEY_LECO_36_BOLD_NUMBERS       },
  { "LECO 38 Bold",           FONT_KEY_LECO_38_BOLD_NUMBERS       },
  { "LECO 42",                FONT_KEY_LECO_42_NUMBERS            },
#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
  { "LECO 60 AM/PM",          FONT_KEY_LECO_60_NUMBERS_AM_PM      },
  { "LECO 60 Bold AM/PM",     FONT_KEY_LECO_60_BOLD_NUMBERS_AM_PM },
#endif
  { "Roboto Bold 49",         FONT_KEY_ROBOTO_BOLD_SUBSET_49      },
  { "Roboto Condensed 21",    FONT_KEY_ROBOTO_CONDENSED_21        },
};
#define NUM_FONTS ((int)(sizeof(s_fonts) / sizeof(s_fonts[0])))

// Five sample strings that together exercise time/digits/alpha/prose/symbols.
// Note: "Bitham *Med Numbers" and "LECO *" fonts only render digits and
// punctuation — alphabetic chars render as placeholder glyphs (by design).
static const char *s_samples[] = {
  "09:42", "0123456789", "AaBbCcDd", "The quick fox", "!@#$%&*()",
};
#define NUM_SAMPLES 5

// ============================================================
// COLOR TABLE
// 64 colors in spectrum order (grays, blue→violet→red→
// yellow→green→cyan).  B&W builds use 4 gray levels only.
//
// PC macro packs a GColor from 2-bit r/g/b channels:
//   argb byte = 0b11rrggbb  (alpha always fully opaque)
// ============================================================
typedef struct {
  const char *name;
  const char *hex;
  uint8_t     r, g, b;
  GColor      color;
} SamplerColor;

#define PC(nm, hx, r2, g2, b2) \
  { nm, hx, r2, g2, b2, \
    { .argb = (uint8_t)(0b11000000 | ((r2) << 4) | ((g2) << 2) | (b2)) } }

#if defined(PBL_COLOR)
static const SamplerColor s_colors[] = {
  // Grays
  PC("GColorBlack",                "#000000", 0,0,0),
  PC("GColorDarkGray",             "#555555", 1,1,1),
  PC("GColorLightGray",            "#aaaaaa", 2,2,2),
  PC("GColorWhite",                "#ffffff", 3,3,3),
  // Blues
  PC("GColorOxfordBlue",           "#000055", 0,0,1),
  PC("GColorDukeBlue",             "#0000aa", 0,0,2),
  PC("GColorBlue",                 "#0000ff", 0,0,3),
  // Blue-violet
  PC("GColorElectricUltramarine",  "#5500ff", 1,0,3),
  PC("GColorIndigo",               "#5500aa", 1,0,2),
  PC("GColorImperialPurple",       "#550055", 1,0,1),
  // Violets
  PC("GColorVividViolet",          "#aa00ff", 2,0,3),
  PC("GColorPurple",               "#aa00aa", 2,0,2),
  PC("GColorJazzberryJam",         "#aa0055", 2,0,1),
  // Dark reds
  PC("GColorBulgarianRose",        "#550000", 1,0,0),
  PC("GColorDarkCandyAppleRed",    "#aa0000", 2,0,0),
  // Reds / pinks
  PC("GColorRed",                  "#ff0000", 3,0,0),
  PC("GColorFolly",                "#ff0055", 3,0,1),
  PC("GColorFashionMagenta",       "#ff00aa", 3,0,2),
  PC("GColorMagenta",              "#ff00ff", 3,0,3),
  PC("GColorShockingPink",         "#ff55ff", 3,1,3),
  PC("GColorBrilliantRose",        "#ff55aa", 3,1,2),
  PC("GColorSunsetOrange",         "#ff5555", 3,1,1),
  PC("GColorOrange",               "#ff5500", 3,1,0),
  // Yellows / warm
  PC("GColorChromeYellow",         "#ffaa00", 3,2,0),
  PC("GColorRajah",                "#ffaa55", 3,2,1),
  PC("GColorMelon",                "#ffaaaa", 3,2,2),
  PC("GColorRichBrilliantLavender","#ffaaff", 3,2,3),
  PC("GColorYellow",               "#ffff00", 3,3,0),
  PC("GColorIcterine",             "#ffff55", 3,3,1),
  PC("GColorPastelYellow",         "#ffffaa", 3,3,2),
  // Olive / army
  PC("GColorArmyGreen",            "#555500", 1,1,0),
  PC("GColorLimerick",             "#aaaa00", 2,2,0),
  PC("GColorBrass",                "#aaaa55", 2,2,1),
  PC("GColorBabyBlueEyes",         "#aaaaff", 2,2,3),
  // Greens
  PC("GColorSpringBud",            "#aaff00", 2,3,0),
  PC("GColorInchworm",             "#aaff55", 2,3,1),
  PC("GColorKellyGreen",           "#55aa00", 1,2,0),
  PC("GColorBrightGreen",          "#55ff00", 1,3,0),
  PC("GColorDarkGreen",            "#005500", 0,1,0),
  PC("GColorIslamicGreen",         "#00aa00", 0,2,0),
  PC("GColorGreen",                "#00ff00", 0,3,0),
  PC("GColorScreaminGreen",        "#55ff55", 1,3,1),
  PC("GColorMayGreen",             "#55aa55", 1,2,1),
  PC("GColorMalachite",            "#00ff55", 0,3,1),
  PC("GColorMediumAquamarine",     "#55ffaa", 1,3,2),
  PC("GColorMintGreen",            "#aaffaa", 2,3,2),
  // Teals
  PC("GColorJaegerGreen",          "#00aa55", 0,2,1),
  PC("GColorMidnightGreen",        "#005555", 0,1,1),
  PC("GColorMediumSpringGreen",    "#00ffaa", 0,3,2),
  // Cyans / light blues
  PC("GColorElectricBlue",         "#55ffff", 1,3,3),
  PC("GColorCeleste",              "#aaffff", 2,3,3),
  PC("GColorCyan",                 "#00ffff", 0,3,3),
  PC("GColorTiffanyBlue",          "#00aaaa", 0,2,2),
  PC("GColorCadetBlue",            "#55aaaa", 1,2,2),
  PC("GColorVividCerulean",        "#00aaff", 0,2,3),
  PC("GColorPictonBlue",           "#55aaff", 1,2,3),
  PC("GColorBlueMoon",             "#0055ff", 0,1,3),
  PC("GColorCobaltBlue",           "#0055aa", 0,1,2),
  // Blue-purples
  PC("GColorLiberty",              "#5555aa", 1,1,2),
  PC("GColorVeryLightBlue",        "#5555ff", 1,1,3),
  PC("GColorLavenderIndigo",       "#aa55ff", 2,1,3),
  PC("GColorPurpureus",            "#aa55aa", 2,1,2),
  PC("GColorRoseVale",             "#aa5555", 2,1,1),
  PC("GColorWindsorTan",           "#aa5500", 2,1,0),
};
#define NUM_COLORS ((int)(sizeof(s_colors) / sizeof(s_colors[0])))
#else
static const SamplerColor s_colors[] = {
  PC("GColorBlack",    "#000000", 0,0,0),
  PC("GColorDarkGray", "#555555", 1,1,1),
  PC("GColorLightGray","#aaaaaa", 2,2,2),
  PC("GColorWhite",    "#ffffff", 3,3,3),
};
#define NUM_COLORS 4
#endif  // PBL_COLOR

// ============================================================
// STATE — all persistent layer/window pointers
// ============================================================
static Window    *s_main_window;
static MenuLayer *s_main_menu;

static Window      *s_font_window;
static MenuLayer   *s_font_menu;
static Window      *s_font_detail_window;
static Layer       *s_font_header_bg;    // full-width black fill for header
static TextLayer   *s_font_header_layer; // inset font name label
static ScrollLayer *s_font_scroll_layer;
static Layer       *s_font_canvas_layer;
static int          s_current_font = 0;
static int16_t      s_font_canvas_h = 0;

static Window    *s_color_window;
static MenuLayer *s_color_menu;
static Window    *s_color_detail_window;
static Layer     *s_color_fill_layer;
static TextLayer *s_color_info_layer;
static int        s_current_color = 0;

static Window      *s_platform_window;
static ScrollLayer *s_platform_scroll;
static TextLayer   *s_platform_text_layer;

// ============================================================
// HELPERS
// ============================================================

// Luminance heuristic for choosing black or white overlay text.
// Weights: G channel counts most, B least (matches human perception).
static bool color_is_light(const SamplerColor *c) {
  return (c->r * 2 + c->g * 5 + c->b) >= 9;
}

// ============================================================
// FONT DETAIL WINDOW
//
// Layout:
//   [0 .. FONT_HDR_H)   Black header strip (bg Layer + text TextLayer)
//   [FONT_HDR_H .. h)   ScrollLayer containing canvas Layer
//
// UP/DOWN: scroll within specimen page; at boundaries, advance
//          to the prev/next font and reset scroll to top.
//
// Round screens:
//   FONT_HDR_H     = ROUND_TOP_PAD + 12  (text sits in safe zone)
//   FONT_HDR_TXT_Y = ROUND_TOP_PAD - 4   (-4 accounts for ~2px
//                    Gothic 14 internal leading -> visible 2px shift up)
// ============================================================
#if defined(PBL_ROUND)
#  define FONT_HDR_H     (ROUND_TOP_PAD + 12)
#  define FONT_HDR_TXT_Y (ROUND_TOP_PAD - 4)
#else
#  define FONT_HDR_H     20
#  define FONT_HDR_TXT_Y 3
#endif

// Padding inside the specimen canvas and between sample lines.
#define FONT_PAD      6
#define FONT_LINE_GAP 4
// Extra height added to each draw rect to accommodate descenders.
// Must match between font_canvas_draw and compute_font_canvas_h.
#define FONT_LINE_EXTRA 4

// Row height shared by both font list and color list.
#define LIST_ROW_H 34

static void font_header_bg_draw(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

// Forward declaration needed because font_detail_up/down call it.
static void font_detail_reload(void);

// Draw all five sample strings into the scrollable canvas.
static void font_canvas_draw(Layer *layer, GContext *ctx) {
  const SamplerFont *f       = &s_fonts[s_current_font];
  GFont              specimen = fonts_get_system_font(f->key);
  GRect              bounds   = layer_get_bounds(layer);
  int                w        = bounds.size.w;
  int                pad      = FONT_PAD + ROUND_INSET;
  int                y        = FONT_PAD;

  graphics_context_set_text_color(ctx, GColorBlack);
  for (int i = 0; i < NUM_SAMPLES; i++) {
    GSize sz = graphics_text_layout_get_content_size(
      s_samples[i], specimen, GRect(0, 0, w - 2*pad, 1000),
      GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int lh = sz.h + FONT_LINE_GAP;
    if (lh < 14) lh = 14;  // minimum height for very small fonts
    graphics_draw_text(ctx, s_samples[i], specimen,
      GRect(pad, y, w - 2*pad, lh + FONT_LINE_EXTRA),
      GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    y += lh + FONT_LINE_GAP;
    if (i < NUM_SAMPLES - 1) {
      graphics_context_set_stroke_color(ctx, GColorLightGray);
      graphics_draw_line(ctx, GPoint(pad, y), GPoint(w - pad, y));
      y += 3;
    }
  }
}

// Calculate total canvas height needed for the current font.
// Must mirror font_canvas_draw's layout arithmetic exactly.
static int16_t compute_font_canvas_h(int w) {
  const SamplerFont *f       = &s_fonts[s_current_font];
  GFont              specimen = fonts_get_system_font(f->key);
  int                pad      = FONT_PAD + ROUND_INSET;
  int                y        = FONT_PAD;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    GSize sz = graphics_text_layout_get_content_size(
      s_samples[i], specimen, GRect(0, 0, w - 2*pad, 1000),
      GTextOverflowModeWordWrap, GTextAlignmentLeft);
    int lh = sz.h + FONT_LINE_GAP;
    if (lh < 14) lh = 14;
    y += lh + FONT_LINE_GAP + FONT_LINE_EXTRA;  // match draw call
    if (i < NUM_SAMPLES - 1) y += 3;
  }
  return (int16_t)(y + FONT_PAD);
}

// Reload the header label and reset canvas/scroll for s_current_font.
static void font_detail_reload(void) {
  text_layer_set_text(s_font_header_layer, s_fonts[s_current_font].name);
  Layer *root  = window_get_root_layer(s_font_detail_window);
  GRect  b     = layer_get_bounds(root);
  int    w     = b.size.w;
  int    vis_h = b.size.h - FONT_HDR_H;
  s_font_canvas_h = compute_font_canvas_h(w);
  if (s_font_canvas_h < vis_h) s_font_canvas_h = (int16_t)vis_h;
  layer_set_frame(s_font_canvas_layer, GRect(0, 0, w, s_font_canvas_h));
  scroll_layer_set_content_size(s_font_scroll_layer, GSize(w, s_font_canvas_h));
  scroll_layer_set_content_offset(s_font_scroll_layer, GPointZero, false);
  layer_mark_dirty(s_font_canvas_layer);
}

static void font_detail_up(ClickRecognizerRef r, void *ctx) {
  GPoint off = scroll_layer_get_content_offset(s_font_scroll_layer);
  if (off.y >= 0) {
    // Already at top of page: go to previous font
    if (s_current_font > 0) { s_current_font--; font_detail_reload(); }
  } else {
    scroll_layer_scroll_up_click_handler(r, (void *)s_font_scroll_layer);
  }
}

static void font_detail_down(ClickRecognizerRef r, void *ctx) {
  GPoint off   = scroll_layer_get_content_offset(s_font_scroll_layer);
  Layer *root  = window_get_root_layer(s_font_detail_window);
  int    vis_h = layer_get_bounds(root).size.h - FONT_HDR_H;
  if (-off.y + vis_h >= s_font_canvas_h) {
    // Already at bottom of page: go to next font
    if (s_current_font < NUM_FONTS - 1) { s_current_font++; font_detail_reload(); }
  } else {
    scroll_layer_scroll_down_click_handler(r, (void *)s_font_scroll_layer);
  }
}

static void font_detail_click_config(void *ctx) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP,   150, font_detail_up);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 150, font_detail_down);
}

static void font_detail_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect  b    = layer_get_bounds(root);
  int    w    = b.size.w;
  int    h    = b.size.h;

  // Full-width black bg covers the entire header strip including corners
  // on round screens (where the text TextLayer is narrower than the screen).
  s_font_header_bg = layer_create(GRect(0, 0, w, FONT_HDR_H));
  layer_set_update_proc(s_font_header_bg, font_header_bg_draw);
  layer_add_child(root, s_font_header_bg);

  // Single-line name label, horizontally inset on round screens.
  int txt_x = ROUND_INSET + 2;
  int txt_w = w - 2 * (ROUND_INSET + 2);
  s_font_header_layer = text_layer_create(
    GRect(txt_x, FONT_HDR_TXT_Y, txt_w, FONT_HDR_H - FONT_HDR_TXT_Y));
  text_layer_set_background_color(s_font_header_layer, GColorClear);
  text_layer_set_text_color(s_font_header_layer, GColorWhite);
  text_layer_set_font(s_font_header_layer,
    fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_font_header_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_font_header_layer,
    GTextOverflowModeTrailingEllipsis);
  text_layer_set_text(s_font_header_layer, s_fonts[s_current_font].name);
  layer_add_child(root, text_layer_get_layer(s_font_header_layer));

  // ScrollLayer below the header.
  int vis_h = h - FONT_HDR_H;
  s_font_canvas_h = compute_font_canvas_h(w);
  if (s_font_canvas_h < vis_h) s_font_canvas_h = (int16_t)vis_h;

  s_font_scroll_layer = scroll_layer_create(GRect(0, FONT_HDR_H, w, vis_h));
  scroll_layer_set_content_size(s_font_scroll_layer, GSize(w, s_font_canvas_h));
  layer_add_child(root, scroll_layer_get_layer(s_font_scroll_layer));

  s_font_canvas_layer = layer_create(GRect(0, 0, w, s_font_canvas_h));
  layer_set_update_proc(s_font_canvas_layer, font_canvas_draw);
  scroll_layer_add_child(s_font_scroll_layer, s_font_canvas_layer);

  window_set_click_config_provider(window, font_detail_click_config);
}

static void font_detail_unload(Window *window) {
  layer_destroy(s_font_header_bg);
  text_layer_destroy(s_font_header_layer);
  layer_destroy(s_font_canvas_layer);
  scroll_layer_destroy(s_font_scroll_layer);
  // Sync font list selection back to where we are.
  MenuIndex idx = { 0, (uint16_t)s_current_font };
  menu_layer_set_selected_index(s_font_menu, idx, MenuRowAlignCenter, false);
}

// ============================================================
// FONT LIST WINDOW
// Gothic 14, LIST_ROW_H rows, names truncated with ellipsis.
// ============================================================
static uint16_t font_menu_num_rows(MenuLayer *ml, uint16_t s, void *c) {
  return (uint16_t)NUM_FONTS;
}
static int16_t font_menu_cell_h(MenuLayer *ml, MenuIndex *i, void *c) {
  return LIST_ROW_H;
}
static void font_menu_draw_row(GContext *ctx, const Layer *cell,
                               MenuIndex *idx, void *c) {
  GRect b     = layer_get_bounds(cell);
  bool  hi    = menu_cell_layer_is_highlighted(cell);
  int   inset = ROUND_INSET + 4;
  graphics_context_set_text_color(ctx, hi ? GColorWhite : GColorBlack);
  // Text rect: vertically centred, 18px tall (Gothic 14 + leading).
  graphics_draw_text(ctx, s_fonts[idx->row].name,
    fonts_get_system_font(FONT_KEY_GOTHIC_14),
    GRect(inset, (b.size.h - 18) / 2, b.size.w - 2 * inset, 18),
    GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}
static void font_menu_select(MenuLayer *ml, MenuIndex *idx, void *c) {
  s_current_font = idx->row;
  window_stack_push(s_font_detail_window, true);
}
static void font_window_load(Window *w) {
  Layer *root = window_get_root_layer(w);
  s_font_menu = menu_layer_create(layer_get_bounds(root));
  menu_layer_set_callbacks(s_font_menu, NULL, (MenuLayerCallbacks){
    .get_num_rows   = font_menu_num_rows,
    .get_cell_height = font_menu_cell_h,
    .draw_row       = font_menu_draw_row,
    .select_click   = font_menu_select,
  });
  menu_layer_set_click_config_onto_window(s_font_menu, w);
  layer_add_child(root, menu_layer_get_layer(s_font_menu));
}
static void font_window_unload(Window *w) { menu_layer_destroy(s_font_menu); }

// ============================================================
// COLOR DETAIL WINDOW
// Full-screen fill in the selected color.
// Overlay text (name / hex / RGB) uses black or white depending
// on luminance of the background color.
// UP/DOWN navigates colors directly; back returns list to same row.
// ============================================================
static void color_detail_refresh(void);

static void color_fill_draw(Layer *layer, GContext *ctx) {
  const SamplerColor *c = &s_colors[s_current_color];
#if defined(PBL_COLOR)
  graphics_context_set_fill_color(ctx, c->color);
#else
  // Map 2-bit gray level to the appropriate B&W GColor.
  GColor f = (c->r == 0) ? GColorBlack :
             (c->r == 1) ? GColorDarkGray :
             (c->r == 2) ? GColorLightGray : GColorWhite;
  graphics_context_set_fill_color(ctx, f);
#endif
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void color_detail_refresh(void) {
  const SamplerColor *c = &s_colors[s_current_color];
  // buf needs to hold e.g. "GColorRichBrilliantLavender\n#ffaaff\nR:3  G:2  B:3"
  // max ~51 chars; 80 is comfortably safe.
  static char buf[80];
  snprintf(buf, sizeof(buf), "%s\n%s\nR:%d  G:%d  B:%d",
           c->name, c->hex, (int)c->r, (int)c->g, (int)c->b);
  text_layer_set_text(s_color_info_layer, buf);
  text_layer_set_background_color(s_color_info_layer, GColorClear);
  text_layer_set_text_color(s_color_info_layer,
    color_is_light(c) ? GColorBlack : GColorWhite);
  layer_mark_dirty(s_color_fill_layer);
}

static void color_detail_up(ClickRecognizerRef r, void *c) {
  if (s_current_color > 0) { s_current_color--; color_detail_refresh(); }
}
static void color_detail_down(ClickRecognizerRef r, void *c) {
  if (s_current_color < NUM_COLORS - 1) { s_current_color++; color_detail_refresh(); }
}
static void color_detail_click_config(void *c) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP,   150, color_detail_up);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 150, color_detail_down);
}

static void color_detail_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);

  s_color_fill_layer = layer_create(bounds);
  layer_set_update_proc(s_color_fill_layer, color_fill_draw);
  layer_add_child(root, s_color_fill_layer);

  // Info overlay: horizontally inset (extra on round), vertically centred.
  int   inset   = ROUND_INSET + 8;
  int   info_h  = 72;
  GRect info_rect = GRect(inset, (bounds.size.h - info_h) / 2,
                          bounds.size.w - 2 * inset, info_h);
  s_color_info_layer = text_layer_create(info_rect);
  text_layer_set_font(s_color_info_layer,
    fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_color_info_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_color_info_layer, GTextOverflowModeWordWrap);
  layer_add_child(root, text_layer_get_layer(s_color_info_layer));

  window_set_click_config_provider(window, color_detail_click_config);
  color_detail_refresh();
}

static void color_detail_unload(Window *window) {
  layer_destroy(s_color_fill_layer);
  text_layer_destroy(s_color_info_layer);
  // Sync color list selection back to where we are.
  MenuIndex idx = { 0, (uint16_t)s_current_color };
  menu_layer_set_selected_index(s_color_menu, idx, MenuRowAlignCenter, false);
}

// ============================================================
// COLOR LIST WINDOW
// LIST_ROW_H rows; color swatch left, name right (truncated).
// Swatch is 16px on round (saves space), 20px on rect.
// ============================================================
static uint16_t color_menu_num_rows(MenuLayer *ml, uint16_t s, void *c) {
  return (uint16_t)NUM_COLORS;
}
static int16_t color_menu_cell_h(MenuLayer *ml, MenuIndex *i, void *c) {
  return LIST_ROW_H;
}
static void color_menu_draw_row(GContext *ctx, const Layer *cell,
                                MenuIndex *idx, void *c) {
  const SamplerColor *sc     = &s_colors[idx->row];
  GRect               bounds = layer_get_bounds(cell);
  bool                hi     = menu_cell_layer_is_highlighted(cell);
  int                 inset  = ROUND_INSET;
  int                 sw     = (inset > 0) ? 16 : 20;  // swatch size
  int                 sy     = (bounds.size.h - sw) / 2;
  int                 sx     = inset + 4;
  GRect               swatch = GRect(sx, sy, sw, sw);

#if defined(PBL_COLOR)
  graphics_context_set_fill_color(ctx, sc->color);
  graphics_fill_rect(ctx, swatch, 2, GCornersAll);
  graphics_context_set_stroke_color(ctx, hi ? GColorWhite : GColorDarkGray);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_round_rect(ctx, swatch, 2);
#else
  GColor f = (sc->r == 0) ? GColorBlack :
             (sc->r == 1) ? GColorDarkGray :
             (sc->r == 2) ? GColorLightGray : GColorWhite;
  graphics_context_set_fill_color(ctx, f);
  graphics_fill_rect(ctx, swatch, 0, GCornerNone);
  graphics_context_set_stroke_color(ctx, hi ? GColorWhite : GColorBlack);
  graphics_draw_rect(ctx, swatch);
#endif

  int tx = sx + sw + 5;
  int tw = bounds.size.w - tx - inset - 2;
  graphics_context_set_text_color(ctx, hi ? GColorWhite : GColorBlack);
  // Skip the "GColor" prefix (6 chars) — saves space and reduces redundancy.
  graphics_draw_text(ctx, sc->name + 6,
    fonts_get_system_font(FONT_KEY_GOTHIC_14),
    GRect(tx, sy - 1, tw, bounds.size.h),
    GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}
static void color_menu_select(MenuLayer *ml, MenuIndex *idx, void *c) {
  s_current_color = idx->row;
  window_stack_push(s_color_detail_window, true);
}
static void color_window_load(Window *w) {
  Layer *root  = window_get_root_layer(w);
  s_color_menu = menu_layer_create(layer_get_bounds(root));
  menu_layer_set_callbacks(s_color_menu, NULL, (MenuLayerCallbacks){
    .get_num_rows    = color_menu_num_rows,
    .get_cell_height = color_menu_cell_h,
    .draw_row        = color_menu_draw_row,
    .select_click    = color_menu_select,
  });
  menu_layer_set_click_config_onto_window(s_color_menu, w);
  layer_add_child(root, menu_layer_get_layer(s_color_menu));
}
static void color_window_unload(Window *w) { menu_layer_destroy(s_color_menu); }

// ============================================================
// PLATFORM WINDOW
// Scrollable info screen for the current hardware platform.
//
// Health capabilities (sourced from Rebble developer docs +
// Pebble legacy support articles):
//   Aplite  (Classic / Steel)  — no health API
//   Basalt  (Time / Steel)     — steps, sleep, calories; NO HR
//   Chalk   (Time Round)       — steps, sleep, calories; NO HR
//   Diorite (Pebble 2)         — steps, sleep, calories, HR (not SE)
//   Flint   (Pebble 2 Duo)     — steps, sleep, calories; NO HR
//   Emery   (Time 2)           — steps, sleep, calories, HR
//   Gabbro  (Round 2)          — steps, sleep only; NO HR
// ============================================================
static void platform_window_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);
  static char info[512];

#if defined(PBL_PLATFORM_APLITE)
  snprintf(info, sizeof(info),
    "Aplite\nPebble Classic / Steel\n"
    "\nScreen\n144 x 168 px\n"
    "\nShape\nRectangle\n"
    "\nColor\nBlack & White\n"
    "\nHealth\nNone");
#elif defined(PBL_PLATFORM_BASALT)
  snprintf(info, sizeof(info),
    "Basalt\nPebble Time / Steel\n"
    "\nScreen\n144 x 168 px\n"
    "\nShape\nRectangle\n"
    "\nColor\n64-color\n"
    "\nHealth\nSteps, Sleep,\nCalories (no HR)");
#elif defined(PBL_PLATFORM_CHALK)
  snprintf(info, sizeof(info),
    "Chalk\nPebble Time Round\n"
    "\nScreen\n180 x 180 px\n"
    "\nShape\nRound\n"
    "\nColor\n64-color\n"
    "\nHealth\nSteps, Sleep,\nCalories (no HR)");
#elif defined(PBL_PLATFORM_DIORITE)
  snprintf(info, sizeof(info),
    "Diorite\nPebble 2\n"
    "\nScreen\n144 x 168 px\n"
    "\nShape\nRectangle\n"
    "\nColor\nBlack & White\n"
    "\nHealth\nSteps, Sleep,\nCalories, HR\n(not SE model)");
#elif defined(PBL_PLATFORM_FLINT)
  snprintf(info, sizeof(info),
    "Flint\nPebble 2 Duo\n"
    "\nScreen\n144 x 168 px\n"
    "\nShape\nRectangle\n"
    "\nColor\nBlack & White\n"
    "\nHealth\nSteps, Sleep,\nCalories (no HR)");
#elif defined(PBL_PLATFORM_EMERY)
  snprintf(info, sizeof(info),
    "Emery\nPebble Time 2\n"
    "\nScreen\n200 x 228 px\n"
    "\nShape\nRectangle\n"
    "\nColor\n64-color\n"
    "\nHealth\nSteps, Sleep,\nCalories, HR\n"
    "\n* LECO 60 only on\n  Emery / Gabbro");
#elif defined(PBL_PLATFORM_GABBRO)
  snprintf(info, sizeof(info),
    "Gabbro\nPebble Round 2\n"
    "\nScreen\n260 x 260 px\n"
    "\nShape\nRound\n"
    "\nColor\n64-color\n"
    "\nHealth\nSteps, Sleep\n(no HR)\n"
    "\n* LECO 60 only on\n  Emery / Gabbro");
#else
  snprintf(info, sizeof(info), "Unknown platform");
#endif

  int   hpad      = ROUND_INSET + 6;
  int   top_pad   = ROUND_TOP_PAD + 4;
  int   text_w    = bounds.size.w - 2 * hpad;
  GFont font      = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GSize sz        = graphics_text_layout_get_content_size(
    info, font, GRect(0, 0, text_w, 2000),
    GTextOverflowModeWordWrap, GTextAlignmentLeft);
  int   content_h = top_pad + sz.h + 24;
  if (content_h < bounds.size.h) content_h = bounds.size.h;

  s_platform_scroll = scroll_layer_create(bounds);
  scroll_layer_set_content_size(s_platform_scroll,
    GSize(bounds.size.w, content_h));
  scroll_layer_set_click_config_onto_window(s_platform_scroll, window);
  layer_add_child(root, scroll_layer_get_layer(s_platform_scroll));

  s_platform_text_layer = text_layer_create(
    GRect(hpad, top_pad, text_w, content_h - top_pad));
  text_layer_set_font(s_platform_text_layer, font);
  text_layer_set_overflow_mode(s_platform_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_text(s_platform_text_layer, info);
  scroll_layer_add_child(s_platform_scroll,
    text_layer_get_layer(s_platform_text_layer));
}
static void platform_window_unload(Window *w) {
  text_layer_destroy(s_platform_text_layer);
  scroll_layer_destroy(s_platform_scroll);
}

// ============================================================
// MAIN MENU WINDOW
// Three entries: Fonts / Colors / Platform.
// ============================================================
static uint16_t main_menu_num_rows(MenuLayer *ml, uint16_t s, void *c) {
  return 3;
}
static void main_menu_draw_row(GContext *ctx, const Layer *cell,
                               MenuIndex *idx, void *c) {
  static const char *titles[]   = { "Fonts", "Colors", "Platform" };
  static const char *subtitles[] = {
    "System font specimen",
#if defined(PBL_COLOR)
    "64-color palette",
#else
    "4 B&W levels",
#endif
    "Device info",
  };
  menu_cell_basic_draw(ctx, cell, titles[idx->row], subtitles[idx->row], NULL);
}
static void main_menu_select(MenuLayer *ml, MenuIndex *idx, void *c) {
  switch (idx->row) {
    case 0: window_stack_push(s_font_window,     true); break;
    case 1: window_stack_push(s_color_window,    true); break;
    case 2: window_stack_push(s_platform_window, true); break;
  }
}
static void main_window_load(Window *w) {
  Layer *root = window_get_root_layer(w);
  s_main_menu = menu_layer_create(layer_get_bounds(root));
  menu_layer_set_callbacks(s_main_menu, NULL, (MenuLayerCallbacks){
    .get_num_rows = main_menu_num_rows,
    .draw_row     = main_menu_draw_row,
    .select_click = main_menu_select,
  });
  menu_layer_set_click_config_onto_window(s_main_menu, w);
  layer_add_child(root, menu_layer_get_layer(s_main_menu));
}
static void main_window_unload(Window *w) { menu_layer_destroy(s_main_menu); }

// ============================================================
// APP LIFECYCLE
// All windows are pre-created at init and destroyed at deinit.
// Windows are never created/destroyed mid-session, so layer
// state (s_current_font, s_current_color) persists correctly.
// ============================================================
static Window *make_window(WindowHandler load, WindowHandler unload) {
  Window *w = window_create();
  window_set_window_handlers(w, (WindowHandlers){ .load = load, .unload = unload });
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

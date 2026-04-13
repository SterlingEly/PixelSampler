# CONTEXT_PIXELSAMPLER.md

Implementation reference for Pixel Sampler v1.0. Use this file to resume
development in future sessions without needing to re-read the full codebase.

---

## Overview

Pixel Sampler is a Pebble developer reference app (watchapp, not watchface).
Three sections accessible from a main MenuLayer:

1. **Fonts** — list of all system fonts → specimen detail page
2. **Colors** — 64-color palette list → full-screen color preview
3. **Platform** — scrollable hardware info for the current device

All windows are pre-created in `init()` and persist for the app's lifetime.
This keeps navigation snappy and preserves `s_current_font` / `s_current_color`
state across visits.

---

## Repository

- **Repo:** `SterlingEly/PixelSampler`, branch `main`
- **UUID:** `a7f3c821-4d9e-4b15-8e62-1f0d5a3c7b94`
- **Version:** 1.0.0
- **Store:** submitted to Rebble/Repebble store (Tools & Utilities)
- **Contest:** Spring 2026 Pebble App Contest (submitted April 2026)

---

## Platform geometry constants

```c
// chalk  (Pebble Time Round, 180×180)
ROUND_INSET   = 30   // horizontal safe zone (narrowest chord near poles)
ROUND_TOP_PAD = 26   // vertical offset to clear circle top

// gabbro (Pebble Round 2, 260×260)
ROUND_INSET   = 40
ROUND_TOP_PAD = 36

// rect platforms
ROUND_INSET   = 0
ROUND_TOP_PAD = 0
```

---

## Font detail window layout

```
[0 .. FONT_HDR_H)     Black header strip
  s_font_header_bg    full-width Layer (drawn black)
  s_font_header_layer TextLayer, inset by ROUND_INSET+2 on round,
                      y = FONT_HDR_TXT_Y = ROUND_TOP_PAD-4 on round, 3 on rect
                      single line, GTextOverflowModeTrailingEllipsis
[FONT_HDR_H .. h)     ScrollLayer (s_font_scroll_layer)
  s_font_canvas_layer custom Layer, height = compute_font_canvas_h()
```

**Header height:**
- Round: `ROUND_TOP_PAD + 12`
- Rect:  `20`

**FONT_HDR_TXT_Y:** `ROUND_TOP_PAD - 4` on round (the -4 accounts for ~2px
Gothic 14 Bold internal leading, producing a visually centred result).

**Canvas height:** `compute_font_canvas_h()` mirrors `font_canvas_draw()` exactly.
Both use `FONT_LINE_EXTRA = 4` added to each line's draw rect height to
accommodate descenders. **If you change one, change the other.**

**UP/DOWN navigation logic:**
- `off.y >= 0` → at top of scroll → go to previous font
- `-off.y + vis_h >= canvas_h` → at bottom → go to next font
- Otherwise → normal scroll step

---

## Color detail window

- Full-screen `s_color_fill_layer` drawn in the selected color
- `s_color_info_layer` centred overlay (TextLayer, GColorClear bg)
- Contrast-adaptive text: `color_is_light()` uses `r*2 + g*5 + b >= 9`
- UP/DOWN navigates directly without returning to list
- `color_detail_unload` calls `menu_layer_set_selected_index` to sync list

---

## Color list

- 64 colors (or 4 on B&W) in spectrum order
- `LIST_ROW_H = 34`, swatch 20px rect / 16px round
- Name displayed with `GColor` prefix stripped (saves 6 chars of width)
- `GTextOverflowModeTrailingEllipsis` for long names
- `PC()` macro: packs a `GColor` from 2-bit r/g/b: `argb = 0b11rrggbb`

---

## Platform health capabilities (verified from Rebble docs)

| Platform | Model              | HR  | Steps/Sleep/Cal |
|----------|--------------------|-----|-----------------|
| aplite   | Classic / Steel    | No  | No              |
| basalt   | Time / Time Steel  | No  | Yes             |
| chalk    | Time Round         | No  | Yes             |
| diorite  | Pebble 2           | Yes (not SE) | Yes  |
| flint    | Pebble 2 Duo       | No  | Yes             |
| emery    | Pebble Time 2      | Yes | Yes             |
| gabbro   | Pebble Round 2     | No  | Steps/Sleep only|

Note: Basalt/Chalk/Time Steel never had built-in HR (smartstrap-only).
Gabbro: no HR, steps and sleep only (no calories confirmed).

---

## Key implementation notes

- **`s_font_canvas_h` type:** `int16_t` — ScrollLayer content size uses
  `GSize` which takes `int16_t`. Cast carefully.
- **`(void *)s_font_scroll_layer` cast** in scroll click handlers is correct
  per Pebble SDK scroll_layer_scroll_up/down_click_handler signature.
- **`LIST_ROW_H = 34`** shared by both font list and color list.
- **`static char info[512]`** in platform_window_load — longest platform
  string is ~180 chars; 512 is comfortable.
- **`static char buf[80]`** in color_detail_refresh — longest color string
  is ~51 chars; 80 is comfortable.
- **LECO 60 guard:** `#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)`
  — these font keys don't exist in older SDK headers.

---

## CloudPebble notes

- Import from GitHub: `SterlingEly/PixelSampler`, branch `main`
- Menu icon must be added manually via CloudPebble UI (Resources → + → Image
  → mark as menu icon) — GitHub import does not carry icons
- No font resources needed (all system fonts)
- No JS / config page
- `resources.media` array is empty — do not add it back, CloudPebble will
  error on unsupported resource types

---

## Files

| File | Purpose |
|------|---------|
| `src/main.c` | Full app source |
| `package.json` | App manifest, UUID, platform targets |
| `README.md` | Project overview + platform table |
| `CHANGELOG.md` | Version history |
| `STORE_LISTING.md` | Store copy (description, keywords, changelog) |
| `CONTEXT_PIXELSAMPLER.md` | This file — implementation reference |
| `assets/banner.png` | Store banner (470×320) |
| `assets/icon_25x25.png` | Menu icon (25×25 B&W PNG) |

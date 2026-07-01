# Pixel Sampler — Project Context

> **For AI collaborators and future contributors:** Read this file before touching any code.
> It is the authoritative implementation reference for this project.

---

## Purpose

Pixel Sampler is a Pebble **watchapp** (not a watchface) that serves as an
on-wrist developer reference tool. It lets Pebble developers browse all system
fonts, the full 64-color palette, and hardware platform capabilities without
leaving their wrist or switching windows.

Three sections, accessible from a root MenuLayer:
1. **Fonts** — browse all system fonts by name → tap for a scrollable specimen page
2. **Colors** — full 64-color palette list → tap for a full-screen color preview
3. **Platform** — scrollable hardware info for the device currently running the app

---

## Current Status

- **Version:** 1.0.0 (shipped)
- **Store:** Live on Rebble / Repebble store — Tools & Utilities
- **Contest:** Entered in Spring 2026 Pebble App Contest (submitted April 2026)
- **Branch:** `main` — stable, no open PRs
- **Next milestone:** v1.1 (no scope defined yet — see TODO)

---

## Human / AI Role Split

| Responsibility | Owner |
|---|---|
| Product concept & design direction | Sterling Ely |
| Platform / device testing | Sterling Ely |
| Store submission & asset production | Sterling Ely |
| C implementation (`src/main.c`) | AI (Claude, Anthropic) |
| Documentation & context files | AI (Claude, Anthropic) |
| Code review & bug verification | Sterling Ely + AI jointly |

---

## Repository Structure

```
PixelSampler/
  src/
    main.c              — full app source (single file)
  assets/
    banner.png          — store banner (470×320)
    icon_25x25.png      — CloudPebble menu icon (25×25 B&W PNG)
  package.json          — app manifest: UUID, version, platform targets
  wscript               — Pebble SDK build script (do not edit)
  README.md             — project overview, platform table, attribution
  PROJECT_CONTEXT.md    — this file: implementation reference for collaborators
  CHANGELOG.md          — version history (technical)
  STORE_LISTING.md      — store description copy
```

---

## Build / Deployment Rules

### CloudPebble
- Import from GitHub: `SterlingEly/PixelSampler`, branch `main`
- **Menu icon must be added manually** via CloudPebble UI:
  Resources → + → Image → mark as menu icon
  GitHub import does not carry the icon over automatically
- No font resources needed — all fonts are system fonts
- No JS / config page / clay config
- **Do not add a `resources.media` array** to `package.json` —
  CloudPebble errors on unsupported published resource types

### SDK notes
- `LECO_60_*` font keys only exist in emery/gabbro SDK headers; always guard with:
  `#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)`
- `s_font_canvas_h` is typed `int16_t` because `GSize` (used by ScrollLayer) takes `int16_t`
- The `(void *)s_font_scroll_layer` cast in UP/DOWN click handlers is correct per
  Pebble SDK `scroll_layer_scroll_up/down_click_handler` signature

---

## Architecture

### Window stack
All 6 windows are **pre-created in `init()` and never destroyed mid-session**.
This keeps navigation instant and preserves `s_current_font` / `s_current_color`
state across window pushes/pops.

```
main menu window
  ├── font list window (MenuLayer)
  │     └── font detail window (ScrollLayer + custom canvas Layer)
  ├── color list window (MenuLayer)
  │     └── color detail window (fill Layer + info TextLayer)
  └── platform window (ScrollLayer + TextLayer)
```

### Font detail window layout
```
[0 .. FONT_HDR_H)       Black header strip
  s_font_header_bg      full-width Layer, custom draw proc fills black
  s_font_header_layer   TextLayer, inset ROUND_INSET+2 on round,
                        y = FONT_HDR_TXT_Y, single line, TrailingEllipsis
[FONT_HDR_H .. h)       ScrollLayer (s_font_scroll_layer)
  s_font_canvas_layer   custom Layer, height = compute_font_canvas_h()
```

**Header height:**
- Round: `ROUND_TOP_PAD + 12`
- Rect: `20`

**`FONT_HDR_TXT_Y`:** `ROUND_TOP_PAD - 4` on round. The `-4` compensates for
~2px Gothic 14 Bold internal leading, producing a visually centred result.

**Canvas height contract:** `compute_font_canvas_h()` must mirror
`font_canvas_draw()` arithmetic exactly — both use `FONT_LINE_EXTRA = 4`
added to each draw rect height for descender clearance.
**If you change one, change the other.**

**UP/DOWN scroll boundary logic:**
- `off.y >= 0` → at top → go to previous font
- `-off.y + vis_h >= canvas_h` → at bottom → go to next font
- Otherwise → normal scroll step (delegate to scroll layer handler)

### Color detail window
- `s_color_fill_layer`: full-screen layer, custom draw proc fills selected color
- `s_color_info_layer`: centred TextLayer, `GColorClear` background
- Contrast-adaptive text: `color_is_light()` → `r*2 + g*5 + b >= 9`
- `color_detail_unload` calls `menu_layer_set_selected_index` to sync list position on back

### Color list
- 64 colors (4 on B&W platforms) in spectrum order
- `GColor` prefix stripped from names in list rows (saves 6 chars of width)
- `GTextOverflowModeTrailingEllipsis` handles long names (e.g. "ElectricUltramar...")
- `PC()` macro packs a `GColor` from 2-bit r/g/b: `argb = 0b11rrggbb`

---

## Critical Constants

```c
// Shared by font list and color list
#define LIST_ROW_H  34

// Font detail header
#define FONT_HDR_H      (ROUND_TOP_PAD + 12)  // round
#define FONT_HDR_H      20                     // rect
#define FONT_HDR_TXT_Y  (ROUND_TOP_PAD - 4)   // round
#define FONT_HDR_TXT_Y  3                      // rect
#define FONT_LINE_EXTRA 4   // must match in both draw and compute functions

// Round-screen geometry
// chalk (Pebble Time Round, 180×180)
#define ROUND_INSET    30   // narrowest safe horizontal zone near poles
#define ROUND_TOP_PAD  26   // vertical clear for circle top

// gabbro (Pebble Round 2, 260×260)
#define ROUND_INSET    40
#define ROUND_TOP_PAD  36

// rect platforms
#define ROUND_INSET    0
#define ROUND_TOP_PAD  0

// Buffer sizes (verified safe)
static char info[512];  // platform_window_load — longest string ~180 chars
static char buf[80];    // color_detail_refresh — longest string ~51 chars
```

---

## Platform Hardware Reference

Verified against Rebble developer docs and legacy Pebble support articles.

| Platform | Model              | Screen  | Shape | Color    | Touch | HR  | Health          |
|----------|--------------------|---------|-------|----------|-------|-----|------------------|
| aplite   | Classic / Steel    | 144×168 | Rect  | B&W      | No    | No  | None            |
| basalt   | Time / Time Steel  | 144×168 | Rect  | 64-color | No    | No  | Steps/Sleep/Cal |
| chalk    | Time Round         | 180×180 | Round | 64-color | No    | No  | Steps/Sleep/Cal |
| diorite  | Pebble 2           | 144×168 | Rect  | B&W      | No    | Yes*| Steps/Sleep/Cal |
| flint    | Pebble 2 Duo       | 144×168 | Rect  | B&W      | No    | No  | Steps/Sleep/Cal |
| emery    | Pebble Time 2      | 200×228 | Rect  | 64-color | Yes   | Yes | Steps/Sleep/Cal |
| gabbro   | Pebble Round 2     | 260×260 | Round | 64-color | No    | No  | Steps/Sleep only|

\* Diorite HR: not available on SE model.  
Basalt/chalk/Time Steel: HR was smartstrap-only, never built-in.  
Gabbro: steps and sleep only — calories not confirmed.

---

## Known Bugs / Known Traps

- None known at v1.0 ship.
- **Trap:** `compute_font_canvas_h()` and `font_canvas_draw()` must stay in
  sync — both must use `FONT_LINE_EXTRA`. If canvas height is calculated
  differently from how content is drawn, the last sample row may be clipped.
- **Trap:** Adding a `resources.media` block to `package.json` will cause
  CloudPebble to error on build with "unsupported published resource type".
- **Trap:** `LECO_60_*` font keys cause compile errors on non-emery/gabbro
  platforms if not guarded by the platform `#if`.

---

## Current TODO

*v1.1 scope not yet defined. Candidate ideas (not committed):*

- Show each font name **in its own font** in the list (currently all Gothic 14)
- Add raw `0b11rrggbb` ARGB byte to color detail screen
- Color search / filter by name
- Hex code lookup (enter hex, see matching GColor)

---

## Verification Plan

Before any future release, test on at minimum:
- [ ] aplite (rect, B&W, no health)
- [ ] basalt or emery (rect, color)
- [ ] chalk or gabbro (round, color) — verify ROUND_INSET clipping and header text position
- [ ] diorite or flint (rect, B&W, health)

Key things to verify on round:
- Platform screen: first line of text not clipped at circle top
- Font detail: header name fits / truncates cleanly, not clipped at sides
- Color list: names truncate (not wrap) on long entries like "ElectricUltramarine"

---

## Source of Truth / External Links

- **Rebble hardware table:** https://developer.rebble.io/guides/tools-and-resources/hardware-information/
- **Rebble developer docs:** https://developer.rebble.io
- **Store listing:** https://apps.repebble.com (search "Pixel Sampler" by Sterling Ely)
- **Repo:** https://github.com/SterlingEly/PixelSampler
- **UUID:** `a7f3c821-4d9e-4b15-8e62-1f0d5a3c7b94`

---

## Last Updated

April 2026 — v1.0 ship. Updated by AI collaborator (Claude, Anthropic).

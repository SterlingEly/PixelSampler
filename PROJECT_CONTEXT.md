# Pixel Sampler — Project Context

> **For AI collaborators and future contributors:**
> Read this file before making any code changes.
> It is the authoritative implementation reference for this project.

---

## Purpose

Pixel Sampler is a Pebble **watchapp** (not a watchface) that serves as an
on-wrist developer reference tool. It lets Pebble app and watchface developers
browse all system fonts, the full 64-color palette, and hardware platform
capabilities without leaving the wrist or switching windows.

Three sections, accessible from a root `MenuLayer`:

1. **Fonts** — browse all system fonts by name → select for a scrollable specimen page
2. **Colors** — full 64-color palette list → select for a full-screen color preview
3. **Platform** — scrollable hardware info for the device currently running the app

### Design philosophy

Pixel Sampler is a utility, not a showcase. It should be fast, dense, and
transparent. Every screen answers a developer question: *what does this font
look like? what is this color called? what can this platform do?* Decisions
favour information density and navigational speed over visual polish.

---

## Current Status *(Live)*

- **Version:** 1.0.0 — shipped April 2026
- **Store:** Live on Rebble / Repebble store — Tools & Utilities
- **Contest:** Entered in Spring 2026 Pebble App Contest
- **Branch:** `main` — stable, no open PRs
- **Next milestone:** v1.1 — scope not yet defined (see [Current TODO](#current-todo))

---

## Human / AI Role Split *(Stable)*

| Responsibility | Owner |
|---|---|
| Product concept & design direction | Sterling Ely |
| Platform / device testing | Sterling Ely |
| Store submission & asset production | Sterling Ely |
| C implementation (`src/main.c`) | AI collaborator (Claude, Anthropic) |
| Documentation & context files | AI collaborator (Claude, Anthropic) |
| Code review & bug verification | Sterling Ely + AI jointly |

---

## Repository Structure *(Stable)*

```
PixelSampler/
  src/
    main.c                — full app source (single C file)
  assets/
    banner.png            — store banner (470×320)
    icon_25x25.png        — CloudPebble menu icon (25×25 B&W PNG)
  package.json            — app manifest: UUID, version, platform targets
  wscript                 — Pebble SDK build script (do not edit)
  README.md               — project overview, features, design philosophy
  PROJECT_CONTEXT.md      — this file: implementation reference
  CHANGELOG.md            — version history (technical)
  STORE_LISTING.md        — store description copy
  CONTEXT_PIXELSAMPLER.md — deprecated stub, redirects here
```

---

## Build & Deployment Rules *(Stable)*

### Importing into CloudPebble
- Import from GitHub: `SterlingEly/PixelSampler`, branch `main`
- **Menu icon must be added manually** after import:
  CloudPebble UI → Resources → + → Image → mark as menu icon.
  GitHub import does not carry the icon.
- No custom font resources — all fonts are system fonts.
- No JS, no config page, no Clay.
- **Do not add a `resources.media` array to `package.json`.**
  CloudPebble throws "unsupported published resource type" and the build fails.

### SDK constraints
- `LECO_60_*` font keys only exist in emery/gabbro SDK headers.
  Always guard with:
  ```c
  #if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
  ```
- `s_font_canvas_h` is `int16_t` because `GSize` (used by `ScrollLayer`) takes `int16_t`.
- The `(void *)s_font_scroll_layer` cast in click handlers is correct per
  the Pebble SDK `scroll_layer_scroll_up/down_click_handler` signature.

---

## Architecture *(Stable)*

### Window lifecycle

All 6 windows are **pre-created in `init()` and never destroyed mid-session.**
This keeps navigation instant and preserves `s_current_font` / `s_current_color`
across window pushes and pops without re-reading state.

```
main menu window
  ├── font list window        (MenuLayer)
  │     └── font detail window  (ScrollLayer + custom canvas Layer)
  ├── color list window       (MenuLayer)
  │     └── color detail window (fill Layer + info TextLayer)
  └── platform window         (ScrollLayer + TextLayer)
```

### Font detail window

```
[0 .. FONT_HDR_H)         Black header strip
  s_font_header_bg        full-width Layer, draw proc fills black
  s_font_header_layer     TextLayer, inset (ROUND_INSET+2) on round,
                          y = FONT_HDR_TXT_Y, single line, TrailingEllipsis
[FONT_HDR_H .. h)         ScrollLayer (s_font_scroll_layer)
  s_font_canvas_layer     custom Layer, height = compute_font_canvas_h()
```

**Header height:**
- Round: `ROUND_TOP_PAD + 12`
- Rect: `20`

**`FONT_HDR_TXT_Y`:** `ROUND_TOP_PAD - 4` on round.
The `-4` compensates for ~2px Gothic 14 Bold internal leading,
producing a visually centred result in the header strip.

**Canvas height contract:**
`compute_font_canvas_h()` must mirror `font_canvas_draw()` arithmetic exactly.
Both use `FONT_LINE_EXTRA = 4` added to each line's draw rect for descender clearance.
**If you change one, change the other.**

**UP/DOWN boundary logic:**
- `off.y >= 0` → at scroll top → advance to previous font
- `-off.y + vis_h >= canvas_h` → at scroll bottom → advance to next font
- Otherwise → delegate to `scroll_layer_scroll_up/down_click_handler`

### Color detail window

- `s_color_fill_layer` — full-screen `Layer`, draw proc fills in the selected color
- `s_color_info_layer` — centred `TextLayer`, `GColorClear` background
- Contrast-adaptive text: `color_is_light()` returns true when `r*2 + g*5 + b >= 9`
- `color_detail_unload` calls `menu_layer_set_selected_index` to sync list position on back

### Color list

- 64 colors in spectrum order (4 on B&W platforms)
- `GColor` prefix stripped from names in list rows (saves ~6px of width)
- `GTextOverflowModeTrailingEllipsis` on long names (e.g. "RichBrilliantLavender")
- `PC()` macro packs a `GColor` from 2-bit r/g/b channels: `argb = 0b11rrggbb`

---

## Critical Constants *(Stable)*

```c
// Shared row height — font list and color list must both use this
#define LIST_ROW_H  34

// Font detail header — values differ by platform shape
//   Round:  FONT_HDR_H = ROUND_TOP_PAD + 12
//           FONT_HDR_TXT_Y = ROUND_TOP_PAD - 4
//   Rect:   FONT_HDR_H = 20
//           FONT_HDR_TXT_Y = 3
#define FONT_LINE_EXTRA  4   // MUST match between draw and compute functions

// Round-screen geometry
// chalk  (Pebble Time Round,  180×180px)
#define ROUND_INSET    30    // safe horizontal zone (narrowest chord near poles)
#define ROUND_TOP_PAD  26    // vertical offset to clear circle top
// gabbro (Pebble Round 2,    260×260px)
#define ROUND_INSET    40
#define ROUND_TOP_PAD  36
// rect platforms
#define ROUND_INSET     0
#define ROUND_TOP_PAD   0

// Static buffer sizes (verified adequate)
// info[512] in platform_window_load  — longest string ~180 chars
// buf[80]   in color_detail_refresh  — longest string ~51 chars
```

---

## Platform Hardware Reference *(Stable)*

Verified against Rebble developer docs and multiple CES 2026 reviews.

| Platform | Model              | Screen  | Shape | Color    | Touch | HR    | Health           |
|----------|--------------------|---------|-------|----------|-------|-------|------------------|
| aplite   | Classic / Steel    | 144×168 | Rect  | B&W      | No    | No    | None             |
| basalt   | Time / Time Steel  | 144×168 | Rect  | 64-color | No    | No    | Steps/Sleep/Cal  |
| chalk    | Time Round         | 180×180 | Round | 64-color | No    | No    | Steps/Sleep/Cal  |
| diorite  | Pebble 2           | 144×168 | Rect  | B&W      | No    | Yes†  | Steps/Sleep/Cal  |
| flint    | Pebble 2 Duo       | 144×168 | Rect  | B&W      | No    | No    | Steps/Sleep/Cal  |
| emery    | Pebble Time 2      | 200×228 | Rect  | 64-color | Yes   | Yes   | Steps/Sleep/Cal  |
| gabbro   | Pebble Round 2     | 260×260 | Round | 64-color | Yes‡  | No    | Steps/Sleep•     |

† Diorite HR: not available on the SE model.  
Basalt / chalk / Time Steel: HR was smartstrap-only, never built-in.  
‡ Gabbro touch: confirmed present but secondary. PebbleOS is buttons-first on all
platforms; the touchscreen is a supplementary input layer available to developers
and can be disabled in settings. Same model as emery. Not a replacement for
button-based navigation.  
• Gabbro calories: not listed in official Core Devices specs or press materials.
Whether the SDK exposes calories for gabbro is unconfirmed pending device availability
(shipping July 2026). Steps and sleep are confirmed.

---

## Known Bugs / Known Traps *(Live)*

**No known bugs at v1.0.**

**Traps:**

- `compute_font_canvas_h()` and `font_canvas_draw()` must stay in sync.
  Both must add `FONT_LINE_EXTRA` to each line height.
  A mismatch causes the last specimen row to clip silently.

- Do not add `resources.media` to `package.json`.
  CloudPebble will fail the build with "unsupported published resource type".

- `LECO_60_*` font keys do not exist in older SDK headers.
  Omitting the `#if PBL_PLATFORM_EMERY || PBL_PLATFORM_GABBRO` guard
  causes compile errors on all other platforms.

- CloudPebble does not import the menu icon from GitHub.
  It must be added manually after every re-import.

---

## Current TODO *(Live)*

*v1.1 scope is undefined. Candidate ideas — none committed:*

- Render each font name **in its own font** in the font list
  (currently all rows use Gothic 14 for density)
- Show raw `0b11rrggbb` ARGB byte on color detail screen
- Color name search / filter
- Hex code lookup — enter a hex value, find the closest GColor
- Revisit gabbro health display once device ships (July 2026) — confirm calories

---

## Verification Plan *(Stable)*

Before any future release, test on at minimum:

- [ ] aplite — rect, B&W, no health API
- [ ] basalt or emery — rect, color
- [ ] chalk or gabbro — round, color
- [ ] diorite or flint — rect, B&W, health

**Round-specific checks:**
- Platform screen: first text line not clipped at circle top
- Font detail header: name fits or truncates cleanly, not clipped at sides
- Color list: long names truncate with ellipsis (not wrap)
  — test "ElectricUltramarine" and "RichBrilliantLavender"

---

## Related Projects *(Stable)*

**TallBoy**  
https://github.com/SterlingEly/TallBoy  
Digit-column watchface; shares Pebble SDK patterns and round-screen geometry solutions.

**Radium 2**  
https://github.com/SterlingEly/Radium2  
Radial bar graph watchface; the 64-color palette in Pixel Sampler directly supports Radium 2 color selection.

**Bar Graph 2**  
https://github.com/SterlingEly/BarGraph2  
Modernized bar graph watchface; shares platform and health capability constants.

**Monogram**  
https://github.com/SterlingEly/Monogram  
Artistic digit watchface for gabbro (Round 2); shares gabbro geometry constants.

---

## Source of Truth / External Links *(Stable)*

- **Rebble hardware table:** https://developer.rebble.io/guides/tools-and-resources/hardware-information/
- **Rebble developer docs:** https://developer.rebble.io
- **Rebble app store:** https://apps.rebble.io
- **Repebble app store:** https://apps.repebble.com
- **Repo:** https://github.com/SterlingEly/PixelSampler
- **App UUID:** `a7f3c821-4d9e-4b15-8e62-1f0d5a3c7b94`

---

## Last Updated

April 2026 — v1.0 ship + documentation standardization pass.
Gabbro hardware facts refined: touch confirmed secondary, calories unconfirmed pending device.
Updated by AI collaborator (Claude, Anthropic) with direction from Sterling Ely.

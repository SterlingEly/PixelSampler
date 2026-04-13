# Pixel Sampler

A developer reference app for Pebble. Browse all system fonts, the full
64-color palette, and hardware platform details — all on your wrist.

## Features

**Fonts** — Scrollable list of all system fonts by name. Select any font
for a full-screen specimen page showing five sample strings (time, digits,
alpha, prose, symbols). UP/DOWN scrolls the specimen; at the boundary it
advances to the next/previous font automatically.

**Colors** — The full 64-color Pebble palette in spectrum order, with a
color swatch and name per row. Select any color for a full-screen preview
filled with that color, overlaid with the GColor constant name, hex code,
and 2-bit RGB channel values. Contrast-adaptive text (black or white).
UP/DOWN navigates colors without leaving the detail screen.

**Platform** — Scrollable info screen showing the current device's platform
codename, watch model, screen dimensions, shape, color depth, and health
sensor capabilities.

## Platform support

All 7 Pebble platforms supported:

| Platform | Model              | Screen     | Shape | Color    | Health / HR          |
|----------|--------------------|------------|-------|----------|----------------------|
| aplite   | Classic / Steel    | 144×168    | Rect  | B&W      | None                 |
| basalt   | Time / Time Steel  | 144×168    | Rect  | 64-color | Steps/Sleep/Cal      |
| chalk    | Time Round         | 180×180    | Round | 64-color | Steps/Sleep/Cal      |
| diorite  | Pebble 2           | 144×168    | Rect  | B&W      | Steps/Sleep/Cal + HR |
| flint    | Pebble 2 Duo       | 144×168    | Rect  | B&W      | Steps/Sleep/Cal      |
| emery    | Pebble Time 2      | 200×228    | Rect  | 64-color | Steps/Sleep/Cal + HR |
| gabbro   | Pebble Round 2     | 260×260    | Round | 64-color | Steps/Sleep only     |

LECO 60 fonts are only available on emery and gabbro.

## Notes

- "Bitham Med Numbers" and "LECO" fonts only render digits and punctuation;
  alphabetic sample strings will show placeholder glyphs — this is correct
  and informative behavior.
- B&W platforms (aplite, diorite, flint) show 4 gray levels in Colors.

## Attribution

Design & concept: Sterling Ely  
Implementation: Sterling Ely + Claude (Anthropic), 2026

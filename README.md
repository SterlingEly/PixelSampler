# Pixel Sampler

A developer reference app for Pebble. Browse all system fonts, the full
64-color palette, and hardware platform details — all on your wrist.

> **Contributors and AI collaborators:**
> Read [`PROJECT_CONTEXT.md`](PROJECT_CONTEXT.md) before making any code changes.
> It contains the full implementation reference, architecture, build rules, and known traps.

---

## What it does

**Fonts** — Scrollable list of all system fonts by name. Select any font
for a full-screen specimen page with five sample strings covering time, digits,
alpha, prose, and symbols. UP/DOWN scrolls the specimen; at the boundary it
advances to the next or previous font automatically.

**Colors** — The full 64-color Pebble palette in spectrum order. Each row
shows a color swatch and name. Select any color for a full-screen preview
filled with that color, overlaid with the GColor constant name, hex code, and
2-bit RGB channel values. Text color adapts automatically for contrast.
UP/DOWN navigates colors without returning to the list.

**Platform** — Scrollable info screen showing the current device's platform
codename, watch model, screen dimensions, shape, color depth, and health
sensor capabilities.

---

## Design philosophy

Pixel Sampler is a utility, not a showcase. Every screen answers a developer
question: *what does this font look like? what is this color called? what can
this platform do?* Decisions favour information density and navigational speed.
The watch is the reference; the phone stays in your pocket.

---

## Platform support

All 7 Pebble platforms supported:

| Platform | Model              | Screen  | Shape | Color    | Touch | Health / HR          |
|----------|--------------------|---------|-------|----------|-------|----------------------|
| aplite   | Classic / Steel    | 144×168 | Rect  | B&W      | No    | None                 |
| basalt   | Time / Time Steel  | 144×168 | Rect  | 64-color | No    | Steps/Sleep/Cal      |
| chalk    | Time Round         | 180×180 | Round | 64-color | No    | Steps/Sleep/Cal      |
| diorite  | Pebble 2           | 144×168 | Rect  | B&W      | No    | Steps/Sleep/Cal + HR |
| flint    | Pebble 2 Duo       | 144×168 | Rect  | B&W      | No    | Steps/Sleep/Cal      |
| emery    | Pebble Time 2      | 200×228 | Rect  | 64-color | Yes   | Steps/Sleep/Cal + HR |
| gabbro   | Pebble Round 2     | 260×260 | Round | 64-color | No    | Steps/Sleep only     |

LECO 60 fonts are only available on emery and gabbro.

---

## Notes

- "Bitham Med Numbers" and "LECO" fonts only render digits and punctuation.
  Alphabetic sample strings show placeholder glyphs — this is correct behavior
  and is intentionally informative.
- B&W platforms (aplite, diorite, flint) show 4 gray levels in the Colors section.

---

## Attribution

Design & concept: Sterling Ely  
Implementation: Sterling Ely + Claude (Anthropic), 2026

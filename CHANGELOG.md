# Changelog — Pixel Sampler

## v1.0 — April 2026

Initial release.

- **Fonts**: all 25 system fonts (23 on older platforms; LECO 60 pair only on
  emery/gabbro). Scrollable specimen view per font with 5 sample strings.
  UP/DOWN advances font at scroll boundary.
- **Colors**: full 64-color palette in spectrum order (4 grays on B&W platforms).
  Full-screen color detail with GColor name, hex, and 2-bit RGB values.
  Contrast-adaptive overlay text. UP/DOWN navigates without leaving detail.
- **Platform**: scrollable hardware info for all 7 platforms — codename, model,
  screen size, shape, color depth, health sensor capabilities.
- Round-screen support (chalk/gabbro): safe-zone insets, top padding, full-width
  black header bg, single-line truncated font titles.
- All 7 Pebble platforms: aplite, basalt, chalk, diorite, emery, flint, gabbro.

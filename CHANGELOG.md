# Changelog — Pixel Sampler

## v1.0 — April 2026

Initial release. The Pebble developer's on-wrist reference — browse all system
fonts, the full 64-color palette, and hardware platform info across all 7 platforms.

**Fonts**
- 25 system fonts (23 on aplite/basalt/chalk/diorite/flint; LECO 60 pair
  exclusive to emery/gabbro)
- Scrollable specimen view per font with 5 sample strings: time, digits,
  alpha, prose, symbols
- UP/DOWN scrolls within specimen; at boundary advances to next/previous font
- Font list uses Gothic 14 at 34px rows, matching color list density

**Colors**
- Full 64-color Pebble palette in spectrum order (4 grays on B&W platforms)
- Full-screen color detail: GColor name, hex code, 2-bit RGB channel values
- Contrast-adaptive overlay text (black or white based on luminance heuristic)
- UP/DOWN navigates colors without returning to list; back restores list position

**Platform**
- Scrollable hardware info: codename, model, screen size, shape, color depth,
  health and HR capabilities
- Health data verified against Rebble developer docs and legacy Pebble support
  articles: chalk/basalt/flint have no HR; diorite and emery have HR;
  gabbro has steps/sleep only

**Round-screen support (chalk/gabbro)**
- Platform-specific ROUND_INSET (30px chalk, 40px gabbro) matches narrowest
  chord near display poles — prevents content clipping as you scroll
- ROUND_TOP_PAD offsets first text row below the circular boundary
- Font detail header: full-width black bg layer + inset single-line label
  with trailing ellipsis truncation

**Platforms:** aplite · basalt · chalk · diorite · emery · flint · gabbro

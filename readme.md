# Notoco Keymap for Zima

This is a custom keymap and software for [splitkb.com Zima](https://splitkb.com/collections/keyboard-kits/products/zima).

## Installation

In [qmk](https://github.com/qmk/qmk_firmware) repo, clone this repository under `keyboards/splitkb/zima/keymaps` then run `qmk compile -kb splitkb/zima -km nocoto_zima` (or equivalent `make` command).

## Features

- Dedicated layer for layer selections.
- Dynamic encoder mouse scroll direction toggle.
- OLED display showing keys.
- Smaller than default keymap while adding more mappings to existing one.
- Can be operated in no-oled, audio-only mode (once you get used to it).
- Cute little animation and icons.

## Default usage guide

OLED shows you the keys mappings.

In any layer, press encoder to go to layer selection.

In layer selection, press bottom right key to toggle reverse mouse scroll.

## Tips for Mill-Max Hot Swap users

Support the PCB with ~4mm silicone stand-offs to prevent PCB sinks during key presses.

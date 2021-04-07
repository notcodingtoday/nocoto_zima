/* Copyright 2021 Nocoto Day (notcoding.today)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include <stdio.h>

enum nocoto_layers {
  LAYER_NUM = 0,
  LAYER_SET,
  LAYER_RGB,
  LAYER_FNC,
  LAYER_MDA,

  LAYER_SLT
};

enum nocoto_keycodes {
  KC_RMW = SAFE_RANGE,
  KC_MDA_SCRL,
  SPOTIFY_VOL_UP,
  SPOTIFY_VOL_DOWN,
  SPOTIFY_FF,
  SPOTIFY_RW,
  SPOTIFY_SHOW,
};

#define NO_MUSIC_MODE

static bool reverse_mw = true;
static bool mda_scroll_mode = false;
static float song_mw_toggle[][2] = SONG(QWERTY_SOUND);
static float song_slt_enter[][2] = SONG(NUM_LOCK_OFF_SOUND);
static float song_slt_exit[][2] = SONG(CAPS_LOCK_ON_SOUND);

#ifdef OLED_DRIVER_ENABLE
static bool is_asleep = false;
static uint32_t screen_timer = 0;
static uint32_t anim_timer = 0;

static const char icon_playpause = 0x0E;
static const char icon_arrow_right = 0x1A;
static const char icon_arrow_left = 0x1B;
static const char icon_triangle_right = 0x10;
static const char icon_triangle_left = 0x11;

void suspend_power_down_user(void) {
    is_asleep = true;
}

void suspend_wakeup_init_user(void) {
    is_asleep = false;
}

void oled_task_user(void) {
  if (is_asleep || timer_elapsed32(screen_timer) > 30000) {
    oled_off();
    return;
  }

  oled_on();
  oled_scroll_off();

  char anim_k = '|';
  if (timer_elapsed32(anim_timer) % 4000 < 1000) {
    anim_k = '/';
  } else if (timer_elapsed32(anim_timer) % 4000 < 2000) {
    anim_k = '-';
  } else if (timer_elapsed32(anim_timer) % 4000 < 3000) {
    anim_k = '\\';
  }

  char top_k[4] = {0x20, 0x20 ,0x20, 0x00};
  char bot_k[4] = {0x20, 0x20 ,0x20, 0x00};
  if (reverse_mw) {
    top_k[0] = 'U';
    top_k[1] = icon_arrow_left;

    bot_k[1] = icon_arrow_right;
    bot_k[2] = 'D';
  } else {
    top_k[1] = icon_arrow_right;
    top_k[2] = 'U';

    bot_k[0] = 'D';
    bot_k[1] = icon_arrow_left;
  }

  if (get_highest_layer(layer_state) == LAYER_NUM) {
    oled_write((char[]){' ', anim_k, ' ', 0x00}, false);
    oled_write_ln_P(PSTR(   "  0  Ent  |  Num"), false);
    oled_write_ln_P(PSTR(" 7   8   9   |  "), false);
    oled_write_P(PSTR(   " 4   5   6   |  "), false);
    oled_write_ln(top_k, false);
    oled_write_P(PSTR(   " 1   2   3   |  "), false);
    oled_write_ln(bot_k, false);

  } else if (get_highest_layer(layer_state) == LAYER_SET) {
    oled_write((char[]){' ', anim_k, ' ', 0x00}, false);
    oled_write_ln_P(PSTR(   "  .   NML |  Set"), false);
    is_audio_on() ? oled_write_P(PSTR("AOf "), false) : oled_write_P(PSTR("AOn "), false);
    is_clicky_on() ? oled_write_P(PSTR("TOf "), false) : oled_write_P(PSTR("TOn "), false);
    oled_write_ln_P(PSTR(        " Tr  |  "), false);
    oled_write_P(PSTR(   " .   .   T+  |  "), false);
    oled_write_ln(top_k, false);
    oled_write_P(PSTR(   " .   .   T-  |  "), false);
    oled_write_ln(bot_k, false);

  } else if (get_highest_layer(layer_state) == LAYER_RGB) {
    oled_write((char[]){' ', anim_k, ' ', 0x00}, false);
    oled_write_ln_P(PSTR(   "  .   .   |  RGB"), false);
    oled_write_ln_P(PSTR("H+  S+  V+   |  "), false);
    oled_write_P(PSTR(   "H-  S-  V-   |  "), false);
    oled_write_ln(top_k, false);
    oled_write_P(PSTR(   "M- Tog  M+   |  "), false);
    oled_write_ln(bot_k, false);

  } else if (get_highest_layer(layer_state) == LAYER_FNC) {
    oled_write((char[]){' ', anim_k, ' ', 0x00}, false);
    oled_write_ln_P(PSTR(   " F10 F11  |  Fnc"), false);
    oled_write_ln_P(PSTR("F7  F8  F9   |  "), false);
    oled_write_P(PSTR(   "F4  F5  F6   |  "), false);
    oled_write_ln(top_k, false);
    oled_write_P(PSTR(   "F1  F2  F3   |  "), false);
    oled_write_ln(bot_k, false);

  } else if (get_highest_layer(layer_state) == LAYER_MDA) {
    oled_write((char[]){' ', anim_k, ' ', 0x00}, false);
    if (mda_scroll_mode) {
      oled_write_P(PSTR(" Vol "), false);
    } else {
      oled_write_P(PSTR(" Srl "), false);
    }
    oled_write_ln_P(PSTR(        " S   |  Mda"), false);
    oled_write_ln_P(PSTR(" <<  +  >>   |  "), false);

    oled_write_P(PSTR(" "), false);
    oled_write((char[]){icon_triangle_left, 0x00}, false);
    oled_write_P(PSTR("   "), false);
    oled_write((char[]){icon_playpause, 0x00}, false);
    oled_write_P(PSTR("   "), false);
    oled_write((char[]){icon_triangle_right, 0x00}, false);
    oled_write_P(PSTR("   |  "), false);
    oled_write_ln(top_k, false);

    oled_write_P(PSTR(   " .   -   .   |  "), false);
    oled_write_ln(bot_k, false);

  } else if (get_highest_layer(layer_state) == LAYER_SLT) {
    oled_write_P(PSTR(   " +  Set RGB  |   "), false);
    oled_write_ln((char[]){anim_k, 0x00}, false);
    oled_write_ln_P(PSTR("Fnc Num  .   |  "), false);
    oled_write_P(PSTR(   " .  Mda  .   |  "), false);
    oled_write_ln(top_k, false);
    oled_write_P(PSTR(   " .   .  "), false);
    if (reverse_mw) {
      oled_write((char[]){'D', icon_arrow_right, 'U', 0x00}, false);
    } else {
      oled_write((char[]){'U', icon_arrow_right, 'D', 0x00}, false);
    }
    oled_write_P(PSTR(              "  |  "), false);
    oled_write_ln(bot_k, false);
  }
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
#    ifdef OLED_DRIVER_ENABLE
  screen_timer = timer_read32();
#    endif

  if (keycode == KC_RMW) {
    if (record->event.pressed) {
      reverse_mw = !reverse_mw;
      PLAY_SONG(song_mw_toggle);
    }
    return false;
  } else if (keycode == KC_MDA_SCRL) {
    if (record->event.pressed) {
      mda_scroll_mode = !mda_scroll_mode;
      PLAY_SONG(song_mw_toggle);
    }
    return false;
  } else if (keycode >= SPOTIFY_VOL_UP && keycode <= SPOTIFY_SHOW) {
    if (record->event.pressed) {
      register_code(KC_LALT);
      register_code(KC_LGUI);
      if (keycode == SPOTIFY_VOL_UP) {
        tap_code(KC_K);
      } else if (keycode == SPOTIFY_VOL_DOWN) {
        tap_code(KC_J);
      } else if (keycode == SPOTIFY_FF) {
        tap_code(KC_L);
      } else if (keycode == SPOTIFY_RW) {
        tap_code(KC_H);
      } else if (keycode == SPOTIFY_SHOW) {
        tap_code(KC_O);
      }
    } else {
      unregister_code(KC_LGUI);
      unregister_code(KC_LALT);
    }
    return false;
  }
  return true;
}

layer_state_t layer_state_set_user(layer_state_t new_state) {
  if (get_highest_layer(new_state) == LAYER_SLT) {
    PLAY_SONG(song_slt_enter);
  } else {
    PLAY_SONG(song_slt_exit);
  }
  return new_state;
}

#ifdef ENCODER_ENABLE
void encoder_update_user(uint8_t index, bool clockwise) {
#    ifdef OLED_DRIVER_ENABLE
  screen_timer = timer_read32();
#    endif

  if (clockwise) {
    if (!mda_scroll_mode && get_highest_layer(layer_state) == LAYER_MDA) {
      tap_code(KC_VOLU);
    } else if (reverse_mw) {
      tap_code(KC_WH_D);
    } else {
      tap_code(KC_WH_U);
    }
  } else {
    if (!mda_scroll_mode && get_highest_layer(layer_state) == LAYER_MDA) {
      tap_code(KC_VOLD);
    } else if (reverse_mw) {
      tap_code(KC_WH_U);
    } else {
      tap_code(KC_WH_D);
    }
  }
}
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Base
    [LAYER_NUM] = LAYOUT_ortho_4x3(
        TG(LAYER_SLT),   KC_P0, KC_PENT,
        KC_P7,           KC_P8, KC_P9,
        KC_P4,           KC_P5, KC_P6,
        KC_P2,           KC_P2, KC_P3
    ),

    // Settings
    [LAYER_SET] = LAYOUT_ortho_4x3(
        TG(LAYER_SLT), XXXXXXX, KC_NUMLOCK,
        AU_TOG,        CK_TOGG, CK_RST,
        XXXXXXX,       XXXXXXX, CK_UP,
        XXXXXXX,       XXXXXXX, CK_DOWN
    ),

    // RGB
    [LAYER_RGB] = LAYOUT_ortho_4x3(
        TG(LAYER_SLT), XXXXXXX, XXXXXXX,
        RGB_HUI,       RGB_SAI, RGB_VAI,
        RGB_HUD,       RGB_SAD, RGB_VAD,
        RGB_RMOD,      RGB_TOG, RGB_MOD
    ),

    // FN
    [LAYER_FNC] = LAYOUT_ortho_4x3(
        TG(LAYER_SLT), KC_F10,  KC_F11,
        KC_F7,         KC_F8,   KC_F9,
        KC_F4,         KC_F5,   KC_F6,
        KC_F1,         KC_F2,   KC_F3
    ),

    // FN
    [LAYER_MDA] = LAYOUT_ortho_4x3(
        TG(LAYER_SLT), KC_MDA_SCRL,      SPOTIFY_SHOW,
        SPOTIFY_RW,    SPOTIFY_VOL_UP,   SPOTIFY_FF,
        KC_MPRV,       KC_MPLY,          KC_MNXT,
        XXXXXXX,       SPOTIFY_VOL_DOWN, XXXXXXX
    ),

    // Layers
    [LAYER_SLT] = LAYOUT_ortho_4x3(
        _______,       TO(LAYER_SET), TO(LAYER_RGB),
        TO(LAYER_FNC), TO(LAYER_NUM), XXXXXXX,
        XXXXXXX,       TO(LAYER_MDA), XXXXXXX,
        XXXXXXX,       XXXXXXX,       KC_RMW
    ),
};

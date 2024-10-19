// Copyright 2024 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include QMK_KEYBOARD_H
#include <math.h>
#include "bmp.h"
#include "bmp_custom_keycodes.h"
#include "quantum.h"


#include "oneshot.h"
#include "swapper.h"
#include "secret.h"

#define HOME G(KC_LEFT)
#define END G(KC_RGHT)
#define FWD G(KC_RBRC)
#define BACK G(KC_LBRC)
#define TAB_L G(S(KC_LBRC))
#define TAB_R G(S(KC_RBRC))
#define SPACE_L A(G(KC_LEFT))
#define SPACE_R A(G(KC_RGHT))
#define LA_SYM MO(SYM)
#define LA_NAV MO(NAV)
#define KC_VOLU KC_KB_VOLUME_UP
#define KC_VOLD KC_KB_VOLUME_DOWN
#define KC_MUTE KC_KB_MUTE
#define DRG_SCRL DRAG_SCROLL


uint8_t set_scrolling = 0;
uint8_t set_encoder = 0;

enum custom_keycodes {
    DRAG_SCROLL = BMP_SAFE_RANGE,
    TRACKBALL_AS_ENCODER1,
    TRACKBALL_AS_ENCODER2,
    SCRL_TO,

    // Custom oneshot mod implementation with no timers.
    OS_SHFT,
    OS_CTRL,
    OS_ALT,
    OS_CMD,

    SW_WIN,  // Switch to next window         (cmd-tab)
    SW_LANG, // Switch to next input language (ctl-spc)
    SW_TAB,  // Switch to next tab            (ctl-tab)

    MACRO1,
    MY_HOME,
    MY_END,
};

enum layers
{
    DEF,
    SYM,
    NAV,
    NUM,
    MOU,
};


// Disable BMP dynamic matrix size
#undef MATRIX_ROWS
#define MATRIX_ROWS MATRIX_ROWS_DEFAULT
#undef MATRIX_COLS
#define MATRIX_COLS MATRIX_COLS_DEFAULT

// clang-format off
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [DEF] = LAYOUT_mini(
    KC_Q        , KC_W     , KC_F     , KC_P         , KC_G         ,                                 KC_J     , KC_L     , KC_U     , KC_Y     , KC_QUOT,
    KC_A        , KC_R     , KC_S     , KC_T         , KC_D         ,                                 KC_H     , KC_N     , KC_E     , KC_I     , KC_O   ,
    KC_Z        , KC_X     , KC_C     , KC_V         , KC_B         ,                                 KC_K     , KC_M     , KC_COMM  , KC_DOT   , KC_SCLN,
    _______     , _______  , _______  , KC_LSFT      , LA_NAV      , KC_TAB,               KC_SPC,   LA_SYM    , XXXXXXX  , XXXXXXX  , XXXXXXX  , SCRL_TO
  ),

  [SYM] = LAYOUT_mini(
    KC_ESC      , KC_LBRC  , KC_LCBR  , KC_LPRN      , KC_TILD      ,                                KC_CIRC   , KC_RPRN  , KC_RCBR  , KC_RBRC  , KC_GRV ,
    KC_MINS     , KC_ASTR  , KC_EQL   , KC_UNDS      , KC_DLR       ,                                KC_HASH   , OS_SHFT  , OS_CMD   , OS_ALT   , OS_CTRL,
    KC_PLUS     , KC_PIPE  , KC_AT    , KC_SLSH      , KC_PERC      ,                                XXXXXXX   , KC_BSLS  , KC_AMPR  , KC_QUES  , KC_EXLM,
    MACRO1      , _______  , _______  , _______      , _______      , _______   ,         _______ ,  _______   , XXXXXXX  , XXXXXXX  , XXXXXXX  , _______
  ),

  [NAV] = LAYOUT_mini(
    KC_TAB      , SW_WIN   , TAB_L    , TAB_R        , KC_VOLU      ,                                KC_CAPS   , MY_HOME  , KC_UP    , MY_END   , KC_DEL,
    OS_CTRL     , OS_ALT   , OS_CMD   , OS_SHFT      , KC_VOLD      ,                                XXXXXXX   , KC_LEFT  , KC_DOWN  , KC_RGHT  , KC_BSPC,
    SPACE_L     , SPACE_R  , BACK     , FWD          , KC_MUTE      ,                                SW_TAB    , KC_PGDN  , KC_PGUP  , SW_LANG  , KC_ENT,
    _______     , _______  , _______  , _______      , _______      ,  _______ ,           _______ ,   _______ , XXXXXXX  , XXXXXXX  , XXXXXXX  , _______
  ),

  [NUM] = LAYOUT_mini(
    KC_1        , KC_2     , KC_3     , KC_4         , KC_5         ,                                 KC_6     , KC_7     , KC_8     , KC_9     , KC_0,
    OS_CTRL     , OS_ALT   , OS_CMD   , OS_SHFT      , KC_F11       ,                                 KC_F12   , OS_SHFT  , OS_CMD   , OS_ALT   , OS_CTRL,
    KC_F1       , KC_F2    , KC_F3    , KC_F4        , KC_F5        ,                                 KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10,
    _______     , _______  , _______  , _______      , _______      , _______  ,          _______ ,  _______   , _______  , _______  , _______  , _______
  ),
  [MOU] = LAYOUT_mini(
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            KC_BTN5  , DRG_SCRL , _______   , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            KC_BTN4  , KC_BTN1  , KC_BTN3  , KC_BTN2  , OSL(MOU)    ,
    _______  , _______  , _______  , _______  , _______  , _______  ,      _______,   _______  , XXXXXXX  , XXXXXXX  , XXXXXXX  , _______
  ),
};
// clang-format on

const uint16_t encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, },
    [1] = { {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, },
    [2] = { {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, },
    [3] = { {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, },
    [4] = { {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, {KC_RIGHT, KC_LEFT}, {KC_UP, KC_DOWN}, },
};

bool is_mouse_record_user(uint16_t keycode, keyrecord_t* record) {
    switch(keycode) {
        case KC_NO:
        case KC_LEFT_CTRL...KC_RIGHT_GUI:
        case DRAG_SCROLL:
        case TRACKBALL_AS_ENCODER1 ... TRACKBALL_AS_ENCODER2:
            return true;
        default:
            return false;
    }
    return false;
}

#define SNAP_BUF_LEN 5
#define SNAP_TIME_OUT 500

typedef enum {
    SNAP_NONE,
    SNAP_X,
    SNAP_Y,
} snap_state_t;

snap_state_t update_snap(snap_state_t current_snap, uint8_t snap_history) {
    uint8_t x_cnt = 0;
    for (int i = 0; i < SNAP_BUF_LEN; i++) {
        if (snap_history & (1 << i)) x_cnt++;
    }

    if (current_snap == SNAP_X && x_cnt <= SNAP_BUF_LEN / 2 && (snap_history & 0x03) == 0) {
        return SNAP_Y;
    } else if (current_snap == SNAP_Y && x_cnt > SNAP_BUF_LEN / 2 && (snap_history & 0x03) == 3) {
        return SNAP_X;
    } else if (current_snap == SNAP_NONE) {
        return (x_cnt <= SNAP_BUF_LEN / 2) ? SNAP_Y : SNAP_X;
    }

    return current_snap;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    static bool         snap_start;
    static int8_t       snap_buf_cnt;
    static uint8_t      snap_history;
    static snap_state_t snap_state;
    static float        x_kh, y_kh;
    static uint32_t     last_scroll_time;

    if (!is_pointing_device_active()) {
        return mouse_report;
    }

    uint32_t highest_layer_bits = (1 << get_highest_layer(layer_state));
    uint8_t  set_encoder_layer  = (eeconfig_kb.pseudo_encoder.layer & highest_layer_bits) ? (1 << 0) : 0;
    uint8_t  set_encoder_flag   = set_encoder_layer | set_encoder;

    float divide = 1;
    if (set_scrolling > 0 && eeconfig_kb.scroll.divide > 0) {
        divide *= eeconfig_kb.scroll.divide;
    }

    if (set_encoder_flag > 0 && eeconfig_kb.pseudo_encoder.divide > 0) {
        divide *= eeconfig_kb.pseudo_encoder.divide;
    }

    if ((eeconfig_kb.cursor.fine_layer & highest_layer_bits) && eeconfig_kb.cursor.fine_div > 0) {
        divide *= eeconfig_kb.cursor.fine_div;
    }

    if ((eeconfig_kb.cursor.rough_layer & highest_layer_bits) && eeconfig_kb.cursor.rough_mul > 0) {
        divide /= eeconfig_kb.cursor.rough_mul;
    }

    float rad = eeconfig_kb.cursor.rotate / 180.0f * M_PI;
    float c = cosf(rad);
    float s = sinf(rad);
    float x   = (c * mouse_report.x + -s * mouse_report.y) / divide + x_kh;
    float y   = (s * mouse_report.x + c * mouse_report.y) / divide + y_kh;

    mouse_report.x = (int)x;
    mouse_report.y = (int)y;
    x_kh           = x - mouse_report.x;
    y_kh           = y - mouse_report.y;

    set_scrolling &= ~(1 << 1);
    set_scrolling |= (eeconfig_kb.scroll.layer & highest_layer_bits) ? 2 : 0;

    if (set_scrolling > 0 || set_encoder_flag > 0) {
        bool snap_option = ((set_scrolling > 0) && (eeconfig_kb.scroll.options.snap)) //
                           || ((set_encoder_flag > 0)                                 //
                               && ((eeconfig_kb.pseudo_encoder.snap_layer & highest_layer_bits) != 0));
        bool invert_option = ((set_scrolling > 0) && (eeconfig_kb.scroll.options.invert)); //

        if (mouse_report.x == 0 && mouse_report.y == 0) {
            mouse_report.h = 0;
            mouse_report.v = 0;
            return mouse_report;
        }
        else if (snap_option) {
            if (!snap_start || timer_elapsed32(last_scroll_time) > SNAP_TIME_OUT) {
                snap_start   = true;
                snap_buf_cnt = 0;
                snap_history = 0;
                snap_state   = SNAP_NONE;
            }
            last_scroll_time = timer_read32();

            snap_history <<= 1;
            if (snap_state == SNAP_X) {
                snap_history |= abs(mouse_report.y) > abs(mouse_report.x) * 4 ? 0 : 1;
            } else {
                snap_history |= abs(mouse_report.x) > abs(mouse_report.y) * 4 ? 1 : 0;
            }
            snap_buf_cnt += snap_buf_cnt < SNAP_BUF_LEN ? 1 : 0;
            snap_history &= ((1 << SNAP_BUF_LEN) - 1);
            if (snap_buf_cnt == SNAP_BUF_LEN) {
                snap_state = update_snap(snap_state, snap_history);
                if (snap_state == SNAP_X) {
                    mouse_report.h = (mouse_report.x > 127) ? 127 : ((mouse_report.x < -127) ? -127 : mouse_report.x);
                } else {
                    mouse_report.v = (mouse_report.y > 127) ? -127 : ((mouse_report.y < -127) ? 127 : -mouse_report.y);
                }
            }
        } else {
            mouse_report.h = (mouse_report.x > 127) ? 127 : ((mouse_report.x < -127) ? -127 : mouse_report.x);
            mouse_report.v = (mouse_report.y > 127) ? -127 : ((mouse_report.y < -127) ? 127 : -mouse_report.y);
        }

        if (invert_option) {
            mouse_report.h *= -1;
            mouse_report.v *= -1;
        }

        mouse_report.x = 0;
        mouse_report.y = 0;

        if (set_encoder_flag > 0) {
            if (mouse_report.h != 0) {
                action_exec(mouse_report.h > 0 ? MAKE_ENCODER_CW_EVENT(biton(set_encoder_flag) * 2 + 0, true) //
                                               : MAKE_ENCODER_CCW_EVENT(biton(set_encoder_flag) * 2 + 0, true));
                action_exec(mouse_report.h > 0 ? MAKE_ENCODER_CW_EVENT(biton(set_encoder_flag) * 2 + 0, false) //
                                               : MAKE_ENCODER_CCW_EVENT(biton(set_encoder_flag) * 2 + 0, false));
            }
            if (mouse_report.v != 0) {
                action_exec(mouse_report.v > 0 ? MAKE_ENCODER_CW_EVENT(biton(set_encoder_flag) * 2 + 1, true) //
                                               : MAKE_ENCODER_CCW_EVENT(biton(set_encoder_flag) * 2 + 1, true));
                action_exec(mouse_report.v > 0 ? MAKE_ENCODER_CW_EVENT(biton(set_encoder_flag) * 2 + 1, false) //
                                               : MAKE_ENCODER_CCW_EVENT(biton(set_encoder_flag) * 2 + 1, false));
            }
        }

        if (set_scrolling == 0) {
            mouse_report.h = 0;
            mouse_report.v = 0;
        }

    } else {
        snap_start = false;
    }

    return mouse_report;
}

// perform as ignore mod tap interrupt
extern bool get_hold_on_other_key_press_vial(uint16_t keycode, keyrecord_t *record);
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            return get_hold_on_other_key_press_vial(keycode, record);
        default:
            return true;
    }
}

/*

==================================================================================
user level customizations
==================================================================================

*/

layer_state_t layer_state_set_user(layer_state_t state)
{
  state = update_tri_layer_state(state, SYM, NAV, NUM);
  if (get_highest_layer(state) == DEF)
  {
    ; // default layer
  }
  return state;
}


void switch_key_by_os(uint16_t macos_key, uint16_t other_key, keyrecord_t *record)
{
  os_variant_t os = detected_host_os();
  if (record->event.pressed)
  {
    register_code16(os == OS_MACOS ? macos_key : other_key);
  }
  else
  {
    unregister_code16(os == OS_MACOS ? macos_key : other_key);
  }
}
bool is_oneshot_cancel_key(uint16_t keycode)
{
  switch (keycode)
  {
  case LA_SYM:
  case LA_NAV:
    return true;
  default:
    return false;
  }
}

bool is_oneshot_ignored_key(uint16_t keycode)
{
  switch (keycode)
  {
  case LA_SYM:
  case LA_NAV:
  case KC_LSFT:
  case OS_SHFT:
  case OS_CTRL:
  case OS_ALT:
  case OS_CMD:
    return true;
  default:
    return false;
  }
}

bool sw_win_active = false;
bool sw_lang_active = false;
bool sw_tab_active = false;

oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_SCROLL:
            set_scrolling &= ~(1 << 0);
            set_scrolling |= record->event.pressed ? 1 : 0;
            break;

        case TRACKBALL_AS_ENCODER1 ... TRACKBALL_AS_ENCODER2:
            set_encoder &= ~(1 << (keycode - TRACKBALL_AS_ENCODER1));
            set_encoder |= record->event.pressed ? (1 << (keycode - TRACKBALL_AS_ENCODER1)) : 0;
            break;
    }

    if(keycode == SCRL_TO){
        set_scrolling ^= record->event.pressed;
        return false;
    }

    if (keycode == MY_HOME){
        switch_key_by_os(HOME, KC_HOME, record);
        return false;
    }
    if (keycode == MY_END) {
        switch_key_by_os(END, KC_END, record);
        return false;
    }
    if (keycode == MACRO1 && record->event.pressed)
    {
        SEND_STRING(MACRO1_STR);
        return false;
    }

    update_swapper(
        &sw_win_active, KC_LGUI, KC_TAB, SW_WIN,
        keycode, record);
    update_swapper(
        &sw_lang_active, KC_LCTL, KC_SPC, SW_LANG,
        keycode, record);
    update_swapper(
        &sw_tab_active, KC_LCTL, KC_TAB, SW_TAB,
        keycode, record);

    update_oneshot(
        &os_shft_state, KC_LSFT, OS_SHFT,
        keycode, record);
    update_oneshot(
        &os_ctrl_state, KC_LCTL, OS_CTRL,
        keycode, record);
    update_oneshot(
        &os_alt_state, KC_LALT, OS_ALT,
        keycode, record);
    update_oneshot(
        &os_cmd_state, KC_LCMD, OS_CMD,
        keycode, record);

    return true;
}



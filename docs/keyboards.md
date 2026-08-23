# キーボード

*キーボード*は仮想キーボードの仕様を定義するものです。

## 構成

- `keyboards/`
    - `<keyboard>/`
        - `keymaps/`
        - `keymap.h`
        - (`config.h`)
        - (`post_config.h`)
        - (`CMakeLists.txt`)

各*キーボード*は`keyboards/`内の`<keyboard>/`に配置されます。`<keyboard>`はキーボード名を表す説明用の名前で、実際のキーボード名には`/`を含めることができます。

### keymap.h

*キーボード*は`<keyboard>/`直下に`keymap.h`を必要とします。この`keymap.h`は、*キーマップ*によって`QMK_KEYBOARD_H`マクロを介してインクルードされるため、キーマップを定義するのに必要なヘッダをインクルードしたり便利なマクロを定義したりするのに適しています。例えば、既存の実装から`keymap.c`を流用する場合、ここで`LAYOUT`マクロを定義すると良いでしょう。

以下はその一例です。

```c:keymap.h
#pragma once

// 既定の共通定義
#include <quark/keymap.h>

// キーコードを定義するための便利なマクロ群
#include <quantum_keycodes.h>

// 言語固有のキーを定義するマクロ群
#include <keymap_japanese.h>

// キーの1次元配列を2次元に再配置するマクロ
#define LAYOUT( \
  ESCAPE, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, PRINT_SCREEN, SCROLL_LOCK, PAUSE, \
  HANKAKU_ZENKAKU, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, MINUS, CIRCUMFLEX, YEN, BACKSPACE, INSERT, HOME, PAGE_UP, NUM_LOCK, KP_SLASH, KP_ASTERISK, KP_MINUS, \
  TAB, Q, W, E, R, T, Y, U, I, O, P, AT, LEFT_BRACKET, DELETE, END, PAGE_DOWN, KP_7, KP_8, KP_9, \
  CAPS_LOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, COLON, RIGHT_BRACKET, ENTER, KP_4, KP_5, KP_6, KP_PLUS, \
  LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, DOT, SLASH, BACKSLASH, RIGHT_SHIFT, UP, KP_1, KP_2, KP_3, \
  LEFT_CTRL, LEFT_GUI, LEFT_ALT, MUHENKAN, SPACE, HENKAN, KATAKANA_HIRAGANA, RIGHT_ALT, RIGHT_GUI, APPLICATION, RIGHT_CTRL, LEFT, DOWN, RIGHT, KP_0, KP_DOT, KP_ENTER \
) { \
  {ESCAPE, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, PRINT_SCREEN, SCROLL_LOCK, PAUSE}, \
  {HANKAKU_ZENKAKU, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, MINUS, CIRCUMFLEX, YEN, BACKSPACE, INSERT, HOME, PAGE_UP, NUM_LOCK, KP_SLASH, KP_ASTERISK, KP_MINUS}, \
  {TAB, Q, W, E, R, T, Y, U, I, O, P, AT, LEFT_BRACKET, DELETE, END, PAGE_DOWN, KP_7, KP_8, KP_9}, \
  {CAPS_LOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, COLON, RIGHT_BRACKET, ENTER, KP_4, KP_5, KP_6, KP_PLUS}, \
  {LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, DOT, SLASH, BACKSLASH, RIGHT_SHIFT, UP, KP_1, KP_2, KP_3}, \
  {LEFT_CTRL, LEFT_GUI, LEFT_ALT, MUHENKAN, SPACE, HENKAN, KATAKANA_HIRAGANA, RIGHT_ALT, RIGHT_GUI, APPLICATION, RIGHT_CTRL, LEFT, DOWN, RIGHT, KP_0, KP_DOT, KP_ENTER}, \
}
```

### config.h と post_config.h

`<keyboard>/`直下に`config.h`や`post_config.h`が存在する場合、それらはすべてのソースファイルで暗黙的にインクルードされます。`config.h`は`post_config.h`や*キーマップ*の`config.h`の**前**にインクルードされるので、`MATRIX_COLS`や`MATRIX_ROWS`などの*キーマップ*ごとに変化しない機能オプションを設定するのに適しています。一方の`post_config.h`は*キーマップ*の`config.h`の**後**にインクルードされるので、設定されたオプションに応じて関連するマクロを定義するのに適しています。

以下はその一例です。

```c:config.h
#pragma once

// 仮想キーボードの仕様を設定する
#define MATRIX_COLS 22
#define MATRIX_ROWS 6

// その他、不要な機能を無効化することもできる
// #define NO_DEBUG
// #define NO_PRINT
// #define NO_ACTION_LAYER
// #define NO_ACTION_TAPPING
// #define NO_ACTION_ONESHOT
```

```c:post_config.h
#pragma once

// config.hの結果に応じて、追加のマクロを定義することもできる
#ifndef NO_DEBUG
#define ANOTHER_DEBUG_FEATURES_ON
#endif
```

### CMakeLists.txt

`<keyboard>/`直下に`CMakeLists.txt`が存在する場合、そのディレクトリはビルド対象に含まれるようになります。そこでは、cmakeの変数としてQMKのビルドオプションを設定したり、`keyboard`というライブラリを定義したりすることができます。この`keyboard`ライブラリは、そのままQMKにリンクされるので、*キーマップ*ごとに変化しない機能を実装するのに適しています。

以下はその一例です。

```cmake:CMakeLists.txt
# ビルドオプションを設定できる
set(NKRO_ENABLE yes PARENT_SCOPE)

# keyboardライブラリを定義できる
add_library(keyboard STATIC
    keyboard.cpp
)
target_link_libraries(keyboard PRIVATE
    config  # 共通設定をまとめたライブラリ
)
```

### keymaps

`keymaps/`内には各*キーマップ*が配置されます。詳しくは[keymaps](keymaps.md)を参照してください。

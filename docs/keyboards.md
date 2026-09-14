# キーボード

*キーボード*は仮想キーボードの仕様を定義するものです。

## 構成

- `keyboards/`
    - `<keyboard>/`
        - `keymaps/`
        - (`<keyboard>.h`)
        - (`<keyboard>.c`)
        - (`config.h`)
        - (`post_config.h`)
        - (`rules.cmake`)
        - (`CMakeLists.txt`)

各*キーボード*は`keyboards/`内の`<keyboard>/`に配置されます。`<keyboard>`はキーボード名を表す説明用の名前で、実際のキーボード名には`/`を含めることができます。

### \<keyboard\>.h

`<keyboard>/`直下に`<keyboard>.h`が存在する場合、それは*キーマップ*によって`QMK_KEYBOARD_H`マクロを介してインクルードされます。そのため、これはキーマップを定義するのに必要なヘッダをインクルードしたり便利なマクロを定義したりするのに適しています。

以下はその一例です。

```c:keboard.h
#pragma once

// キーコードを定義するための便利なマクロ群
#include <quantum_keycodes.h>

// 言語固有のキーを定義するマクロ群
#include <keymap_japanese.h>

// キーの1次元配列を2次元に再配置するマクロ
#define LAYOUT(...) ...
```

### \<keyboard\>.c

`<keyboard>/`直下に`<keyboard>.c`が存在する場合、それはQMKの一部としてコンパイルされます。

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

### rules.cmake と post_rules.cmake

`<keyboard>/`直下に`rules.cmake`や`post_rules.cmake`が存在する場合、それらはビルドスクリプトで暗黙的にインクルードされます。`rules.cmake`は`post_rules.cmake`や*キーマップ*の`rules.cmake`の**前**にインクルードされるので、*キーマップ*ごとに変化しないビルドオプションを設定するのに適しています。一方の`post_rules.cmake`は*キーマップ*の`rules.cmake`の**後**にインクルードされるので、設定されたオプションに応じて関連するオプションを定義するのに適しています。

以下はその一例です。

```cmake:rules.cmake
# ビルドオプションを定義できる
set(NKRO_ENABLE yes)
```

```cmake:post_rules.cmake
# 定義されたビルドオプションに応じて、追加のオプションを定義することもできる
if(NKRO_ENABLE)
  ...
endif()
```

### CMakeLists.txt

`<keyboard>/`直下に`CMakeLists.txt`が存在する場合、そのディレクトリはビルド対象に含まれるようになります。そこで定義される`keyboard`という名前のライブラリはキーボードエンジンにリンクされるので、*キーボード*固有の機能を実装するのに適しています。

以下はその一例です。

```cmake:CMakeLists.txt
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

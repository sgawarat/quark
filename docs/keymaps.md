## キーマップ

*キーマップ*は仮想キーボードの挙動を定義するものです。

## 構成

- `keyboards/`
    - `<keyboard>/`
        - `keymaps/`
            - `<keymap>/`
                - `keymap.c`
                - (`config.h`)
                - (`CMakeLists.txt`)

`<keyboard>`に対する*キーマップ*は`keyboards/<keyboard>/`内の`<keymap>/`に配置されます。`<keymap>`はキーマップ名を表す説明用の名前で、実際のキーマップ名には`/`を含めることができます。

### keymap.c

*キーマップ*は`<keymap>/`直下に`keymap.c`を必要とします。この`keymap.c`はQMKとともにビルドされるため、QMKの必要とする`keymaps`変数を定義するのに適しています。`keymap.c`では、*キーボード*の`keymap.h`へのパスが`QMK_KEYBOARD_H`に定義され、これをインクルードすることによって必要最低限の準備が整う設計になっています。

以下はその一例です。

```c:keymap.c
#include QMK_KEYBOARD_H  // キーボードのkeymap.h

// QMKで使われるkeymaps変数を定義する
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(...),  // keymap.hで定義されているマクロが使える
};
```

### config.h

`<keymap>/`直下に`config.h`が存在する場合、それはすべてのソースファイルで暗黙的にインクルードされます。`config.h`は*キーボード*の`config.h`の後かつ`post_config.h`の前にインクルードされるので、*キーマップ*固有の機能オプションを設定するのに適しています。

以下はその一例です。

```c:config.h
#pragma once

// Nキーロールオーバーをデフォルトで有効化
#define NKRO_DEFAULT_ON true
```

### CMakeLists.txt

`<keymap>/`直下に`CMakeLists.txt`が存在する場合、そのディレクトリはビルド対象に含まれるようになります。そこでは、cmakeの変数としてQMKのビルドオプションを設定したり、`keymap`というライブラリを定義したりすることができます。この`keymap`ライブラリは、そのままQMKにリンクされるので、*キーマップ*固有の機能を実装するのに適しています。

以下はその一例です。

```cmake:CMakeLists.txt
# ビルドオプションを設定できる
set(NKRO_ENABLE yes PARENT_SCOPE)

# keymapライブラリを定義できる
add_library(keymap STATIC
    keymap.cpp
)
target_link_libraries(keyboard PRIVATE
    config  # 共通設定をまとめたライブラリ
)
```

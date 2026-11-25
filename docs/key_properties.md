# キー

## キー表現

QuarkはOSとQMKの橋渡しを行うソフトウェアですので、その**キー**には*OSによって表現される形式*と*QMKによって表現される形式*の2種類が存在します。OSのキー表現は直ちにQMKのキー表現へ変換されるため、ユーザーがOSのキー表現に触れることはありません。ただし、OSのキー表現とQMKのキー表現は完全には対応しないため、重複するキーや使用できないキーが存在する可能性があることに注意してください。この変換は[win32/key.hpp](../include/quark/win32/key.hpp)や[linux/key.hpp](../include/quark/linux/key.hpp)で定義されています。

一方、QMKのキー表現は[quantum/key.hpp](../include/quark/quantum/key.hpp)で定義され、`KeyCode`型に包まれて扱われます。各定数はオリジナルの`KC_*`のものをそのまま用います。

## プロパティ

QuarkはOSとQMKの橋渡しを行うソフトウェアですが、キーの配置や挙動といった関連情報は既定では設定されていません。Quarkでは、ユーザーがこのキーごとの関連情報を`get_key_property`関数に実装する設計となっています。そして、これを実装する場所として`keyboard`ライブラリや`keymap`ライブラリといった仕組みが用意されています。構成の詳細については[keyboards](keyboards.md)や[keymaps](keymaps.md)を参照してください。

`get_key_property`関数は以下のようなタグディスパッチ関数として定義されます。

```cpp
namespace quark {
template <typename T>
struct KeyPropertyTag {};

template <typename T>
T get_key_property(KeyCode keycode) {
  return get_key_property(keycode, KeyPropertyTag<T>{});
}
}
```

ユーザーは各プロパティ`T`に対して`T get_key_property(KeyCode, KeyPropertyTag<T>)`を実装する必要があります。各プロパティは[quark/quantum/key.hpp](../include/quark/quantum/key.hpp)に宣言されています。

### keypos_tプロパティ

`keypos_t`プロパティは実際に入力されたキーの種類（`KeyCode`型）が仮想的な*キーボード*上のどの位置にあるか（`keypos_t`型）という対応関係を定義します。この`keypos_t`の値はキー入力の状態管理やキーマップの参照のさいにインデックスとして用いられます。

これを簡易に定義するために`define_layout`関数が用意されています。これは`KeyCode`型の2次元配列から`keypos_t`型の1次元配列を生成する関数で、キーの並びそのものを連続する`keypos_t`に変換します。

### NonHoldableプロパティ

`NonHoldable`プロパティはキーのリリースを待たずに即座に離した扱いにするかを定義します。そのため、このプロパティを設定したキーは長押しを表現できなくなります。

これは主に、押したときにのみキーイベントを発生させるキーを通常フローの中で正常に動作させることを目的としています。具体的には、日本語環境のWindowsにおいて、「半角全角」「英数」「カタカナひらがな」といったIMEの状態を変更するキーは離したときにキーイベントを発生させないので、このプロパティで挙動を変更する必要があります。

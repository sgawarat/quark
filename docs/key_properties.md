# キーのプロパティ

QuarkはOSとQMKの橋渡しを行うソフトウェアですが、キーの配置や挙動といった関連情報はユーザーが設定できます。Quarkでは、このキーごとの関連情報を`get_key_property`関数に実装する設計となっています。そして、これを実装する場所として`keyboard`ライブラリや`keymap`ライブラリといった仕組みが用意されています。構成の詳細については[keyboards](keyboards.md)や[keymaps](keymaps.md)を参照してください。

## get_key_property関数

`get_key_property`関数は以下のようなタグディスパッチ関数として定義されます。

```cpp
namespace quark {
template <typename T>
struct KeyPropertyTag {};

template <typename T>
T get_key_property(Key key) {
  return get_key_property(key, KeyPropertyTag<T>{});
}
}
```

ユーザーは各プロパティ`T`に対して`T get_key_property(Key, KeyPropertyTag<T>)`を実装する必要があります。

### keypos_tプロパティ

`keypos_t`プロパティはOSのキー表現からQMKのキー表現への変換を定義します。

QuarkはOSのキー表現を連続的に再配置した`Key`型（[key.hpp](../include/quark/key.hpp)）としてキー入力を表現する一方、QMKはキーをMatrixという二次元配列上の位置（`keypos_t`型）として表現します。各キーのMatrix上における配置は*キーボード*によって定義されるので、`Key`型と`keypos_t`型の対応関係もまた*キーボード*によって定義されなければなりません。

### NonHoldableプロパティ

`NonHoldable`プロパティは、そのキーが長押しできないことを定義します。

Windowsでは、押したときにのみキーイベントを発生させるキーが存在します。そのようなキーは通常のフローで正常に処理できないので、このプロパティを設定する必要があります。このプロパティを設定したキーは"押したと同時に離した"と解釈されて長押しを表現できなくなりますが、とりあえず通常のフローの中で正常に動作させることができます。

具体的には、日本語環境での「半角全角」「英数」「カタカナひらがな」といったIMEの状態を変更するキーは離したときにキーイベントを発生させないようなので、このプロパティを設定すると良いでしょう。これらのキーはOSの仕様に従って事前に決定できるはずですが、言語環境によってどのような違いがあるかを確認できていないため、その判断はユーザーにおまかせするかたちとなっています。

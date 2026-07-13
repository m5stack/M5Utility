# M5Utility

## Overview
This library is a collection of common functionality for other M5Libraries and products.

## Supported environments
Requires a C++11 (or later) toolchain that ships the C++ standard library (libstdc++ / libc++).

- ESP32 family (ESP-IDF and Arduino-ESP32, all chips)
- Non-ESP32 Arduino cores with the C++ standard library (e.g. SAMD, RP2040, Renesas RA (UNO R4))
- Hosted C++ environments (native unit tests etc.)

AVR cores (Arduino Uno / Mega etc.) are **not** supported because avr-gcc does not provide the C++ standard library.

## UnitTest (For internal maintainers)
UnitTest is included, which can run on PlatformIO.

## Doxygen document

[GitHub Pages](https://m5stack.github.io/M5Utility/)

If you want to generate documents on your local machine, execute the following command

```
bash docs/doxy.sh
```

It will output it under docs/html  
If you want to output Git commit hashes to html, do it for the git cloned folder.

### Required
- [Doxygen](https://www.doxygen.nl/)
- [pcregrep](https://formulae.brew.sh/formula/pcre2)
- [Git](https://git-scm.com/) (Output commit hash to html)


---

## 概要
このライブラリは M5Stack 関連ライブラリのための共通ライブラリです。

## 対応環境
C++11 以降かつ C++ 標準ライブラリ (libstdc++ / libc++) を持つツールチェーンが必要です。

- ESP32 ファミリー (ESP-IDF および Arduino-ESP32、全チップ)
- C++ 標準ライブラリを持つ非 ESP32 Arduino コア (SAMD、RP2040、Renesas RA (UNO R4) など)
- ホスト C++ 環境 (ネイティブユニットテストなど)

AVR コア (Arduino Uno / Mega など) は avr-gcc が C++ 標準ライブラリを提供しないため **非対応** です。

## ユニットテスト(メンテナンス用)
googleTest によるユニットテストを内包しています。 PlatformIO で実行できます。


## Doxygen ドキュメント
[GitHub Pages](https://m5stack.github.io/M5Utility/)

あなたのローカルマシンでドキュメントを生成したい場合は、以下のコマンドを実行してください。
```
bash docs/doxy.sh
```
docs/html の下に出力されます。  
Git コミットのハッシュを html に出力したい場合は、 git クローンしたフォルダに対して実行してください。

### 必要な物
- [Doxygen](https://www.doxygen.nl/)
- [pcregrep](https://formulae.brew.sh/formula/pcre2)
- [Git](https://git-scm.com/)





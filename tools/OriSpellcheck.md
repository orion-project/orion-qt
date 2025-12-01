# `Ori::Spellcheck`

[Spellcheck](./OriSpellcheck.h) provides spell checking functionality for `QTextEdit` via the [Hunspell](https://github.com/hunspell/hunspell) library.

## Enable module

This is an optional component so it doesn't not compile by default and should be enabled explicitly in particular application project files that are going to use it. In the target `.pro` file:

```cmake
include($$ORION/orion.pri)
include($$ORION/orion_spellcheck.pri)
```

or in `CMakeLists.txt`:

```cmake
set(ORI_WITH_SPELLCHECK ON)
add_subdirectory(libs/orion)
```

## Download dependency

The [hunspell](http://hunspell.github.io/) library is used for spell checking. If a project is going to use spell checking functionality, the hunspell code must be downloaded or git-cloned explicitly to the `orion` repo root (the hunspell directory is already git-ignored). For example:

```bash
git clone https://github.com/hunspell/hunspell
cd hunspell
git checkout v1.7.2
```

### Download dictionaries

In the example below, a downloaded zip package is the full set of dictionaries for [LibreOffice](https://github.com/LibreOffice/dictionaries) (74M). One have to extract `.dic` and `.aff` files for required languages.

By default, the `Ori::Spellcheck` module loads dictionaries from the `dicts` directory nearby the running executable. Orion examples are compiled into the `bin` directory in the `orion` repo root; so this is why we unzip from `hunspell` into `../bin/dicts`:

```bash
curl https://codeload.github.com/LibreOffice/dictionaries/zip/libreoffice-6.3.0.4 > libreoffice-6.3.0.4.zip
unzip -j libreoffice-6.3.0.4.zip dictionaries-libreoffice-6.3.0.4/en/en_US.* -d ../bin/dicts
unzip -j libreoffice-6.3.0.4.zip dictionaries-libreoffice-6.3.0.4/ru_RU/ru_RU.* -d ../bin/dicts
```

## Usage

```cpp
#include "tools/OriSpellcheck.h"

#include <QTextEdit>

// ...

auto editor = new QTextEdit;

auto spellcheck = new Ori::Spellcheck(editor);
spellcheck->setLang("en_US"); // Enable spell checking
```

The editor widget takes ownership on the spellcheck object.

## Demo

See the [demo project](../examples/spellcheck/main.cpp) for how to use the module.

![](../examples/spellcheck/spellcheck.png)

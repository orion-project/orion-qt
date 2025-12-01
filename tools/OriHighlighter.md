# `Ori::Highlighter`

[Highlighter](./OriHighlighter.h) is a simple implementation of `QSyntaxHighlighter` based on regular expressions and predefined rule sets stored in custom format.

## Usage

```cpp
#include "tools/OriHighlighter.h"

//...

auto editor = new QPlainTextEdit;
Ori::Highlighter::setHighlighter(editor, "../syntax/python.ohl");
```

## See also

- [Example rules](../syntax/README.md)
- [Rule editor utility](../utils/ohl_editor/README.md)
- [Code editor widget](../widgets/OriCodeEditor.md)

<br><img src="OriHighlighter.png" width="600px">



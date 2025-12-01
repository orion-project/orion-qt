# [`Ori::Gui::PopupMessage`](./OriPopupMessage.h)

[PopupMessage](./OriPopupMessage.h) is a temporary popup message, automatically hidden by timeout. 

## Usage

```cpp
#include "widgets/OriPopupMessage.h"

Ori::Gui::PopupMessage::affirm("Image has been copied to Clipboard");
```

Pass zero duration to disable auto-hide behavior, then use `cancel()` to close the message.

## Appearance

The message is transparent by default. To give it some look consistent with the app, use application styles:

For example:

```css
#OriPopupMessage {
  border-radius: 4px;
}
#OriPopupMessage QLabel {
  font-size: 10pt;
  margin: 15px;
}
#OriPopupMessage[mode=warning] {
  border: 1px solid #f3af8f;
  background: #ffdcbc;
}
#OriPopupMessage[mode=affirm] {
  border: 1px solid #7ee87e;
  background: #b5fbb5;
}
#OriPopupMessage[mode=error] {
  border: 1px solid #e87e7e;
  background: #ffb5b5;
}
#OriPopupMessage[mode=hint] {
  border: 1px solid #e5c300;
  background: #ffec7c;
}
```

<img src="OriPopupMessage.png" width="600px">


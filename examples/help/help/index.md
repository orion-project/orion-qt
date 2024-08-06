# Ori::HelpWindow

HelpWindow provides a quick and simple way for displaying app documentation as a simple set of markdown files. It allows for avoiding usage of intermediate file format, help compiles, bundling Qt Assistant or other help viewer tools.

By default it searches for documentation files in the `help` subdirectory near the application executable. The root path can be configured by setting a static getter before usage of the window:

```c++
Ori::HelpWindow::getHelpDir = []{
    return qApp->applicationDirPath() + "/../examples/help/help";
};
```

## Styles

The default text styles in [QTextBrowser](https://doc.qt.io/qt-6/qtextbrowser.html) probably do not fit well for displaying documentation files. If so, one can provide a stylesheet file via application resources by settings the `:/style/help` resource path:

```xml
<qresource prefix="/style">
    <file alias="help">help.css</file>
</qresource>
```

## See also

- [Dev commands](./dev.md)

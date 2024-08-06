# Dev commands

Dev commands are available when a static flag is set:

```c++
Ori::HelpWindow::isDevMode = true;
```

Commands:

- Refresh
- Edit stylesheet

## Stylesheet editor

Customize where the CSS source file is:

```c++
Ori::HelpWindow::getCssSrc = []{
    return QString(qApp->applicationDirPath() + "/../examples/help/help.css");
};
```

Add highlighter rules to the app resource file to get CSS highlighting in the editor:

```xml
<qresource prefix="/syntax">
    <file alias="css">../../syntax/css.ohl</file>
</qresource>
```

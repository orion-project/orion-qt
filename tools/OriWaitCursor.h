#ifndef ORI_WAIT_CURSOR_H
#define ORI_WAIT_CURSOR_H

#include <QApplication>

namespace Ori {

class WaitCursor
{
public:
    WaitCursor()
    {
        show();
    }

    ~WaitCursor()
    {
        restore();
    }

    void show() const
    {
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();
    #endif
    }

    void restore() const
    {
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
    #endif
    }
};

} // namespace Ori

#endif // ORI_WAIT_CURSOR_H


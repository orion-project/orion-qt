#ifndef ORI_STYLER_H
#define ORI_STYLER_H

#include <QObject>

namespace Ori {

class Styler : public QObject
{
    Q_OBJECT

public:
    Styler(QObject* parent = 0);
    Styler(const QString& style, QObject* parent = 0);

    QString currentStyle() const;
    void setCurrentStyle(const QString& style);

    QStringList getStyles() const;
};

} // namespace Ori

#endif // ORI_STYLER_H

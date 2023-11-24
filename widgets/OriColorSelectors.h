#ifndef ORI_COLOR_SELECTORS_H
#define ORI_COLOR_SELECTORS_H

#include <QToolButton>
#include <QPen>

namespace Ori {
namespace Widgets {

//------------------------------------------------------------------------------
//                                 ColorGrid
//------------------------------------------------------------------------------

class ColorGrid : public QWidget
{
    Q_OBJECT

public:
    ColorGrid(QWidget* parent = nullptr);

    void setColors(const QList<QColor> &colors, int colCount = 0);
    void setColCount(int value);
    void setCellSize(int value);
    void setCellSpacing(int value);
    bool showTooltip() const { return _showTooltip; }
    void setShowTooltip(bool on) { _showTooltip = on; }

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

signals:
    void hovered(const QColor&);
    void selected(const QColor&);
    void rejected();

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void leaveEvent(QEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    int _cellSize = 20, _cellSpacing = 2;
    int _colCount = 10, _rowCount = 0;
    int _curIndex = -1;
    bool _showTooltip = false;
    QPixmap _backBuffer;
    QList<QColor> _colors;

    void calcRowCount();
    void drawBackBuffer();
};

//------------------------------------------------------------------------------
//                               BaseColorButton
//------------------------------------------------------------------------------

class BaseColorButton : public QToolButton
{
    Q_OBJECT

public:
    QColor selectedColor() const { return _color; }
    void setSelectedColor(const QColor& value);
    QIcon baseIcon() const { return _baseIcon; }
    void setBaseIcon(const QIcon& icon);
    QRect iconRect() const { return _iconRect; }
    void setIconRect(const QRect& r);

    bool drawIconFrame = true;
    bool allowTransparentColors = false;
    bool useNativeColorDialog = false;

signals:
    void colorSelected(const QColor&);

protected slots:
    void showColorDialog();

protected:
    QColor _color = Qt::white;

    BaseColorButton(QWidget* parent);

    void resizeEvent(QResizeEvent*) override;

private:
    QIcon _baseIcon;
    QRect _iconRect;
};

//------------------------------------------------------------------------------
//                                 ColorButton
//------------------------------------------------------------------------------

class ColorButton : public BaseColorButton
{
    Q_OBJECT

public:
    ColorButton(QWidget* parent = nullptr);
};

//------------------------------------------------------------------------------
//                               PopupColorButton
//------------------------------------------------------------------------------

class PopupColorButton : public BaseColorButton
{
    Q_OBJECT

public:
    PopupColorButton(QWidget* parent = nullptr);

    void setColors(const QList<QColor> &colors, int colCount = 0) { _grid->setColors(colors, colCount); }
    void setColCount(int value) { _grid->setColCount(value); }
    void setCellSize(int value) { _grid->setCellSize(value); }
    void setCellSpacing(int value) { _grid->setCellSpacing(value); }
    bool showTooltip() const { return _grid->showTooltip(); }
    void setShowTooltip(bool on) { _grid->setShowTooltip(on); }
    bool showColorDialog() const;
    void setShowColorDialog(bool on);

private:
    ColorGrid *_grid;
    QAction *_actnColorDlg;
    QMenu *_menu;

    void onGridColorSelected(const QColor& color);
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_COLOR_SELECTORS_H

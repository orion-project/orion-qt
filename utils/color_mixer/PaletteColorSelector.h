#ifndef PALETTECOLORCOMBO_H
#define PALETTECOLORCOMBO_H

#include <QComboBox>

class PaletteGroupCombo : public QComboBox
{
    Q_OBJECT

public:
    explicit PaletteGroupCombo(QWidget *parent = 0);

    QPalette::ColorGroup selectedGroup() const { return _selectedGroup; }

signals:
    void groupSelected();

private slots:
    void itemSelected(int index);

private:
    QPalette::ColorGroup _selectedGroup;
};


class PaletteRoleCombo : public QComboBox
{
    Q_OBJECT

public:
    explicit PaletteRoleCombo(QWidget *parent = 0);

    QPalette::ColorRole selectedRole() const { return _selectedRole; }

signals:
    void roleSelected();

private slots:
    void itemSelected(int index);

private:
    QPalette::ColorRole _selectedRole;
};

class PaletteColorSelector : public QWidget
{
    Q_OBJECT

public:
    explicit PaletteColorSelector(QWidget *parent = 0);

    const QColor& selectedColor() const { return _selectedColor; }

signals:
    void colorSelected(const QColor& color);

private slots:
    void makeColor();

private:
    QColor _selectedColor;
    PaletteGroupCombo* _groupCombo;
    PaletteRoleCombo* _roleCombo;
};

#endif // PALETTECOLORCOMBO_H

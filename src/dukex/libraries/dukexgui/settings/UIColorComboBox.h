#ifndef UICOLORCOMBOBOX_H
#define UICOLORCOMBOBOX_H

#include <QComboBox>

class QColor;
class QWidget;

class UIColorComboBox : public QComboBox {

    Q_OBJECT

public:
    UIColorComboBox(QWidget * parent= 0);

public:
    QColor color() const;
    void setColor(QColor c);

private slots:
    void on_this_currentIndexChanged(int); // manual

private:
    void populateList();
};

#endif // UICOLORCOMBOBOX_H

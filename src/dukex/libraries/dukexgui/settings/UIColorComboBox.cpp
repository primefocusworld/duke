#include "UIColorComboBox.h"
#include <QColor>
#include <QWidget>
#include <iostream>

UIColorComboBox::UIColorComboBox(QWidget * parent) :
    QComboBox(parent) {
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_this_currentIndexChanged(int)));
    QStringList colorNames = QColor::colorNames();
    for (int i = 0; i < colorNames.size(); ++i) {
        QColor color(colorNames[i]);
        insertItem(i, colorNames[i]);
        setItemData(i, color, Qt::DecorationRole);
    }
}

QColor UIColorComboBox::color() const {
    return qVariantValue<QColor> (itemData(currentIndex(), Qt::DecorationRole));
}

void UIColorComboBox::setColor(QColor color) {
    setCurrentIndex(findData(color, int(Qt::DecorationRole)));
}

void UIColorComboBox::on_this_currentIndexChanged(int index){
    QString sheet = QString("QComboBox::drop-down { border-left-width: 10px; border-left-color: %1;}").arg(color().name ());
    setStyleSheet(sheet);
}

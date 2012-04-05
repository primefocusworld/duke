#include <QtGui>
#include <iostream>
#include "UISettingsPage.h"

//
// GeneralSettingsPage
//
GeneralSettingsPage::GeneralSettingsPage(QWidget *parent) :
    QWidget(parent) {
    ui.setupUi(this);
}

//
// CachingSettingsPage
//
CachingSettingsPage::CachingSettingsPage(QWidget *parent) :
    QWidget(parent) {
    ui.setupUi(this);
}

void CachingSettingsPage::on_cacheModeComboBox_currentIndexChanged(int index){
    std::cerr << "on_cacheModeComboBox_currentIndexChanged !! " << index << std::endl;
    if(index == 1) { // no caching
        ui.cacheSizeLabel->setEnabled(false);
        ui.cacheSizeDoubleSpinBox->setEnabled(false);
        ui.readerThreadsLabel->setEnabled(false);
        ui.readerThreadsSpinBox->setEnabled(false);
    } else {
        ui.cacheSizeLabel->setEnabled(true);
        ui.cacheSizeDoubleSpinBox->setEnabled(true);
        ui.readerThreadsLabel->setEnabled(true);
        ui.readerThreadsSpinBox->setEnabled(true);
    }
}

//
// RenderingSettingsPage
//
RenderingSettingsPage::RenderingSettingsPage(QWidget *parent) :
    QWidget(parent) {
    ui.setupUi(this);
    ui.backgroundColorComboBox->setColor(Qt::black);
}

#include <QtGui>
#include "UISettingsDialog.h"
#include "UISettingsPage.h"

UISettingsDialog::UISettingsDialog(QWidget *parent) :
    QDialog(parent) {
    ui.setupUi(this);
}


void UISettingsDialog::on_listWidget_itemEntered(QListWidgetItem * item){
}

void UISettingsDialog::on_listWidget_itemClicked(QListWidgetItem * item){
    ui.stackedWidget->setCurrentIndex(ui.listWidget->row(item));
}


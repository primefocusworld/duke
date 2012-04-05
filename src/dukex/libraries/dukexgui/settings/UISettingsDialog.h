#ifndef UISETTINGSDIALOG_H
#define UISETTINGSDIALOG_H

#include "ui_settingsdialog.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
QT_END_NAMESPACE

class UISettingsDialog : public QDialog {

Q_OBJECT //

public:
    UISettingsDialog(QWidget *parent = 0);

public slots:
    void on_listWidget_itemEntered(QListWidgetItem * item);
    void on_listWidget_itemClicked(QListWidgetItem * item);

private:
    Ui::settingsDialog ui;
};

#endif // UISETTINGSDIALOG_H

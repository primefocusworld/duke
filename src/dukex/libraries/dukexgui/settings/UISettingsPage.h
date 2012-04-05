#ifndef UISETTINGSPAGE_H
#define UISETTINGSPAGE_H

#include <QWidget>
#include "ui_generalsettings.h"
#include "ui_cachingsettings.h"
#include "ui_renderingsettings.h"

class GeneralSettingsPage : public QWidget {
public:
    GeneralSettingsPage(QWidget *parent = 0);
private:
    Ui::generalSettings ui;
};

class CachingSettingsPage : public QWidget {
    Q_OBJECT //
public:
    CachingSettingsPage(QWidget *parent = 0);
private slots:
    void on_cacheModeComboBox_currentIndexChanged(int);
private:
    Ui::cachingSettings ui;
};

class RenderingSettingsPage : public QWidget {
public:
    RenderingSettingsPage(QWidget *parent = 0);
private:
    Ui::renderingSettings ui;
};

#endif // UISETTINGSPAGE_H

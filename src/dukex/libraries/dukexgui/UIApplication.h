#ifndef UIAPPLICATION_H
#define UIAPPLICATION_H

#include "ui_mainwindow.h"
#include "settings/UserPreferences.h"
#include <dukexcore/dkxNodeManager.h>
#include <dukexcore/dkxSession.h>
#include <QtGui>

class UIRenderWindow;
class UIFileDialog;
class UISettingsDialog;
class UIImageInfo;
class UITimeline;

class UIApplication : public QMainWindow {

Q_OBJECT

public:
    UIApplication(Session::ptr s);
    ~UIApplication() {
    }

private:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeCentralWidget(const QSize& resolution);
    void updateRecentFilesMenu();

private slots:
    void start();
    // file
    void openFiles(const QStringList &, const bool & browseMode, const bool & parseSequence);
    void openFiles();
    void openRecent();
    void browseDirectory();
    void savePlaylist();
    void openPreferences();
    // control
    void playStop();
    void previousFrame();
    void nextFrame();
    void firstFrame();
    void lastFrame();
    void previousShot();
    void nextShot();
    // display
    void colorspaceLIN();
    void colorspaceLOG();
    void colorspaceSRGB();
    void timeline();
    void imageInfo();
    // window
    void fullscreen();
    void toggleFitMode();
    void fitToNormalSize();
    void fitImageToWindowWidth();
    void fitImageToWindowHeight();
    void zoom(double);
    void pan(double, double);
    // ?
    void about();

    void topLevelChanged(bool); // TEST

private:
    Ui::mainWindow ui;
    NodeManager m_Manager;
    Session::ptr m_Session;
    UserPreferences m_Preferences;
    UIRenderWindow* m_RenderWindow;
    UIFileDialog* m_FileDialog;
    UISettingsDialog* m_SettingsUI;
    UIImageInfo* m_ImageInfoUI;
    UITimeline* m_TimelineUI;
    int m_timerID;
    bool mStarted;
};

#endif // UIAPPLICATION_H

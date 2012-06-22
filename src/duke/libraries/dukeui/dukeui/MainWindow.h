#ifndef MainWindow_H
#define MainWindow_H

#include "ui_mainwindow.h"
#include "settings/UserPreferences.h"
#include "timeline/UITimeline.h"
#include "utils/UIFileDialog.h"

class IDukeEngine;

class MainWindow : public QMainWindow {

Q_OBJECT

public:
    MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow() {
    }

    void bind(IDukeEngine *pEngine) {
        m_pEngine = pEngine;
    }

private:
    bool event(QEvent * pEvent);
    void closeEvent(QCloseEvent *event);
    void resizeCentralWidget(const QSize& resolution);
    void updateRecentFilesMenu();

private slots:
    // file
    void openFiles(const QStringList &, const bool & browseMode);
    void openFiles();
    void openRecent();
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
    //void imageInfo();
    // window
    void fullscreen();
    void toggleFitMode();
    void fitToNormalSize();
    void fitImageToWindowWidth();
    void fitImageToWindowHeight();
    // ?
    void about();

private:
    IDukeEngine *m_pEngine;
    Ui::mainWindow ui;
    UserPreferences mPreferences;
    UITimeline mTimeline;
    UIFileDialog mFileDialog;
};

#endif // MainWindow_H

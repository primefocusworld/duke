#ifndef MainWindow_H
#define MainWindow_H

#include "ui_mainwindow.h"
#include "settings/UserPreferences.h"
#include "timeline/UITimeline.h"
#include "utils/UIFileDialog.h"

class MainWindow : public QMainWindow {

Q_OBJECT

public:
    MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow() {
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
    void zoom(double);
    void pan(double, double);
    // ?
    void about();

    void topLevelChanged(bool); // TEST

private:
    Ui::mainWindow ui;
    UserPreferences mPreferences;
    UITimeline mTimeline;
    UIFileDialog mFileDialog;
};

#endif // MainWindow_H

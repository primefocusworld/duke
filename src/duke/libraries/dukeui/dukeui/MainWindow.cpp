#include "MainWindow.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <QDockWidget>
#include <QShowEvent>
#include <QDebug>
#include <QMessageBox>
#include <dukeapi/messageBuilder/TransportBuilder.h>
#include <dukeengine/renderer/DukeEngine.h>
#include <dukeengine/Version.h>

using namespace ::duke::protocol;
using namespace google::protobuf::serialize;

namespace {

void PUSH(IDukeEngine* pEngine, const google::protobuf::Message & m) {
    if (pEngine) {
        MessageHolder holder;
        pack(m, holder);
        pEngine->onEvent(holder);
    }
}

} // empty namespace

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags) :
                QMainWindow(parent, flags), m_pEngine(NULL) {
    // UI
    ui.setupUi(this);
    addDockWidget(Qt::BottomDockWidgetArea, &mTimeline);

    // Preferences
    mPreferences.loadShortcuts(this);
    mPreferences.loadFileHistory();
    updateRecentFilesMenu();

    // Actions
    // File Actions
    connect(ui.openFileAction, SIGNAL(triggered()), this, SLOT(openFiles()));
    connect(ui.savePlaylistAction, SIGNAL(triggered()), this, SLOT(savePlaylist()));
    connect(ui.quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.preferencesAction, SIGNAL(triggered()), this, SLOT(openPreferences()));
    // Control Actions
    connect(ui.playStopAction, SIGNAL(triggered()), this, SLOT(playStop()));
    connect(ui.nextFrameAction, SIGNAL(triggered()), this, SLOT(nextFrame()));
    connect(ui.previousFrameAction, SIGNAL(triggered()), this, SLOT(previousFrame()));
    connect(ui.firstFrameAction, SIGNAL(triggered()), this, SLOT(firstFrame()));
    connect(ui.lastFrameAction, SIGNAL(triggered()), this, SLOT(lastFrame()));
    connect(ui.nextShotAction, SIGNAL(triggered()), this, SLOT(nextShot()));
    connect(ui.previousShotAction, SIGNAL(triggered()), this, SLOT(previousShot()));
    // Display Actions
    connect(ui.LINAction, SIGNAL(triggered()), this, SLOT(colorspaceLIN()));
    connect(ui.LOGAction, SIGNAL(triggered()), this, SLOT(colorspaceLOG()));
    connect(ui.SRGBAction, SIGNAL(triggered()), this, SLOT(colorspaceSRGB()));
    QActionGroup * displayActionGroup = new QActionGroup(this);
    displayActionGroup->addAction(ui.LINAction);
    displayActionGroup->addAction(ui.LOGAction);
    displayActionGroup->addAction(ui.SRGBAction);
    connect(ui.timelineAction, SIGNAL(triggered()), this, SLOT(timeline()));
    // Window Actions
    connect(ui.fullscreenAction, SIGNAL(triggered()), this, SLOT(fullscreen()));
    connect(ui.toggleFitModeAction, SIGNAL(triggered()), this, SLOT(toggleFitMode()));
    connect(ui.fitImageTo11Action, SIGNAL(triggered()), this, SLOT(fitToNormalSize()));
    connect(ui.fitImageToWindowWidthAction, SIGNAL(triggered()), this, SLOT(fitImageToWindowWidth()));
    connect(ui.fitImageToWindowHeightAction, SIGNAL(triggered()), this, SLOT(fitImageToWindowHeight()));
    // About Actions
    connect(ui.aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

bool MainWindow::event(QEvent * pEvent) {
    assert(pEvent);
    switch (pEvent->type()) {
        case QEvent::ShortcutOverride: {
            QKeyEvent* e = static_cast<QKeyEvent*>(pEvent);
            assert(e);
            if (e->modifiers() == Qt::NoModifier) {
                switch (e->key()) {
                    case Qt::Key_F: // fit
                    case Qt::Key_Left: // previous
                    case Qt::Key_Right: // next
                    case Qt::Key_PageDown: // previous shot
                    case Qt::Key_PageUp: // next shot
                    case Qt::Key_Space: // play/stop
                        pEvent->accept();
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        default:
            break;
    }
    return QMainWindow::event(pEvent);
}

// private
void MainWindow::closeEvent(QCloseEvent *event) {
    if (isFullScreen()) {
        showNormal();
        event->ignore();
        return;
    }
    mPreferences.saveShortcuts(this);
    mPreferences.saveFileHistory();
    event->accept();
}

void MainWindow::updateRecentFilesMenu() {
    ui.openRecentMenu->clear();
    for (size_t i = 0; i < mPreferences.history().size(); ++i) {
        if (mPreferences.history(i) == "")
            continue;
        boost::filesystem::path fn(mPreferences.history(i));
        QAction * act = ui.openRecentMenu->addAction(mPreferences.history(i).c_str());
        act->setData(mPreferences.history(i).c_str());
        connect(act, SIGNAL(triggered()), this, SLOT(openRecent()));
    }
}

// private slot
void MainWindow::openFiles(const QStringList & _list, const bool & browseMode) {
    QMessageBox::about(this, tr("openFiles"), tr("Not yet implemented"));
}

// private slot
void MainWindow::openFiles() {
    QStringList list;
    if (mFileDialog.exec()) {
        list = mFileDialog.selectedFiles();
        if (list.size() != 0)
            openFiles(list, mFileDialog.browseDirectory());
    }
}

// private slot
void MainWindow::openRecent() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString file = action->data().toString();
        if (!file.isEmpty()) {
            QStringList filenames;
            if (file.startsWith("sequence://")) {
                file.remove(0, 11);
                filenames.append(file);
                openFiles(filenames, false);
            } else if (file.startsWith("directory://")) {
                file.remove(0, 12);
                filenames.append(file);
                openFiles(filenames, true);
            }
        }
    }
}

// private slot
void MainWindow::savePlaylist() {
    /*
     QString file = QFileDialog::getSaveFileName(this, tr("Save Current Playlist"), QString(), tr("Duke Playlist (*.dk);;All(*)"));
     if (!file.isEmpty()) {
     if (!file.endsWith(".dk"))
     file.append(".dk");
     SceneNode::ptr scene = m_Manager.nodeByName<SceneNode> ("fr.mikrosimage.dukex.scene");
     if (scene.get() == NULL)
     return;
     scene->save(file.toStdString());
     }*/
    QMessageBox::about(this, tr("savePlaylist"), tr("Not yet implemented"));
}

// private slot
void MainWindow::openPreferences() {
    QMessageBox::about(this, tr("openPreferences"), tr("Not yet implemented"));
}

// private slot
void MainWindow::playStop() {
    PUSH(m_pEngine, TransportBuilder::play());
}

// private slot
void MainWindow::previousFrame() {
    PUSH(m_pEngine, TransportBuilder::previousFrame());
}

// private slot
void MainWindow::nextFrame() {
    PUSH(m_pEngine, TransportBuilder::nextFrame());
}

// private slot
void MainWindow::firstFrame() {
    PUSH(m_pEngine, TransportBuilder::firstFrame());
}

// private slot
void MainWindow::lastFrame() {
    PUSH(m_pEngine, TransportBuilder::lastFrame());
}

// private slot
void MainWindow::previousShot() {
    PUSH(m_pEngine, TransportBuilder::previousShot());
}

// private slot
void MainWindow::nextShot() {
    PUSH(m_pEngine, TransportBuilder::nextShot());
}

// private slot
void MainWindow::colorspaceLIN() {
    QMessageBox::about(this, tr("colorspaceLIN"), tr("Not yet implemented"));
}

// private slot
void MainWindow::colorspaceLOG() {
    QMessageBox::about(this, tr("colorspaceLOG"), tr("Not yet implemented"));
}

// private slot
void MainWindow::colorspaceSRGB() {
    QMessageBox::about(this, tr("colorspaceSRGB"), tr("Not yet implemented"));
}

void MainWindow::timeline() {
    QMessageBox::about(this, tr("timeline"), tr("Not yet implemented"));
}

// private slot
void MainWindow::fullscreen() {
    isFullScreen() ? showNormal() : showFullScreen();
}

// private slot
void MainWindow::toggleFitMode() {
    QMessageBox::about(this, tr("toggleFitMode"), tr("Not yet implemented"));
}

// private slot
void MainWindow::fitToNormalSize() {
    QMessageBox::about(this, tr("fitImageToWindowHeight"), tr("Not yet implemented"));
}

// private slot
void MainWindow::fitImageToWindowWidth() {
    QMessageBox::about(this, tr("fitImageToWindowHeight"), tr("Not yet implemented"));
}

// private slot
void MainWindow::fitImageToWindowHeight() {
    QMessageBox::about(this, tr("fitImageToWindowHeight"), tr("Not yet implemented"));
}

// private slot
void MainWindow::about() {
    QString msg(getVersion("Duke").c_str());
    QMessageBox::about(this, tr("About Duke"), msg);
}


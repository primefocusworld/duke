#include "MainWindow.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <QTimer>
#include <QDockWidget>
#include <QShowEvent>

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags) :
                QMainWindow(parent, flags) {
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

void MainWindow::showEvent(QShowEvent* event) {
    event->accept();
}

void MainWindow::topLevelChanged(bool b) {
    QDockWidget *dockwidget = qobject_cast<QDockWidget *>(sender());
    if (dockwidget && b) {
        dockwidget->setWindowOpacity(0.6);
//        dockwidget->move(mapToGlobal(m_RenderWindow->renderWidget()->pos()) + QPoint(40, 60));
        //dockwidget->adjustSize();
    }
}

// private
void MainWindow::closeEvent(QCloseEvent *event) {
//    if (m_timerID != 0)
//        QObject::killTimer(m_timerID);
    mPreferences.saveShortcuts(this);
    mPreferences.saveFileHistory();
    QMainWindow::closeEvent(event);
    event->accept();
}

// private
void MainWindow::timerEvent(QTimerEvent *event) {
    // retrieve in msgs
    event->accept();
}

// private
void MainWindow::keyPressEvent(QKeyEvent * event) {
    QMainWindow::keyPressEvent(event);
    switch (event->key()) {
        case Qt::Key_Escape:
            close();
            event->accept();
            break;
        default:
            break;
    }
    event->ignore();
}

// private
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

// private
void MainWindow::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

// private
void MainWindow::dropEvent(QDropEvent *event) {
    //    const QMimeData *mimeData = event->mimeData();
    //    if (mimeData->hasUrls()) {
    //        QList < QUrl > urlList = mimeData->urls();
    //        QString text;
    //        for (int i = 0; i < urlList.size() && i < 32; ++i) {
    //            openPlaylist(urlList.at(i).path());
    //        }
    //    } else {
    //        m_statusInfo->setText(tr("Cannot display data"));
    //    }
    //    setBackgroundRole(QPalette::Dark);
    event->acceptProposedAction();
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
//    SceneNode::ptr s = m_Manager.nodeByName<SceneNode> ("fr.mikrosimage.dukex.scene");
//    if (s.get() == NULL)
//        return;
//    if (!_list.isEmpty()) {
//        // --- QStringList to STL vector<string>
//        std::vector<std::string> v;
//        v.resize(_list.count());
//        for (int i = 0; i < _list.count(); ++i) {
//            v[i] = _list[i].toStdString();
//        }
//        s->openFiles(v, browseMode);
//        if (v.size() == 1) { // multi selection not handled in file history
//            QString history = _list[0];
//            if (browseMode) {
//                history.prepend("browse://");
//            } else {
//                if (parseSequence)
//                    history.prepend("sequence://");
//                else
//                    history.prepend("file://");
//            }
//            mPreferences.addToHistory(history.toStdString());
//            updateRecentFilesMenu();
//        }
//    }
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
//    QString file = QFileDialog::getSaveFileName(this, tr("Save Current Playlist"), QString(), tr("Duke Playlist (*.dk);;All(*)"));
//    if (!file.isEmpty()) {
//        if (!file.endsWith(".dk"))
//            file.append(".dk");
//        SceneNode::ptr scene = m_Manager.nodeByName<SceneNode> ("fr.mikrosimage.dukex.scene");
//        if (scene.get() == NULL)
//            return;
//        scene->save(file.toStdString());
//    }
}

// private slot
void MainWindow::openPreferences() {
//    if (!m_SettingsUI)
//        m_SettingsUI = new UISettingsDialog(this);
//    m_SettingsUI->show();
}

// private slot
void MainWindow::playStop() {
//    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
//    if (t.get() == NULL)
//        return;
//    if (m_Session->descriptor().isPlaying())
//        t->stop();
//    else
//        t->play();
}

// private slot
void MainWindow::previousFrame() {
//    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
//    if (t.get() == NULL)
//        return;
//    t->previousFrame();
}

// private slot
void MainWindow::nextFrame() {
//    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
//    if (t.get() == NULL)
//        return;
//    t->nextFrame();
}

// private slot
void MainWindow::firstFrame() {
//    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
//    if (t.get() == NULL)
//        return;
//    t->firstFrame();
}

// private slot
void MainWindow::lastFrame() {
//    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
//    if (t.get() == NULL)
//        return;
//    t->lastFrame();
}

// private slot
void MainWindow::previousShot() {
//    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
//    if (t.get() == NULL)
//        return;
//    t->previousShot();
}

// private slot
void MainWindow::nextShot() {
//    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
//    if (t.get() == NULL)
//        return;
//    t->nextShot();
}

// private slot
void MainWindow::colorspaceLIN() {
//    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
//    if (g.get() == NULL)
//        return;
//    g->setColorspace(::duke::playlist::Display::LIN);
}

// private slot
void MainWindow::colorspaceLOG() {
//    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
//    if (g.get() == NULL)
//        return;!
//    g->setColorspace(::duke::playlist::Display::LOG);
}

// private slot
void MainWindow::colorspaceSRGB() {
//    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
//    if (g.get() == NULL)
//        return;
//    g->setColorspace(::duke::playlist::Display::SRGB);
}

void MainWindow::timeline() {
//    if (!mTimelineUI) {
//        mTimelineUI = new UITimeline(&m_Manager);
//        m_Session->addObserver(mTimelineUI);
//        addDockWidget(Qt::BottomDockWidgetArea, mTimelineUI);
//    }
//    mTimelineUI->setVisible(!mTimelineUI->isVisible());
}

// private slot
void MainWindow::fullscreen() {
//    if (m_RenderWindow->isFullScreen()) {
//        m_RenderWindow->showNormal();
//    } else {
//        m_RenderWindow->showFullScreen();
//    }
}

// private slot
void MainWindow::toggleFitMode() {
//    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
//    if (f.get() == NULL)
//        return;
//    f->toggle();
}

// private slot
void MainWindow::fitToNormalSize() {
//    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
//    if (f.get() == NULL)
//        return;
//    f->fitToNormalSize();
}

// private slot
void MainWindow::fitImageToWindowWidth() {
//    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
//    if (f.get() == NULL)
//        return;
//    f->fitImageToWindowWidth();
}

// private slot
void MainWindow::fitImageToWindowHeight() {
//    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
//    if (f.get() == NULL)
//        return;
//    f->fitImageToWindowHeight();
}

// private slot
void MainWindow::zoom(double z) {
//    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
//    if (g.get() == NULL)
//        return;
//    g->setZoom(z);
}

// private slot
void MainWindow::pan(double x, double y) {
//    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
//    if (g.get() == NULL)
//        return;
//    g->setPan(x, y);
}

// private slot
void MainWindow::about() {
//    QString msg(getVersion("Duke").c_str());
//    QMessageBox::about(this, tr("About Duke"), msg);
}


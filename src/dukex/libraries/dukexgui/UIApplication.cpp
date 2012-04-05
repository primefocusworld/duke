#include "UIApplication.h"
#include "UIRenderWindow.h"
#include "UIFileDialog.h"
#include "UISettingsDialog.h"
#include "timeline/UITimeline.h"
#include "imageinfo/UIImageInfo.h"
#include "UISettingsDialog.h"
#include <dukeengine/Version.h>
#include <dukexcore/nodes/Commons.h>
#include <boost/filesystem.hpp>
#include <iostream>

UIApplication::UIApplication(Session::ptr s) :
    m_Session(s), //
                    m_RenderWindow(new UIRenderWindow(this)), //
                    m_SettingsUI(NULL), //
                    m_ImageInfoUI(NULL), //
                    m_TimelineUI(NULL), //
                    m_timerID(0), mStarted(false) {

    // Global UI
    ui.setupUi(this);
    setCentralWidget(m_RenderWindow);

    // Preferences
    m_Preferences.loadShortcuts(this);
    m_Preferences.loadFileHistory();
    updateRecentFilesMenu();

    // Actions
    // File Actions
    connect(ui.openFileAction, SIGNAL(triggered()), this, SLOT(openFiles()));
    connect(ui.browseDirectoryAction, SIGNAL(triggered()), this, SLOT(browseDirectory()));
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
    connect(ui.imageInfoAction, SIGNAL(triggered()), this, SLOT(imageInfo()));
    // Window Actions
    connect(ui.fullscreenAction, SIGNAL(triggered()), this, SLOT(fullscreen()));
    connect(ui.toggleFitModeAction, SIGNAL(triggered()), this, SLOT(toggleFitMode()));
    connect(ui.fitImageTo11Action, SIGNAL(triggered()), this, SLOT(fitToNormalSize()));
    connect(ui.fitImageToWindowWidthAction, SIGNAL(triggered()), this, SLOT(fitImageToWindowWidth()));
    connect(ui.fitImageToWindowHeightAction, SIGNAL(triggered()), this, SLOT(fitImageToWindowHeight()));
    connect(m_RenderWindow, SIGNAL(zoomChanged(double)), this, SLOT(zoom(double)));
    connect(m_RenderWindow, SIGNAL(panChanged(double,double)), this, SLOT(pan(double, double)));
    // About Actions
    connect(ui.aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void UIApplication::showEvent(QShowEvent* event) {
    // start() will be called as soon as the program returns to the event loop after executing show()
    QTimer::singleShot(0, this, SLOT(start()));
    event->accept();
}

void UIApplication::start() {
    if (mStarted)
        return;
    // starting timer (to compute 'IN' msgs every N ms)
    m_timerID = QObject::startTimer(40);
    // Registering nodes
    CacheNode::ptr ca = CacheNode::ptr(new CacheNode());
    m_Manager.addNode(ca, m_Session);
    SceneNode::ptr sc = SceneNode::ptr(new SceneNode());
    m_Manager.addNode(sc, m_Session);
    TransportNode::ptr t = TransportNode::ptr(new TransportNode());
    m_Manager.addNode(t, m_Session);
    FitNode::ptr f = FitNode::ptr(new FitNode());
    m_Manager.addNode(f, m_Session);
    GradingNode::ptr g = GradingNode::ptr(new GradingNode());
    m_Manager.addNode(g, m_Session);
    InfoNode::ptr info = InfoNode::ptr(new InfoNode());
    m_Manager.addNode(info, m_Session);
    PlaybackNode::ptr pb = PlaybackNode::ptr(new PlaybackNode());
    m_Manager.addNode(pb, m_Session);
    // starting session
    m_Session->start(m_RenderWindow->renderWindowID());
    mStarted = true;
}

void UIApplication::topLevelChanged(bool b) {
    QDockWidget *dockwidget = qobject_cast<QDockWidget *> (sender());
    if (dockwidget && b) {
        dockwidget->setWindowOpacity(0.6);
        dockwidget->move(mapToGlobal(m_RenderWindow->renderWidget()->pos()) + QPoint(40, 60));
        dockwidget->adjustSize();
    }
}

// private
void UIApplication::closeEvent(QCloseEvent *event) {
    if (m_TimelineUI) {
        m_Session->removeObserver(m_TimelineUI);
        delete m_TimelineUI;
    }
    if (m_ImageInfoUI) {
        m_Session->removeObserver(m_ImageInfoUI);
        delete m_ImageInfoUI;
    }
    if(m_timerID != 0)
        QObject::killTimer(m_timerID);
    m_Session->stop();
    m_Manager.clearNodes();
    m_Preferences.saveShortcuts(this);
    m_Preferences.saveFileHistory();
    QMainWindow::closeEvent(event);
    event->accept();
}

// private
void UIApplication::timerEvent(QTimerEvent *event) {
    // retrieve in msgs
    m_Session->receiveMsg();
    event->accept();
}

// private
void UIApplication::keyPressEvent(QKeyEvent * event) {
    QMainWindow::keyPressEvent(event);
    switch (event->key()) {
        case Qt::Key_Escape:
            close();
            break;
        default:
            break;
    }
    event->accept();
}

// private
void UIApplication::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

// private
void UIApplication::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

// private
void UIApplication::dropEvent(QDropEvent *event) {
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

// private
void UIApplication::resizeCentralWidget(const QSize& resolution) {
    QSize renderRes = m_RenderWindow->size();
    if (resolution == renderRes)
        return;
    QList<int> sizes;
    QObjectList childs = children();
    // Fix size of all children widget
    for (int i = 0; i < childs.size(); ++i) {
        QDockWidget* dock = qobject_cast<QDockWidget*> (childs[i]);
        if (!dock || dock->isFloating())
            continue;
        switch (dockWidgetArea(dock)) {
            case Qt::LeftDockWidgetArea:
            case Qt::RightDockWidgetArea: // Constraint width
                sizes.append(dock->minimumWidth());
                sizes.append(dock->maximumWidth());
                dock->setFixedWidth(dock->width());
                break;
            case Qt::TopDockWidgetArea:
            case Qt::BottomDockWidgetArea: // Constraint height
                sizes.append(dock->minimumHeight());
                sizes.append(dock->maximumHeight());
                dock->setFixedHeight(dock->height());
                break;
            default:
                break;
        }
    }
    // Resize renderer to the new resolution and prevent resize event
    m_RenderWindow->blockSignals(true);
    m_RenderWindow->setFixedSize(resolution);
    m_RenderWindow->blockSignals(false);
    // resize its parent (QMainWindow)
    adjustSize();
    // Allow user to resize renderer
    m_RenderWindow->setMinimumSize(0, 0);
    m_RenderWindow->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    // Restore min & max size of all children widget
    for (int i = 0; i < childs.size(); ++i) {
        QDockWidget* dock = qobject_cast<QDockWidget*> (childs[i]);
        if (!dock || dock->isFloating())
            continue;
        switch (dockWidgetArea(dock)) {
            case Qt::LeftDockWidgetArea:
            case Qt::RightDockWidgetArea:
                dock->setMinimumWidth(sizes.takeFirst());
                dock->setMaximumWidth(sizes.takeFirst());
                break;
            case Qt::TopDockWidgetArea:
            case Qt::BottomDockWidgetArea:
                dock->setMinimumHeight(sizes.takeFirst());
                dock->setMaximumHeight(sizes.takeFirst());
                break;
            default:
                break;
        }
    }
}

void UIApplication::updateRecentFilesMenu() {
    ui.openRecentMenu->clear();
    for (size_t i = 0; i < m_Preferences.history().size(); ++i) {
        if (m_Preferences.history(i) == "")
            continue;
        boost::filesystem::path fn(m_Preferences.history(i));
        QAction * act = ui.openRecentMenu->addAction(m_Preferences.history(i).c_str());
        act->setData(m_Preferences.history(i).c_str());
        connect(act, SIGNAL(triggered()), this, SLOT(openRecent()));
    }
}

// private slot
void UIApplication::openFiles(const QStringList & _list, const bool & browseMode, const bool & parseSequence) {
    SceneNode::ptr s = m_Manager.nodeByName<SceneNode> ("fr.mikrosimage.dukex.scene");
    if (s.get() == NULL)
        return;
    if (!_list.isEmpty()) {
        // --- QStringList to STL vector<string>
        std::vector<std::string> v;
        v.resize(_list.count());
        for (int i = 0; i < _list.count(); ++i) {
            v[i] = _list[i].toStdString();
        }
        s->openFiles(v, browseMode);
        if (v.size() == 1) { // multi selection not handled in file history
            QString history = _list[0];
            if (browseMode) {
                history.prepend("browse://");
            } else {
                if (parseSequence)
                    history.prepend("sequence://");
                else
                    history.prepend("file://");
            }
            m_Preferences.addToHistory(history.toStdString());
            updateRecentFilesMenu();
        }
    }
}

// private slot
void UIApplication::openFiles() {
    QStringList list;
    if (m_FileDialog->exec()) {
        list = m_FileDialog->selectedFiles();
        if (list.size() != 0)
            openFiles(list, false, m_FileDialog->asSequence());
    }
}

// private slot
void UIApplication::openRecent() {
    QAction *action = qobject_cast<QAction *> (sender());
    if (action) {
        QString file = action->data().toString();
        if (!file.isEmpty()) {
            QStringList filenames;
            if (file.startsWith("sequence://")) {
                file.remove(0, 11);
                filenames.append(file);
                openFiles(filenames, false, true);
            } else if (file.startsWith("browse://")) {
                file.remove(0, 9);
                filenames.append(file);
                openFiles(filenames, true, false);
            } else { // "file://"
                file.remove(0, 7);
                filenames.append(file);
                openFiles(filenames, false, false);
            }
        }
    }
}

// private slot
void UIApplication::browseDirectory() {
    QStringList list;
    if (m_FileDialog->exec()) {
        list = m_FileDialog->selectedFiles();
        if (list.size() != 0)
            openFiles(list, true, m_FileDialog->asSequence());
    }
}

// private slot
void UIApplication::savePlaylist() {
    QString file = QFileDialog::getSaveFileName(this, tr("Save Current Playlist"), QString(), tr("Duke Playlist (*.dk);;All(*)"));
    if (!file.isEmpty()) {
        if (!file.endsWith(".dk"))
            file.append(".dk");
        SceneNode::ptr scene = m_Manager.nodeByName<SceneNode> ("fr.mikrosimage.dukex.scene");
        if (scene.get() == NULL)
            return;
        scene->save(file.toStdString());
    }
}

// private slot
void UIApplication::openPreferences() {
    if (!m_SettingsUI)
        m_SettingsUI = new UISettingsDialog(this);
    m_SettingsUI->show();
}

// private slot
void UIApplication::playStop() {
    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
    if (t.get() == NULL)
        return;
    if (m_Session->descriptor().isPlaying())
        t->stop();
    else
        t->play();
}

// private slot
void UIApplication::previousFrame() {
    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
    if (t.get() == NULL)
        return;
    t->previousFrame();
}

// private slot
void UIApplication::nextFrame() {
    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
    if (t.get() == NULL)
        return;
    t->nextFrame();
}

// private slot
void UIApplication::firstFrame() {
    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
    if (t.get() == NULL)
        return;
    t->firstFrame();
}

// private slot
void UIApplication::lastFrame() {
    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
    if (t.get() == NULL)
        return;
    t->lastFrame();
}

// private slot
void UIApplication::previousShot() {
    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
    if (t.get() == NULL)
        return;
    t->previousShot();
}

// private slot
void UIApplication::nextShot() {
    TransportNode::ptr t = m_Manager.nodeByName<TransportNode> ("fr.mikrosimage.dukex.transport");
    if (t.get() == NULL)
        return;
    t->nextShot();
}

// private slot
void UIApplication::colorspaceLIN() {
    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
    if (g.get() == NULL)
        return;
    g->setColorspace(::duke::playlist::Display::LIN);
}

// private slot
void UIApplication::colorspaceLOG() {
    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
    if (g.get() == NULL)
        return;!
    g->setColorspace(::duke::playlist::Display::LOG);
}

// private slot
void UIApplication::colorspaceSRGB() {
    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
    if (g.get() == NULL)
        return;
    g->setColorspace(::duke::playlist::Display::SRGB);
}

void UIApplication::timeline() {
    if (!m_TimelineUI) {
        m_TimelineUI = new UITimeline(&m_Manager);
        m_Session->addObserver(m_TimelineUI);
        addDockWidget(Qt::BottomDockWidgetArea, m_TimelineUI);
    }
    m_TimelineUI->setVisible(!m_TimelineUI->isVisible());
}

void UIApplication::imageInfo() {
    if (!m_ImageInfoUI) {
        m_ImageInfoUI = new UIImageInfo(&m_Manager);
        m_Session->addObserver(m_ImageInfoUI);
        addDockWidget(Qt::RightDockWidgetArea, m_ImageInfoUI);
    }
    m_ImageInfoUI->setVisible(!m_ImageInfoUI->isVisible());
}

// private slot
void UIApplication::fullscreen() {
    if (m_RenderWindow->isFullScreen()) {
        m_RenderWindow->showNormal();
    } else {
        m_RenderWindow->showFullScreen();
    }
}

// private slot
void UIApplication::toggleFitMode() {
    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
    if (f.get() == NULL)
        return;
    f->toggle();
}

// private slot
void UIApplication::fitToNormalSize() {
    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
    if (f.get() == NULL)
        return;
    f->fitToNormalSize();
}

// private slot
void UIApplication::fitImageToWindowWidth() {
    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
    if (f.get() == NULL)
        return;
    f->fitImageToWindowWidth();
}

// private slot
void UIApplication::fitImageToWindowHeight() {
    FitNode::ptr f = m_Manager.nodeByName<FitNode> ("fr.mikrosimage.dukex.fit");
    if (f.get() == NULL)
        return;
    f->fitImageToWindowHeight();
}

// private slot
void UIApplication::zoom(double z) {
    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
    if (g.get() == NULL)
        return;
    g->setZoom(z);
}

// private slot
void UIApplication::pan(double x, double y) {
    GradingNode::ptr g = m_Manager.nodeByName<GradingNode> ("fr.mikrosimage.dukex.grading");
    if (g.get() == NULL)
        return;
    g->setPan(x, y);
}

// private slot
void UIApplication::about() {
    QString msg(getVersion("DukeX").c_str());
    QMessageBox::about(this, tr("About DukeX"), msg);
}


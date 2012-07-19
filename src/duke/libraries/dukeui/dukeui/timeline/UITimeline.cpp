#include "UITimeline.h"
#include "UITracksView.h"
#include "UITracksScene.h"
#include "UITracksRuler.h"
#include <QHBoxLayout>
#include <QScrollBar>
#include <QCloseEvent>
#include <QPushButton>

UITimeline::UITimeline() {
    ui.setupUi(this);

    // objectname, used in qss
    setObjectName("timelineDock");

    setMinimumHeight(0);
    setMaximumHeight(30); // global height
    setTitleBarWidget(new QWidget(this)); // remove title bar

    mScene = new UITracksScene(this);
    mView = new UITracksView(mScene, this);
    mView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mRuler = new UITracksRuler(mView, this);
    ui.tracksLayout->addWidget(mView);
    ui.rulerLayout->addWidget(mRuler);

    // Cursor position Events
    connect(mRuler, SIGNAL(cursorMoved(qint64)), mView->tracksCursor(), SLOT(frameChanged(qint64)));
    connect(mView->tracksCursor(), SIGNAL(cursorMoved(qint64)), mRuler, SLOT(frameChanged(qint64)));
    // Pan Events
    connect(mView->horizontalScrollBar(), SIGNAL( valueChanged(int) ), mRuler, SLOT( panChanged(int) ));
    // Zoom Events
    connect(mView, SIGNAL( zoomChanged(double) ), mRuler, SLOT( zoomChanged(double) ));

    // tracks
    mView->addTrack();
    // items
    for(int i=0;i<=100;i+=50)
        mView->addItem("/path/to/sequence", i, i+50, 0, 0);
    setDuration(150);
}

void UITimeline::setDuration(int duration) {
    mView->setDuration(duration);
    mRuler->setDuration(duration);
}

void UITimeline::showEvent(QShowEvent* e) {
    mView->fit();
}


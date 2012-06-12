#include "UITracksView.h"
#include "UIGraphicsMovieItem.h"
#include "UIGraphicsCursorItem.h"
#include "UIGraphicsTrack.h"
#include <QGraphicsLinearLayout>
#include <QScrollBar>
#include <QTextStream>
#include <QDebug>
#include <iostream>

#define TRACKHEIGHT 15

UITracksView::UITracksView(QGraphicsScene *s, QWidget *p) :
                QGraphicsView(s, p), mNumTrack(0), mDuration(0) {
    // main properties
    setMouseTracking(true);
    setAcceptDrops(true);
    setContentsMargins(0, 0, 0, 0);
    setFrameStyle(QFrame::NoFrame);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setCacheMode(QGraphicsView::CacheBackground);

    // main layout
    mVerticalLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mVerticalLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mVerticalLayout->setContentsMargins(0, 0, 0, 0);
    mVerticalLayout->setSpacing(0);
    mVerticalLayout->setPreferredWidth(0);
    QGraphicsWidget *container = new QGraphicsWidget();
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    container->setContentsMargins(0, 0, 0, 0);
    container->setLayout(mVerticalLayout);

    // cursor line
    mCursorLine = new UIGraphicsCursorItem();
    scene()->addItem(mCursorLine);
    connect(mCursorLine, SIGNAL(cursorMoved(qint64)), this, SLOT(ensureCursorVisible()));

    // scrollbars
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QScrollBar:horizontal {"
                  "border: 0px;"
                  "background: #666666;"
                  "height: 1px;"
                  "margin: 0px 0px 0 0px;"
                  "}");
}

void UITracksView::addTrack() {
    UIGraphicsTrack *track = new UIGraphicsTrack(++mNumTrack);
    track->setHeight(TRACKHEIGHT);
    mVerticalLayout->insertItem(0, track);
    mVerticalLayout->activate();
//    mCursorLine->setHeight(mVerticalLayout->contentsRect().height());
    mCursorLine->setHeight(TRACKHEIGHT * mNumTrack);
    scene()->invalidate(); // Redraw the background
}

void UITracksView::addItem(const QString& path, qint64 recin, qint64 recout, qint64 srcin, qint64 srcout) {
    UIGraphicsMovieItem *item = new UIGraphicsMovieItem(path, recin, recout, srcin, srcout);
    scene()->addItem(item);
    item->setPos(QPointF(recin, 0));
}

void UITracksView::clear() {
    while (mVerticalLayout->count() > 0)
        delete mVerticalLayout->itemAt(0);
    mNumTrack = 0;
    setDuration(0);
    addTrack();
}

void UITracksView::fit() {
    int viewsize = viewport()->rect().width();
    QTransform q;
    q.scale((viewsize > mDuration) ? viewsize / (float)mDuration : mDuration/(float)viewsize, 1.0);
    setTransform(q);
    emit zoomChanged(matrix().m11());

    setCenter(QPointF(mDuration / 2., 0));
}

void UITracksView::ensureCursorVisible() {
    int margin;
    if (mCursorLine->cursorPos() > getCenter().x()) {
        // handle right (max) limit
        margin = qMin((qint64) 40, mDuration - mCursorLine->cursorPos());
    } else {
        // handle left (min) limit
        margin = qMin(40, mCursorLine->cursorPos());
    }
    QRectF visibleArea = visibleRect();
    ensureVisible(mCursorLine->cursorPos(), visibleArea.y() + (visibleArea.height() / 2), 1, 1, margin, 0);
}

QRectF UITracksView::visibleRect() {
    QPointF topLeft(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPointF bottomRight(topLeft + viewport()->rect().bottomRight());
    QMatrix reverted = matrix().inverted();
    return reverted.mapRect(QRectF(topLeft, bottomRight));
}

void UITracksView::setDuration(int duration) {
    mDuration = duration;
    mCursorLine->setDuration(mDuration);
}

void UITracksView::setCenter(const QPointF& centerPoint) {
    // get the rectangle of the visible area in scene coords
    QRectF visibleArea = mapToScene(rect()).boundingRect();
    // get the scene area
    QRectF sceneBounds = sceneRect();
    double boundX = visibleArea.width() / 2.0;
    double boundY = visibleArea.height() / 2.0;
    double boundWidth = sceneBounds.width() - 2.0 * boundX;
    double boundHeight = sceneBounds.height() - 2.0 * boundY;
    // the max boundary that the centerPoint can be to
    QRectF bounds(boundX, boundY, boundWidth, boundHeight);
    if (bounds.contains(centerPoint)) {
        // we are within the bounds
        mCurrentCenterPoint = centerPoint;
    } else {
        // we need to clamp or use the center of the screen
        if (visibleArea.contains(sceneBounds)) {
            // use the center of scene ie. we can see the whole scene
            mCurrentCenterPoint = sceneBounds.center();
        } else {
            mCurrentCenterPoint = centerPoint;
            // we need to clamp the center. The centerPoint is too large
            if (centerPoint.x() > bounds.x() + bounds.width()) {
                mCurrentCenterPoint.setX(bounds.x() + bounds.width());
            } else if (centerPoint.x() < bounds.x()) {
                mCurrentCenterPoint.setX(bounds.x());
            }
            if (centerPoint.y() > bounds.y() + bounds.height()) {
                mCurrentCenterPoint.setY(bounds.y() + bounds.height());
            } else if (centerPoint.y() < bounds.y()) {
                mCurrentCenterPoint.setY(bounds.y());
            }

        }
    }
    // update the scrollbars
    centerOn(mCurrentCenterPoint);
}

void UITracksView::wheelEvent(QWheelEvent* event) {
    // get the position of the mouse before scaling, in scene coords
    QPointF pointBeforeScale(mapToScene(event->pos()));
    // get the original screen centerpoint
    QPointF screenCenter = getCenter(); //mCurrentCenterPoint; //(visRect.center());
    // scale the view ie. do the zoom
    double scaleFactor = 1.15;
    if (event->delta() > 0) {
        if(qRound(visibleRect().width()) < mDuration) // zoomOut limit
            scale(1.0 / scaleFactor, 1.0);
    } else {
        if(qRound(visibleRect().width()) > 100) // zoomIn limit
            scale(scaleFactor, 1.0);
    }

    emit zoomChanged(matrix().m11());

    // get the position after scaling, in scene coords
    QPointF pointAfterScale(mapToScene(event->pos()));
    // get the offset of how the screen moved
    QPointF offset = pointBeforeScale - pointAfterScale;
    // adjust to the new center for correct zooming
    QPointF newCenter = screenCenter + offset;
    setCenter(newCenter);
}

void UITracksView::mousePressEvent(QMouseEvent* event) {
    mLastPanPoint = event->pos();
    setCursor(Qt::ClosedHandCursor);
}

void UITracksView::mouseReleaseEvent(QMouseEvent* event) {
    setCursor(Qt::OpenHandCursor);
    mLastPanPoint = QPoint();
}

void UITracksView::mouseMoveEvent(QMouseEvent* event) {
    if (!mLastPanPoint.isNull()) {
        // get how much we panned
        QPointF delta = mapToScene(mLastPanPoint) - mapToScene(event->pos());
        mLastPanPoint = event->pos();
        // update the center ie. do the pan
        QPointF newCenter = QPointF(getCenter().x(), -10.f); // limit to X axis
        setCenter(newCenter + delta);
    }
}

void UITracksView::resizeEvent(QResizeEvent * event) {
    fit();
}

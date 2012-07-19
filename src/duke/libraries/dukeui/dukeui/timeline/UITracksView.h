#ifndef UITRACKSVIEW_H
#define UITRACKSVIEW_H

#include "UIGraphicsCursorItem.h"
#include <QGraphicsView>

// forward declaration
class QGraphicsLinearLayout;
class ItemPosition;

class UITracksView : public QGraphicsView {

    Q_OBJECT

public:
    UITracksView(QGraphicsScene *scene, QWidget *parent = 0);

public:
    inline UIGraphicsCursorItem * tracksCursor() const {
        return mCursorLine;
    }
    void setDuration(int duration);
    void addTrack();
    void addItem(const QString& path, qint64 recin, qint64 recout, qint64 srcin, qint64 srcout);
    void clear();
    void fit();

private:
    QRectF visibleRect();
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void resizeEvent (QResizeEvent * event);
    void setCenter(const QPointF& centerPoint);
    QPointF getCenter() { return mCurrentCenterPoint; }

signals:
    void zoomChanged(double zoomfactor);

private slots:
    void ensureCursorVisible();


private:
    QGraphicsLinearLayout* mVerticalLayout;
    UIGraphicsCursorItem *mCursorLine;
    QPointF mCurrentCenterPoint;
    QPoint mLastPanPoint;
    int mNumTrack;
    qint64 mDuration;
};


#endif // UITRACKSVIEW_H

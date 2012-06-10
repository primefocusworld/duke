#include "UIGraphicsMovieItem.h"
#include "UITracksView.h"
#include "UITracksScene.h"
#include "UITimeline.h"
#include <QTime>

#define CORNERRADIUS 2

UIGraphicsMovieItem::UIGraphicsMovieItem(const QString& path, qint64 recin, qint64 recout, qint64 srcin, qint64 srcout) :
                mWidth(recout - recin) {
}

void UIGraphicsMovieItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    QRectF drawRect;

    // Disable the matrix transformations
    painter->setWorldMatrixEnabled(false);
    painter->setRenderHint(QPainter::Antialiasing);

    // Get the transformations required to map the text on the viewport
    QList<QGraphicsView *> views = scene()->views();
    QTransform viewPortTransform = views[0]->viewportTransform();
    //QTransform viewPortTransform = Timeline::getInstance()->tracksView()->viewportTransform();

    drawRect = boundingRect();

    // Do the transformation
    QRectF mapped = deviceTransform(viewPortTransform).mapRect(drawRect);
    QLinearGradient gradient(mapped.topLeft(), mapped.bottomLeft());
    gradient.setColorAt(0, QColor::fromRgb(78, 78, 78));
    gradient.setColorAt(0.4, QColor::fromRgb(72, 72, 72));
    gradient.setColorAt(0.4, QColor::fromRgb(50, 50, 50));
    gradient.setColorAt(0.6, QColor::fromRgb(45, 45, 45));
    painter->setPen(Qt::black);
    painter->setBrush(QBrush(gradient));

    painter->drawRoundedRect(mapped, CORNERRADIUS, CORNERRADIUS);
}

QRectF UIGraphicsMovieItem::boundingRect() const {
    return QRectF(0, 0, (qreal) mWidth, 15);
}

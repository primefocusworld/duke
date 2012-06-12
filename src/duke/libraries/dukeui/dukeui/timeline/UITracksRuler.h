#ifndef UITRACKSRULER_H
#define UITRACKSRULER_H

#include "UITracksView.h"
#include <QWidget>
#include <QString>
#include <set>

// forward declaration
class QPaintEvent;
class QStylePainter;

class UITracksRuler : public QWidget {

    Q_OBJECT

public:
    UITracksRuler(UITracksView* tracksView, QWidget* parent = 0);

public:
    void setDuration(int duration);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

public slots:
    void frameChanged(qint64 position);
    void zoomChanged(double zoomfactor);
    void panChanged(int panfactor);

signals:
    void cursorMoved(qint64 pos);

private:
    void drawTickText(QStylePainter & painter, const QString txt, int fframe, int fpos);

private:
    UITracksView* m_tracksView;
    float m_fps;
    qint64 m_duration;
    qint64 m_pos;
    double m_zoomFactor;
    int m_panFactor;
};

#endif // UITRACKSRULER_H

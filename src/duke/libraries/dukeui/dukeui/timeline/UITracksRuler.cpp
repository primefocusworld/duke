#include "UITracksRuler.h"
#include "UITracksView.h"
#include <QStylePainter>
#include <QPaintEvent>
#include <QDebug>
#include <iostream>

#define MAXHEIGHT 14
#define LITTLEMARKHEIGHT 2
#define MIDDLEMARKHEIGHT 5
#define BIGMARKHEIGHT 10

UITracksRuler::UITracksRuler(UITracksView* tracksView, QWidget* parent) :
                QWidget(parent), m_tracksView(tracksView), m_fps(25.0), m_duration(0), m_pos(0), m_zoomFactor(1.), m_panFactor(0) {

    setMinimumHeight(MAXHEIGHT);
    setMaximumHeight(MAXHEIGHT);
}

void UITracksRuler::setDuration(int duration) {
    m_duration = duration;
    update();
}

void UITracksRuler::paintEvent(QPaintEvent* e) {
    QStylePainter painter(this);
    painter.setClipRect(e->rect());

    QColor color(78, 78, 78, 255);
    painter.setPen(color);
    painter.setBrush(QBrush(color));
    painter.setFont(QFont("Arial", 11));

    int fpos = (int) (m_pos * m_zoomFactor - m_panFactor);
    int fend = (int) (m_duration * m_zoomFactor - m_panFactor);

    // draw active area
    //if (end - m_panFactor > 1)
    //    painter.fillRect(0, 0, end - m_panFactor, height(), QBrush(QColor(77, 82, 95, 50)));

    // draw cursor (frame #)
    QRectF rect;
    rect = painter.boundingRect(rect, Qt::AlignHCenter, QString::number(m_pos));
    QPointF textpos(m_pos * m_zoomFactor - m_panFactor - rect.width() / 2., MAXHEIGHT - (rect.height() / 4.));
    painter.drawText(textpos, QString::number(m_pos));

    // draw marks
    int littlemarkevery = 5;
    int middlemarkevery = 25;
    int bigmarkevery = 100;
    for (int f = 0; f < fend; f += littlemarkevery) {
        int ff = f * m_zoomFactor - m_panFactor;
        if (f % bigmarkevery <= 0) {
            painter.drawLine(ff, MAXHEIGHT - BIGMARKHEIGHT, ff, MAXHEIGHT);
            drawTickText(painter, QString::number(f), ff, fpos);
        } else if (f % middlemarkevery == 0) {
            painter.drawLine(ff, MAXHEIGHT - MIDDLEMARKHEIGHT, ff, MAXHEIGHT);
            drawTickText(painter, QString::number(f), ff, fpos);
        } else if (f % littlemarkevery == 0)
            painter.drawLine(ff, MAXHEIGHT - LITTLEMARKHEIGHT, ff, MAXHEIGHT);
    }

}

void UITracksRuler::mouseDoubleClickEvent(QMouseEvent* event) {
    update();
}

void UITracksRuler::mousePressEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit cursorMoved((event->x() + m_panFactor) / m_zoomFactor);
    }
    update();
}

void UITracksRuler::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit cursorMoved((event->x() + m_panFactor) / m_zoomFactor);
    }
    update();
}

void UITracksRuler::frameChanged(qint64 frame) {
    m_pos = frame;
    update();
}

void UITracksRuler::zoomChanged(double zoomfactor) {
    m_zoomFactor = zoomfactor;
    update();
}

void UITracksRuler::panChanged(int panfactor) {
    m_panFactor = panfactor;
    update();
}

// private
void UITracksRuler::drawTickText(QStylePainter & painter, const QString txt, int fframe, int fpos) {
    painter.save();
    QRectF rect = painter.boundingRect(rect, Qt::AlignHCenter, txt);
    //if (fpos > (fframe - rect.width() - 10) && fpos < fframe + 10) // tick behind cursor
    painter.setPen(QColor(78, 78, 78, 150));
    QPointF textpos(fframe - rect.width() - 1, MAXHEIGHT - (rect.height() / 4.));
    painter.drawText(textpos, txt);
    painter.restore();
}

